#include "mcv_platform.h"
#include "ai_beacon.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_name.h"
#include "components\entity.h"
#include "components\entity_tags.h"

map<string, statehandler> beacon_controller::statemap = {};

map<int, string> beacon_controller::out = {};

void beacon_controller::readIniFileAttr() {
  CHandle h = CHandle(this).getOwner();
  if (h.isValid()) {
    if (h.hasTag("beacon")) {
      CApp &app = CApp::get();
      std::string file_ini = app.file_initAttr_json;
      map<std::string, float> fields = readIniAtrData(file_ini, "ai_beacon");

      assignValueToVar(range, fields);
      assignValueToVar(width, fields);
      assignValueToVar(rot_speed_sonar, fields);
      assignValueToVar(max_idle_waiting, fields);
    }
  }
}

bool beacon_controller::load(MKeyValue& atts) {
  //beacon_light = atts.getString("beacon_light", "");
  return true;
}

void beacon_controller::Init() {
  //read main attributes from file
  readIniFileAttr();

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

#ifndef NDEBUG
  // raycasts
  Debug->DrawLine(origin, destiny, VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*(range + width), VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*(range + width / 2), VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*(range - width), VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*(range - width / 2), VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*range + me_transform->getLeft() * width, VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*range + me_transform->getLeft() * (width / 2), VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*range - me_transform->getLeft() * width, VEC3(1.0f, 0.0f, 0.0f));
  Debug->DrawLine(origin, me_transform->getPosition() + me_transform->getFront()*range - me_transform->getLeft() * (width / 2), VEC3(1.0f, 0.0f, 0.0f));
#endif

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

    //center
    VEC3 d1 = destiny - origin;
    float dist1 = d1.Length();
    d1.Normalize();
    bool hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    // half front
    d1 = (me_transform->getPosition() + me_transform->getFront()*(range + width / 2)) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    //front
    d1 = (me_transform->getPosition() + me_transform->getFront()*(range + width)) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    //half back
    d1 = (me_transform->getPosition() + me_transform->getFront()*(range - width / 2)) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    //back
    d1 = (me_transform->getPosition() + me_transform->getFront()*(range - width)) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    // half left
    d1 = (me_transform->getPosition() + me_transform->getFront()*range + me_transform->getLeft() * (width / 2)) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    //left
    d1 = (me_transform->getPosition() + me_transform->getFront()*range + me_transform->getLeft() * width) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    //right
    d1 = (me_transform->getPosition() + me_transform->getFront()*range - me_transform->getLeft() * width) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
    // half right
    d1 = (me_transform->getPosition() + me_transform->getFront()*range - me_transform->getLeft() * (width / 2)) - origin;
    dist1 = d1.Length();
    d1.Normalize();
    hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
    if (hitted) return true;
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