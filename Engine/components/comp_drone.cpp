#include "mcv_platform.h"
#include "comp_drone.h"

#include "comp_transform.h"
#include "comp_physics.h"
#include "comp_life.h"
#include "handle\handle.h"
#include "player_controllers/player_controller_cientifico.h"
#include "entity.h"

void TCompDrone::onCreate(const TMsgEntityCreated &)
{
  CHandle h = CHandle(this).getOwner();
  CEntity *e = h;
  if (e) {
    //Init Waypoints (and add the last point the initial position)
    TCompTransform *t = e->get<TCompTransform>();
    wpts[wpts.size() - 1] = t->getPosition();
    waitTimes[waitTimes.size() - 1] = 0;
    curWpt = 0;

    // Set Kinematic
    TCompPhysics *p = e->get<TCompPhysics>();
    p->setKinematic(true);
    final_pos = t->getPosition();

    // Set life
    TCompLife *l = e->get<TCompLife>();
    if (l) {
      l->player_life = false;
    }
  }
  else h.destroy();
}

void TCompDrone::onRecharge(const TMsgActivate &)
{
  CHandle h = CHandle(this).getOwner();
  CEntity *e = h;
  if (e) {
    // Reset life
    TCompLife *l = e->get<TCompLife>();
    if (l) {
      l->currentlife = l->maxlife;
      playerInDistance = false;
      CanRechargeDrone(playerInDistance);
    }
  }
}

void TCompDrone::onRepair(const TMsgRepair &)
{
  CHandle h = CHandle(this).getOwner();
  CEntity *e = h;
  if (e) {
    espatllat = false;
  }
}

bool TCompDrone::SetMyBasicComponents()
{
  CHandle h = CHandle(this).getOwner();
  CEntity *e = h;
  if (!e) return false;
  transform = e->get<TCompTransform>();
  physics = e->get<TCompPhysics>();
  life = e->get<TCompLife>();
  return true;
}

void TCompDrone::update(float elapsed)
{
  if (!SetMyBasicComponents()) return;
  if (!espatllat && (!life || life->currentlife > 0.0f)) {
    if (life) { life->currentlife -= elapsed; }
    if (timeToWait > 0) {
      //Waiting
      timeToWait -= elapsed;
    }
    else {
      if (simpleDist(wpts[curWpt], transform->getPosition()) < mEpsilon) {
        //Arrived
        curWpt = (curWpt + 1) % wpts.size();
        timeToWait = waitTimes[curWpt];
      }
      else {
        //Move to next
        moveToNext(elapsed);
      }
    }
  }
  else {
    // Going down
    VEC3 direction = VEC3(0.0f, -1.0f, 0.0f);
    VEC3 npcPos = transform->getPosition();

    CHandle player = tags_manager.getFirstHavingTag(getID("raijin"));
    CEntity *player_e = player;
    TCompTransform * p_transform = player_e->get<TCompTransform>();
    float distToDrone = realDist(npcPos, p_transform->getPosition());

    npcPos.y -= 1.0f;
    PxRaycastBuffer hit;
    bool ret = g_PhysxManager->raycast(npcPos, direction, fabsf(fallingSpeed * elapsed) + .50f, hit);
    if (!hit.hasAnyHits()) {
      final_pos = final_pos + direction * fallingSpeed * elapsed;
    }
    else {
      CHandle scih;
      CEntity * scie = nullptr;
      for (CHandle sci : tags_manager.getHandlesByTag(getID("AI_cientifico"))) {
        CEntity * sci_e = sci;
        player_controller_cientifico * sci_e_controller = sci_e->get<player_controller_cientifico>();
        if (sci_e_controller && sci_e_controller->npcIsPossessed) {
          scih = sci;
          scie = sci_e;
          break;
        }
      }

      float sciDist = 99.9f;
      if (scie) {
        TCompTransform * scie_transform = scie->get<TCompTransform>();
        sciDist = realDist(transform->getPosition(), scie_transform->getPosition());
      }

      if (!espatllat && !playerInDistance && distToDrone < 3.0f) {
        playerInDistance = true;
        CanRechargeDrone(playerInDistance);
      }
      else if (!espatllat && playerInDistance && distToDrone > 3.0f) {
        playerInDistance = false;
        CanRechargeDrone(playerInDistance);
      }
      else if (espatllat && !playerInDistance && distToDrone < 3.0f) {
        playerInDistance = true;
        CanNotRechargeDrone(playerInDistance);
      }
      else if (espatllat && playerInDistance && distToDrone > 3.0f) {
        playerInDistance = false;
        CanNotRechargeDrone(playerInDistance);
      }
      else if (scie && espatllat && !sciInDistance && sciDist < 3.0f) {
        sciInDistance = true;
        CanRepairDrone(scih, sciInDistance);
      }
      else if (scie && espatllat && sciInDistance && sciDist > 3.0f) {
        sciInDistance = false;
        CanRepairDrone(scih, sciInDistance);
      }
    }
  }
}

