#include "mcv_platform.h"
#include "ai_beacon.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_light_dir_shadows.h"
#include "components\entity.h"
#include "components\entity_tags.h"

map<string, statehandler> beacon_controller::statemap = {};

map<int, string> beacon_controller::out = {};

bool beacon_controller::load(MKeyValue& atts) {
  range = atts.getFloat("range", range);
  width = atts.getFloat("width", width);
  rot_speed_sonar = atts.getFloat("rot_speed_sonar", rot_speed_sonar);
  max_idle_waiting = atts.getFloat("max_idle_waiting", max_idle_waiting);
  return true;
}

void beacon_controller::Init() {
  //read main attributes from file

  full_name = "beacon_" + to_string(id_beacon);

  if (statemap.empty()) {
    AddState("idle", (statehandler)&beacon_controller::Idle);
    AddState("Rotating", (statehandler)&beacon_controller::Rotating);
    AddState("AimPlayer", (statehandler)&beacon_controller::AimPlayer);
  }
  om = getHandleManager<beacon_controller>();
  SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?
  SetMyEntity();
  idle_wait = 0.0f;

  TCompTransform *me_transform = myEntity->get<TCompTransform>();
  TCompLightDirShadows *lshd = myEntity->get<TCompLightDirShadows>();

  if (lshd) {
    VEC3 myposinitial = me_transform->getPosition();
    VEC3 origin = me_transform->getPosition();
    origin.x += me_transform->getFront().x / 2;
    origin.y += 2.5f;
    origin.z += me_transform->getFront().z / 2;
    VEC3 vec1 = origin - (myposinitial + me_transform->getFront()*(range + width));
    VEC3 vec2 = origin - (myposinitial + me_transform->getFront()*(range - width));

    float dot = vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
    float lenSq1 = vec1.x*vec1.x + vec1.y*vec1.y + vec1.z*vec1.z;
    float lenSq2 = vec2.x*vec2.x + vec2.y*vec2.y + vec2.z*vec2.z;
    float fov_in_rads = acos(dot / sqrt(lenSq1 * lenSq2));
    lshd->setNewFov(fov_in_rads);
  }
  else {
    fatal("camera needs a shadow camera component");
  }
  ChangeState("Rotating");
}

void beacon_controller::Idle() {
  //Nothing to do
  if (idle_wait > max_idle_waiting) {
    idle_wait = 0.0f;
    ChangeState("Rotating");
  }
  else {
    idle_wait += getDeltaTime();
  }
}
bool beacon_controller::playerInRange() {
  TCompTransform *me_transform = myEntity->get<TCompTransform>();
  // player detection
  CHandle hPlayer = tags_manager.getFirstHavingTag("raijin");
  CEntity * eplayer = hPlayer;
  TCompTransform * tplayer = eplayer->get<TCompTransform>();

  if (!me_transform->isHalfConeVision(tplayer->getPosition(), deg2rad(15.0f)) || squaredDist(tplayer->getPosition(), me_transform->getPosition()) > 100) {
    return false;
  }

  VEC3 destiny = me_transform->getPosition() + me_transform->getFront()*range;
  VEC3 origin = me_transform->getPosition();
  origin.x += me_transform->getFront().x / 2;
  origin.y += 2.5f;
  origin.z += me_transform->getFront().z / 2;

  VEC3 direction = origin - destiny;
  float height = origin.y - destiny.y;

  TCompCharacterController * cplayer = eplayer->get<TCompCharacterController>();

  float playerHeight = cplayer->GetHeight();

  VEC3 minplayerPos = tplayer->getPosition();
  VEC3 maxplayerPos = tplayer->getPosition();
  maxplayerPos.y += playerHeight;

  float factorMin = (height - (origin.y - minplayerPos.y)) / height;
  float factorMax = (height - (origin.y - maxplayerPos.y)) / height;

  VEC3 lookingPointMin = destiny + direction * factorMin;
  VEC3 lookingPointMax = destiny + direction * factorMax;

  if (realDistXZ(minplayerPos, lookingPointMin) < width || realDistXZ(maxplayerPos, lookingPointMax) < width) {
    //raycast to look for down distance
    PxRaycastBuffer hit;
    CHandle hanHitted;
    VEC3 playerPos = minplayerPos;
    VEC3 playerPosUp = maxplayerPos;
    playerPosUp.y -= 0.1;
    VEC3 playerPosDown = minplayerPos;
    playerPosDown.y += 0.1f;
    playerPos.y += playerHeight / 2;
    VEC3 playerPosRight = (playerPos + me_transform->getLeft() * (cplayer->GetRadius()*0.9));
    VEC3 playerPosLeft = (playerPos + me_transform->getLeft() * (cplayer->GetRadius()*0.9));

    // raycasts
#ifndef NDEBUG
    Debug->DrawLine(origin, playerPos, VEC3(1.0f, 0.0f, 0.0f));
    Debug->DrawLine(origin, playerPosUp, VEC3(1.0f, 0.0f, 0.0f));
    Debug->DrawLine(origin, playerPosDown, VEC3(1.0f, 0.0f, 0.0f));
    Debug->DrawLine(origin, playerPosRight, VEC3(1.0f, 0.0f, 0.0f));
    Debug->DrawLine(origin, playerPosLeft, VEC3(1.0f, 0.0f, 0.0f));
#endif

    // player center
    VEC3 d1 = playerPos - origin;
    float dist1 = d1.Length();
    d1.Normalize();
    bool hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) {
      hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
      if (hPlayer == hanHitted) {
        return true;
      }
    }
    // player up
    d1 = playerPosUp - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) {
      hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
      if (hPlayer == hanHitted) {
        return true;
      }
    }
    // player down
    d1 = playerPosDown - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) {
      hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
      if (hPlayer == hanHitted) {
        return true;
      }
    }
    // player left
    d1 = playerPosLeft - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) {
      hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
      if (hPlayer == hanHitted) {
        return true;
      }
    }
    // player right
    d1 = playerPosRight - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) {
      hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
      if (hPlayer == hanHitted) {
        return true;
      }
    }
  }
  return false;
}

