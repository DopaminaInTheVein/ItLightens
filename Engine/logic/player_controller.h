#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

#include "aicontroller.h"
#include "sbb.h"

#include "handle/handle.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/comp_msgs.h"
#include "components/entity.h"

#include "camera/camera.h"
#include "input/input.h"

#include "player_controller_base.h"

class player_controller : public CPlayerBase {
	CObjectManager<player_controller> *om;
	int topolarizedplus = -1;
	int topolarizedminus = -1;
	bool polarizedMove = false;
	bool tominus = false;
	bool toplus = false;

	float polarizedCurrentSpeed = 0.0f;

	//Possession
	CHandle currentPossessable;
	float possessionReach = 20;

	//Possession debug
	____TIMER_DECLARE_(timeShowAblePossess, 1);

	TCompTransform * getEntityTransform() {
		SetMyEntity();
		CEntity * ent = myParent;
		return ent->get<TCompTransform>();
	}

	CHandle getMinusPointHandle(int i) {
		return SBB::readHandlesVector("wptsMinusPoint")[i];
	}
	CHandle getPlusPointHandle(int i) {
		return SBB::readHandlesVector("wptsPlusPoint")[i];
	}

	void recalcPossassable();
	void UpdatePossession();

public:
	void Init();
	void Idle();

	void DoubleJump();
	void DoubleFalling();

	void Jumping();
	void Falling();

	void AttractToMinus();
	void AttractToPlus();
	void AttractMove(CEntity * ent);
	bool nearMinus();
	bool nearPlus();

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif