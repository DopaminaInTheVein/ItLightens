#ifndef INC_PLAYER_CONTROLLER_MOLE_H_
#define INC_PLAYER_CONTROLLER_MOLE_H_

#include "poss_controller.h"

#include "player_controller_base.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "components/comp_msgs.h"
#include "logic/ai_mole.h"

#include "camera/camera.h"

class CEntity;

template< class TObj >
class CObjectManager;

class player_controller_mole : public PossController {
	CObjectManager<player_controller_mole> *om;
	float player_max_speed = CPlayerBase::player_max_speed;
public:
	void Init();

	void GrabBox();
	void LeaveBox();
	void DestroyWall();
	void Moving();

	void InitControlState();
	bool nearToBox();
	bool nearToWall();
	bool boxGrabbed = false;
	string selectedBox = "";
	int selectedBoxi = 0;
	int selectedWallToBreaki = 0;

	void UpdateInputActions();

	TCompTransform * getEntityTransform() {
		CEntity * ent = myParent;
		return ent->get<TCompTransform>();
	}
	CEntity* player_controller_mole::getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}
	CEntity * getEntityBoxPointer(int i) {
		CHandle han = SBB::readHandlesVector("wptsBoxes")[i];
		CEntity * ent = han;
		return ent;
	}
	CHandle getEntityWallHandle(int i) {
		return SBB::readHandlesVector("wptsBreakableWall")[i];
	}

	//Overload function for handler_manager
	player_controller_mole& player_controller_mole::operator=(player_controller_mole arg) { return arg; }
};

#endif