void TCompDrone::moveToNext(float elapsed) {
  PxRigidDynamic *rd = physics->getActor()->isRigidDynamic();
  if (rd) {
    VEC3 direction = wpts[curWpt] - final_pos;
    direction.Normalize();
    final_pos = final_pos + direction * speed * elapsed;
  }
}

// Loading the wpts
#define WPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d_%s", index, nameSufix);

bool TCompDrone::load(MKeyValue & atts)
{
  espatllat = atts.getBool("espatllat", false);
  int n = atts.getInt("wpts_size", 0);
  wpts.resize(n + 1);
  waitTimes.resize(n + 1);
  for (int i = 0; i < n; i++) {
    WPT_ATR_NAME(atrPos, "pos", i);
    WPT_ATR_NAME(atrWait, "wait", i);
    wpts[i] = atts.getPoint(atrPos);
    waitTimes[i] = atts.getFloat(atrWait, 0);
  }
  return true;
}

void TCompDrone::fixedUpdate(float elapsed) {
  PxRigidDynamic *rd = physics->getActor()->isRigidDynamic();
  if (rd) {
    PxTransform tmx = rd->getGlobalPose();
    VEC3 target = final_pos;
    PxVec3 pxTarget = PhysxConversion::Vec3ToPxVec3(target);
    rd->setKinematicTarget(PxTransform(pxTarget, tmx.q));
  }
}

void TCompDrone::CanRechargeDrone(bool new_range)
{
  TMsgCanRechargeDrone msg;
  msg.range = new_range;
  msg.han = CHandle(this).getOwner();
  CHandle player = tags_manager.getFirstHavingTag(getID("raijin"));
  CEntity *player_e = player;
  player_e->sendMsg(msg);
}
void TCompDrone::CanNotRechargeDrone(bool new_range)
{
  TMsgCanNotRechargeDrone msg;
  msg.range = new_range;
  msg.han = CHandle(this).getOwner();
  CHandle player = tags_manager.getFirstHavingTag(getID("raijin"));
  CEntity *player_e = player;
  player_e->sendMsg(msg);
}
void TCompDrone::CanRepairDrone(CHandle sci, bool new_range) {
  TMsgCanRechargeDrone msg;
  msg.range = new_range;
  msg.han = CHandle(this).getOwner();

  CEntity * scie = sci;
  scie->sendMsg(msg);
}

void TCompDrone::onTriggerEnter(const TMsgTriggerIn & msg)
{
  CHandle h_in = msg.other;
  if (h_in.hasTag("raijin")) {
    CanRechargeDrone(true);
  }
}

void TCompDrone::onTriggerExit(const TMsgTriggerOut & msg)
{
  CHandle h_in = msg.other;
  if (h_in.hasTag("raijin")) {
    CanRechargeDrone(false);
  }
}