#ifndef INC_DRON_H_
#define INC_DRON_H_

#include "comp_base.h"
#include "comp_trigger.h"

class TCompTransform;
class TCompPhysics;
struct TCompLife;

struct TCompDrone : public TTrigger {
  TCompTransform* transform;
  TCompPhysics* physics;
  TCompLife* life;

  std::vector<VEC3> wpts;
  std::vector<float> waitTimes;
  int curWpt;
  float speed = 1.0f;
  float fallingSpeed = 3.0f;
  float timeToWait = 0.f;
  float mEpsilon = .1f;
  bool playerInDistance = false;
  bool espatllat = false;

  std::string self_ilum_front;
  std::string self_ilum_back;

  VEC3 final_pos;

  void onCreate(const TMsgEntityCreated&);
  void onRecharge(const TMsgActivate &);
  void onRepair(const TMsgActivate &);
  void CanRechargeDrone(bool new_range);
  void CanNotRechargeDrone(bool new_range);

  void onTriggerInside(const TMsgTriggerIn& msg) {} //will do nothing, particles effect, ui message or something
  void onTriggerEnter(const TMsgTriggerIn& msg);
  void onTriggerExit(const TMsgTriggerOut& msg);
  void mUpdate(float dt) {}

  bool SetMyBasicComponents();
  void update(float elapsed);
  void moveToNext(float elapsed);
  bool load(MKeyValue& atts);
  void fixedUpdate(float elapsed);
  void renderInMenu() {
    ImGui::Text("I am a drone!!");
  }
};

#endif