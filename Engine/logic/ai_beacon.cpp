#include "mcv_platform.h"
#include "ai_beacon.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\comp_name.h"
#include "components\comp_light_dir_shadows.h"
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
  beacon_light = atts.getString("beacon_light", "");
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

    TCompTransform *me_transform = myEntity->get<TCompTransform>();
    VEC3 lookingPoint = me_transform->getPosition() + me_transform->getFront()*range;

    if (beacon_light != "") {
      for (CHandle aux : tags_manager.getHandlesByTag(getID("beacon_light"))) {
        CEntity * auxe = aux;
        if (auxe) {
          TCompName * n = auxe->get<TCompName>();
          if (n->name == beacon_light) {
            TCompLightDirShadows * compl = auxe->get<TCompLightDirShadows>();
            TCompTransform * compt = auxe->get<TCompTransform>();
            VEC3 lpos = compt->getPosition();
            lpos.x += me_transform->getFront().x / 2;
            lpos.z += me_transform->getFront().z / 2;
            compl->smoothLookAt(lpos, lookingPoint);
            compl->activate();
            break;
          }
        }
      }
    }
  }
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

  VEC3 lookingPoint = me_transform->getPosition() + me_transform->getFront()*range;

  if (beacon_light != "") {
    for (CHandle aux : tags_manager.getHandlesByTag(getID("beacon_light"))) {
      CEntity * auxe = aux;
      if (auxe) {
        TCompName * n = auxe->get<TCompName>();
        if (n->name == beacon_light) {
          TCompLightDirShadows * compl = auxe->get<TCompLightDirShadows>();
          TCompTransform * compt = auxe->get<TCompTransform>();
          VEC3 lpos = compt->getPosition();
          lpos.x += me_transform->getFront().x/2;
          lpos.z += me_transform->getFront().z/2;
          compl->smoothLookAt(lpos, lookingPoint);
          compl->activate();
          break;
        }
      }
    }
  }

  // player detection
  CHandle hPlayer = tags_manager.getFirstHavingTag("raijin");
  CEntity * eplayer = hPlayer;
  TCompTransform * tplayer = eplayer->get<TCompTransform>();
  if (realDistXZ(tplayer->getPosition(), lookingPoint) < width) {
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

  VEC3 lookingPoint = me_transform->getPosition() + me_transform->getFront()*range;

  if (beacon_light != "") {
    CHandle aux = tags_manager.getHandleByTagAndName(beacon_light.c_str(), "beacon_light");
    CEntity * auxe = aux;
    if (auxe) {
      TCompName * n = auxe->get<TCompName>();
      if (n->name == beacon_light) {
        TCompLightDirShadows * compl = auxe->get<TCompLightDirShadows>();
        TCompTransform * compt = auxe->get<TCompTransform>();
        VEC3 lpos = compt->getPosition();
        lpos.x += me_transform->getFront().x / 2;
        lpos.z += me_transform->getFront().z / 2;
        compl->smoothLookAt(lpos, lookingPoint);
        compl->activate();
      }
    }
  }

  if (realDistXZ(tplayer->getPosition(), lookingPoint) < width) {
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