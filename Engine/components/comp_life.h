#ifndef INC_COMPONENT_LIFE_H_
#define INC_COMPONENT_LIFE_H_

#include "comp_base.h"
#include "comp_msgs.h"

// ------------------------------------
struct TCompLife : public TCompBase {
  float life;
  TCompLife() : life(0.f) {
    dbg("ctor of TCompLife\n");
  }
  ~TCompLife() {
    dbg("dtor of TCompLife\n");
  }
  bool load(MKeyValue& atts) {
    life = atts.getFloat("points", 10.f);
    return true;
  }
  void onCreate(const TMsgEntityCreated&) {
    dbg("TCompLife on TMsgEntityCreated\n");
  }
  void renderInMenu() {
    ImGui::DragFloat("Life", &life, 0, 100.f);
  }
  void onDamage(const TMsgDamage& msg) {
    life -= msg.points;
    if (life < 0) {
      dbg("Me he muerto\n");
      CHandle(this).destroy();
      dbg("life is %f\n", life);
    }
  }
};

#endif

