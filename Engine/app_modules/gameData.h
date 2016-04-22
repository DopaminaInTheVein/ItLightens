#ifndef INC_GAME_GAME_DATA_H_
#define INC_GAME_GAME_DATA_H_

#include "components/entity.h"
#include "components/entity_tags.h"
namespace GAMEDATA {
	namespace PLAYER {
		float getLife() {
			CHandle player = tags_manager.getFirstHavingTag(getID("player"));
			if (player.isValid()) {
				CEntity* ePlayer = player;
				TCompLife * life = ePlayer->get<TCompLife>();
				assert(life || fatal("Player doesn't have life component!"));
				return life->getCurrentNormalized();
			}
		}
	}
}

#endif