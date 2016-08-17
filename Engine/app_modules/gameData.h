#ifndef INC_GAME_GAME_DATA_H_
#define INC_GAME_GAME_DATA_H_

#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/components.h"
class GameData {
public:
	static float getLifeNormalized() {
		CHandle player = tags_manager.getFirstHavingTag(getID("raijin"));
		if (player.isValid()) {
			CEntity* ePlayer = player;
			TCompLife * life = ePlayer->get<TCompLife>();
			assert(life || fatal("Player doesn't have life component!"));
			return life->getCurrent() / getLifeMax();
		}
	}

	static float getLifeMax() {
		CHandle player = tags_manager.getFirstHavingTag(getID("raijin"));
		if (player.isValid()) {
			CEntity* ePlayer = player;
			player_controller * pc = ePlayer->get<player_controller>();
			//assert(pc || fatal("Player doesn't have player_controller component!"));
			if (pc)	return pc->getMaxLife();
			else return 100.f;
		}
	}

	static float getLifeEvolution() {
		return 50.0f;
	}
};
#endif