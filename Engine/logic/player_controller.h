#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

#include "aicontroller.h"
#include "sbb.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"

#include "camera\camera.h"
#include "input\input.h"

#include "player_controller_base.h"

class player_controller : public CPlayerBase {
	CObjectManager<player_controller> *om;

public:
	void Init();

	void DoubleJump();
	void DoubleFalling();

	void Jumping();
	void Falling();

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif