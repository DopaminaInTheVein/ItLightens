#ifndef INC_PC_BASE_H_
#define	INC_PC_BASE_H_

#include "aicontroller.h"
#include "sbb.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"

#include "camera\camera.h"
#include "input\input.h"

class CEntity;
class CInput;

extern CInput Input;

template< class TObj >
class CObjectManager;

class CPlayerBase : public aicontroller, public TCompBase {
protected:

	//Enabled
	bool controlEnabled = false;

	// Map for debug on ImGui
	std::map<int, std::string> out;

	// Controller input class
	CHandle camera;

	float dt;
	const float player_max_speed = 5.f;
	const float gravity = 10.0f;
	float player_curr_speed = 0.0f;
	float jspeed = 0.0f;
	const float player_rotation_speed = 5.f;
	const float jimpulse = 5.0f;
	bool onGround = true;
	bool moving = false;

	//CObjectManager<CPlayerBase> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CEntity *myEntity = nullptr;

	VEC3 directionLateral = VEC3(0, 0, 0);
	VEC3 directionForward = VEC3(0, 0, 1);
	VEC3 directionJump = VEC3(0, 0, 0);
	float rotate = 0;
	const float camera_max_height = 4;
	const float camera_min_height = 0;
	float rotateXY = 0;
	float starting_player_y = 0;
	float player_y = 0;

	virtual void UpdateInputActions();
	void UpdateMoves();
	bool UpdateMovDirection();
	void UpdateJumpState();
	void UpdateDirection();

public:

	CPlayerBase();
	void init() { Init(); }
	void update(float elapsed); //deberia ser const pero Recalc no lo es  e historias

	void onSetCamera(const TMsgSetCamera& msg);
	void SetMyEntity();
	virtual void myUpdate(); // deberia ser abstracta pero peta

	// Player states
	void Idle();
	void Jump();
	void Die();

	void Jumping();
	void Falling();
	void Moving();

	void renderInMenu();

	//Overload function for handler_manager
	CPlayerBase& CPlayerBase::operator=(CPlayerBase arg) { return arg; }
};

#endif