#ifndef INC_GAME_CONTROLLER_H_
#define INC_GAME_CONTROLLER_H_

#include "app_module.h"

class CGameController : public IAppModule {

	int game_state = 0;

public:
	enum{
		STARTING = 0,
		RUNNING,
		STOPPED,
		VICTORY,
		LOSE,
		MENU,
	};

	int GetGameState() const { return game_state; }
	void SetGameState(int state) { game_state = state; }

	const char* getName() const {
		return "game_controller";
	}


};

extern CGameController* GameController;

#endif