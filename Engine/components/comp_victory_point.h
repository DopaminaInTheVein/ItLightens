#ifndef	INC_COMP_VICTORY_POINT
#define INC_COMP_VICTORY_POINT

#include "comp_trigger.h"

struct TVictoryPoint : public TTrigger {


	void mUpdate(float dt) {} //not needed
	void onTriggerInside(const TMsgTriggerIn& msg) {}	//not needed
	void onTriggerEnter(const TMsgTriggerIn& msg) {
		GameController->SetGameState(CGameController::VICTORY);
	}
	void onTriggerExit(const TMsgTriggerOut& msg) {}		//not needed
};

#endif