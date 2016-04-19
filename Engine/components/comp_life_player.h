#ifndef INC_COMPONENT_LIFE_PLAYER_H_
#define INC_COMPONENT_LIFE_PLAYER_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "comp_life.h"
#include "entity_tags.h"
#include "entity.h"

// ------------------------------------
struct TCompLifePlayer : public TCompLife {
	void onCreate(const TMsgEntityCreated &) {
		CHandle me = CHandle(this).getOwner();
		if (!me.isValid()) {
			fatal("Error creating life component\n");
			assert(false);
		}
		energyDamageScale = 0.1f;
		last_modif = 0.1f;
	}

	//void onDamage(const TMsgDamage& msg) {
	//	energyDamageScale = msg.modif;
	//}

	//void onReciveDamage(const TMsgSetDamage& msg) {
	//	currentlife -= msg.dmg;
	//}

	//void onStopDamage(const TMsgStopDamage& dmg) {
	//	energyDamageScale = last_modif;
	//}

	//void onSetSaveDamage(const TMsgDamageSave& msg) {
	//	last_modif = msg.modif;
	//	energyDamageScale = last_modif;
	//}

	//void setMaxLife(float max) {
	//	maxlife = max;
	//	currentlife = max;
	//}

	//void renderInMenu() {
	//	ImGui::SliderFloat("life", &currentlife, 0, 1);
	//	ImGui::SliderFloat("Action Damage Scale", &energyDamageScale, 0, 1);
	//}
};

#endif
