#ifndef INC_COMPONENT_LIFE_H_
#define INC_COMPONENT_LIFE_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "entity_tags.h"
#include "entity.h"

// ------------------------------------
struct TCompLife : public TCompBase {
  float currentlife;
  float maxlife;
  float modifier = 1.0f;

  float energyDamageScale = 0.1f;
  float last_modif = energyDamageScale;

  bool dead = false;
  bool player_life = true;

  TCompLife() : maxlife(100.f) {
    dbg("constructor of TCompLife\n");
  }
  ~TCompLife() {
    dbg("destructor of TCompLife\n");
  }

  bool load(MKeyValue& atts) {
    maxlife = atts.getFloat("points", 100.0f);
    currentlife = maxlife;
    return true;
  }

  float getCurrent() {
    return currentlife;
  }

  float getCurrentNormalized() {
    return (currentlife / maxlife);
  }

  float getMax() {
    return maxlife;
  }

  void setCurrent(float new_life) {
    if (new_life > maxlife) maxlife = new_life;
    currentlife = new_life;
  }

  void onCreate(const TMsgEntityCreated &) {
    CHandle me = CHandle(this).getOwner();
    if (!me.isValid()) {
      fatal("Error creating life component\n");
      assert(false);
    }

    //init damage scales by time
    if (me.hasTag("player")) { //El player ya gestiona su damage
      energyDamageScale = 0.f;
      last_modif = 0.f;
    }

    //for NPC will gain life until full life if not possessed
    else if (me.hasTag("AI_poss")) {
      energyDamageScale = 0.0f;
      last_modif = -0.1f;
    }
  }

  void update(float elapsed) {
    if (player_life && !dead) {
      if (modifier != 0 && energyDamageScale != 0)
        currentlife -= energyDamageScale*modifier*elapsed;

      if (currentlife < 0 && !dead) {
        dead = true;
        GameController->SetGameState(CGameController::LOSE);
        currentlife = 0;
      }

      else if (currentlife > maxlife)
        currentlife = maxlife;
    }
  }

  void onDamage(const TMsgDamage& msg) {
    energyDamageScale = msg.modif;
  }

  void onReciveDamage(const TMsgSetDamage& msg) {
    currentlife -= msg.dmg;
  }

  void onStopDamage(const TMsgStopDamage& dmg) {
    energyDamageScale = last_modif;
  }

  void onSetSaveDamage(const TMsgDamageSave& msg) {
    last_modif = msg.modif;
    energyDamageScale = last_modif;
  }

  void setMaxLife(float max) {
    maxlife = max;
    currentlife = max;
  }

  void renderInMenu() {
    ImGui::SliderFloat("life", &currentlife, 0, 1);
    ImGui::SliderFloat("Action Damage Scale", &energyDamageScale, 0, 1);
  }
};

#endif