void beacon_controller::Rotating() {
  SetMyEntity(); //needed in case address Entity moved by handle_manager
  if (!myEntity) return;
  if (!active) {
    idle_wait = 0.0f;
    ChangeState("idle");
  }
  TCompTransform *me_transform = myEntity->get<TCompTransform>();

#ifndef NDEBUG
  VEC3 myposinitial = me_transform->getPosition();
  VEC3 origin = myposinitial;
  origin.x += me_transform->getFront().x / 2;
  origin.z += me_transform->getFront().z / 2;
  origin.y += 2.5f;
  Debug->DrawLine(origin, (myposinitial + me_transform->getFront()*(range + width)), VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, (myposinitial + me_transform->getFront()*(range - width)), VEC3(1.0f, 0.0f, 0.0f));
#endif

  float yaw, pitch;
  me_transform->getAngles(&yaw, &pitch);
  me_transform->setAngles(yaw + rot_speed_sonar*getDeltaTime(), pitch);
  if (playerInRange()) {
    ChangeState("AimPlayer");
  }
}

void beacon_controller::AimPlayer()
{
  SetMyEntity(); //needed in case address Entity moved by handle_manager
  if (!myEntity) return;
  if (!active) {
    idle_wait = 0.0f;
    ChangeState("idle");
  }
  TCompTransform *me_transform = myEntity->get<TCompTransform>();
  CHandle hPlayer = tags_manager.getFirstHavingTag("raijin");
  CEntity * eplayer = hPlayer;
  TCompTransform * tplayer = eplayer->get<TCompTransform>();

  float yaw, pitch;
  me_transform->getAngles(&yaw, &pitch);

  float aimtoplayer = me_transform->getDeltaYawToAimTo(tplayer->getPosition());

  if (aimtoplayer > 0.0f) {
    aimtoplayer = fminf(aimtoplayer, rot_speed_sonar * getDeltaTime());
  }
  else {
    aimtoplayer = fmaxf(aimtoplayer, -rot_speed_sonar * getDeltaTime());
  }

  me_transform->setAngles(yaw + aimtoplayer, pitch);

  if (playerInRange()) {
    TMsgNoise msg;
    msg.source = tplayer->getPosition();
    for (CHandle guardHandle : tags_manager.getHandlesByTag(getID("AI_guard"))) {
      CEntity * ePoss = guardHandle;
      ePoss->sendMsg(msg);
    }
  }
  else {
    idle_wait = 0.0f;
    ChangeState("idle");
  }
}

void beacon_controller::onPlayerAction(TMsgBeaconBusy & msg)
{
  SetMyEntity();
  active = *(msg.reply);
}

void beacon_controller::renderInMenu()
{
  ImGui::Text("Node : %s", out[SBB::readInt(full_name)].c_str());
}

void beacon_controller::SetHandleMeInit()
{
  myHandle = om->getHandleFromObjAddr(this);
  myParent = myHandle.getOwner();
}

void beacon_controller::SetMyEntity() {
  myEntity = myParent;
}