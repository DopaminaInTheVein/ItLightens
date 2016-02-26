#ifndef INC_PLAYER_MOLE_CONTROLLER_H_
#define INC_PLAYER_MOLE_CONTROLLER_H_

#include "aicontroller.h"
#include "player_controller.h"
#include "sbb.h"

#include "components\comp_base.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"
#include "logic/ai_mole.h"

#include "camera\camera.h"
#include "input\input.h"

class CEntity;
class CInput;

extern CInput Input;

template< class TObj >
class CObjectManager;

class player_mole_controller : public player_controller {
	bool boxGrabbed = false;
	int selectedBox = -1;

	TCompTransform * getEntityTransform() {
		CEntity * ent = myParent;
		return ent->get<TCompTransform>();
	}

	CEntity * getEntityPointer(int i) {
		CHandle han = SBB::readHandlesVector("wptsBoxes")[i];
		CEntity * ent = han;
		return ent;
	}
public:

	player_mole_controller() {}
	void Init() override;
	void init() { Init(); }

	void GrabBox();
	void LeaveBox();

	string ParseInput();

	//Overload function for handler_manager
	player_mole_controller& player_mole_controller::operator=(player_mole_controller arg) { return arg; }
};

#endif