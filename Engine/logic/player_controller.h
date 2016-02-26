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

	/**TO REMOVE**/
	bool in_speedy = false;

	const float dash_speed = 0.075f;
	const float dash_max_duration = 1.f;
	float dash_duration = 0.f;

	float dash_timer = 0.f;
	bool dash_ready = true;
	const float dash_cooldown = 5.f;

	const float blink_distance = 8.f;

	float blink_timer = 0.f;
	bool blink_ready = true;
	const float blink_cooldown = 5.f;
	/**TO REMOVE**/

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
	/**TO REMOVE**/
	bool dashFront();
	void resetDashTimer();
	void updateDashTimer();
	void resetBlinkTimer();
	void updateBlinkTimer();
	/**TO REMOVE**/

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
	/**TO REMOVE**/
	void Dashing();
	void Blinking();
	void Blink();
	/**TO REMOVE**/

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif