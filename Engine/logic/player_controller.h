#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

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

class player_controller : public aicontroller, public TCompBase {

	// Map for debug on ImGui
	std::map<int, std::string> out;

	// Controller input class
	CHandle camera;

	float dt;
	const float player_speed = 18.f;
	const float player_rotation_speed = 10.f;

	CObjectManager<player_controller> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CEntity *myEntity = nullptr;

public:

	player_controller() {}
	void Init() override;
	void init() { Init(); }
	void update(float elapsed);

	void onSetCamera(const TMsgSetCamera& msg);

	void SetMyEntity();
	VEC3 GetPlayerPosition();
	VEC3 GetPlayerFront();
	void SetPlayerPosition(VEC3 new_position);
	void GetPlayerAngles(float &yaw, float &pitch);
	void SetPlayerAngles(float new_yaw, float new_pitch);
	void OrbitCamera(float angle);
	string ParseInput();

	// Player states
	void Idle();
	void MoveLeft();
	void MoveRight();		
	void MoveUp();
	void MoveDown();	
	void Action();
	void Jump();
	void Stun();
	void Possess();
	void OrientLeft();
	void OrientRight();

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif