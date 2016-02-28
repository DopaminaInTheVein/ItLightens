#ifndef INC_PLAYER_CONTROLLER_SPEEDY_H_
#define INC_PLAYER_CONTROLLER_SPEEDY_H_

#include "poss_controller.h"
#include "sbb.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"

#include "camera\camera.h"
#include "input\input.h"

#include "player_controller_base.h"

class player_controller_speedy : public PossController {
	CObjectManager<player_controller_speedy> *om;

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

public:
	void Init();
	void update(float elapsed);

	void UpdateInputActions();

	// Speedy specific state
	void Dashing();
	void Blinking();
	void Blink();
	// Speedy auxiliar functions
	bool dashFront();
	void resetDashTimer();
	void updateDashTimer();
	void resetBlinkTimer();
	void updateBlinkTimer();

	void DisabledState();
	void InitControlState();
	CEntity* getMyEntity();

	//Overload function for handler_manager
	player_controller_speedy& player_controller_speedy::operator=(player_controller_speedy arg) { return arg; }
};

#endif