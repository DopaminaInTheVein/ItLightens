#ifndef INC_COMPONENT_LIFE_H_
#define INC_COMPONENT_LIFE_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "entity_tags.h"
#include "utils/XMLParser.h"

#define DMG_SCALE_ACTION_INI	1.0f
#define DMG_SCALE_ENEMY_INI		0.1f

// ------------------------------------
struct TCompLife : public TCompBase {
	float currentlife;
	float maxlife;

	float DMG_SCALE_ACTION;
	float DMG_SCALE_ENEMY;

	float energyDamageScale = 0.1f;
	TCompLife() : maxlife(100.f) {
		dbg("constructor of TCompLife\n");
		DMG_SCALE_ACTION = DMG_SCALE_ACTION_INI;
		DMG_SCALE_ENEMY = DMG_SCALE_ENEMY_INI;
	}
	~TCompLife() {
		dbg("destructor of TCompLife\n");
	}

	bool load(MKeyValue& atts) {
		currentlife = maxlife = atts.getFloat("points", 100.0f);
		return true;
	}

	void onCreate(const TMsgEntityCreated&) {
		dbg("TCompLife on TMsgEntityCreated\n");
	}
	void onDamage(const TMsgDamage& msg) {
		CEntity * victoryPoint = tags_manager.getFirstHavingTag(getID("victory_point"));
		CHandle playerhandle = CHandle(this).getOwner();
		CEntity * target_e = playerhandle;
		TCompTransform * player_transform = target_e->get<TCompTransform>();
		TCompTransform * victoryPoint_transform = victoryPoint->get<TCompTransform>();

		if (0.5f <= simpleDist(victoryPoint_transform->getPosition(), player_transform->getPosition())) {
			float dmgTotal;
			switch (msg.dmgType) {
			case ENERGY_DECREASE:
				dmgTotal = msg.points * DMG_SCALE_ACTION;
				break;
			case LASER:
				dmgTotal = msg.points * DMG_SCALE_ENEMY;
				break;
			case WATER:
				dmgTotal = msg.points * DMG_SCALE_ENEMY;
				break;
			default:
				dmgTotal = 0;
				break;
			}
			currentlife -= dmgTotal;
			if (currentlife > maxlife) {
				currentlife = maxlife;
			}
		}
	}

	void setMaxLife(float max) { 
		maxlife = max;
		currentlife = max; 
	}

	void renderInMenu() {
		ImGui::SliderFloat("Action Damage Scale", &DMG_SCALE_ACTION, 0, 1);
		ImGui::SliderFloat("Enemy Damage Scale", &DMG_SCALE_ENEMY, 0, 1);
	}
};

#endif
