#ifndef INC_COMPONENT_LIFE_H_
#define INC_COMPONENT_LIFE_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "entity_tags.h"
#include "entity.h"

// ------------------------------------
struct TCompLife : public TCompBase {
	static bool dead;
	float currentlife;
	float maxlife;
	float maxlife_inv;
	float modifier = 1.0f;

	float energyDamageScale = 0.1f;
	float last_modif = energyDamageScale;

	bool player_life = true;

	TCompLife() : maxlife(100.f) {
		dbg("constructor of TCompLife\n");
	}
	~TCompLife() {
		dbg("destructor of TCompLife\n");
	}

	bool load(MKeyValue& atts) {
		maxlife = atts.getFloat("points", 100.0f);
		maxlife_inv = 1 / maxlife;
		currentlife = maxlife;
		return true;
	}

	bool save(std::ofstream& os, MKeyValue& atts) {
		atts.put("points", currentlife);
		return true;
	}

	float getCurrent() {
		return currentlife;
	}

	float getCurrentNormalized() {
		return (currentlife * maxlife_inv);
	}

	float getMax() {
		return maxlife;
	}

	void setCurrent(float new_life) {
		if (new_life > maxlife) {
			maxlife = new_life;
			maxlife_inv = 1 / maxlife;
		}
		currentlife = new_life;
	}

	void onCreate(const TMsgEntityCreated &) {
		dead = false;
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

	void update(float elapsed);

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
		maxlife_inv = 1 / max;
	}

	void renderInMenu() {
		ImGui::SliderFloat("life", &currentlife, 0, 1);
		ImGui::SliderFloat("Action Damage Scale", &energyDamageScale, 0, 1);
	}

	static bool isDead() { return dead; }
};

#endif
