#ifndef INC_PLAYER_CONTROLLER_SPEEDY_H_
#define INC_PLAYER_CONTROLLER_SPEEDY_H_

#include "poss_controller.h"
#include "logic\ai_water.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/static_mesh.h"

#include "camera\camera.h"

#include "player_controller_base.h"

#include "utils/XMLParser.h"

extern TTagsManager tags_manager;

//Cambio malla
struct TCompRenderStaticMesh;

class player_controller_speedy : public PossController {
	CObjectManager<player_controller_speedy> *om;

	const float dash_speed = 30.f;
	const float dash_max_duration = 1.f;
	float dash_duration = 0.f;

	float dash_timer = 0.f;
	bool dash_ready = true;
	bool dashing = false;
	const float dash_cooldown = 8.f; //in seconds

	const float max_blink_duration = 2.f; //in seconds
	float blink_duration = max_blink_duration; //in seconds
	const float blink_distance = 8.f;

	float drop_water_timer;
	const float drop_water_timer_reset = 3.f;

	float blink_timer = 0.f;
	bool blink_ready = true;
	bool drop_water_ready = false;
	const float blink_cooldown = 5.f;

	const string water_static_mesh = "static_meshes/water.static_mesh";

	TCompRenderStaticMesh* mesh;
	string pose_idle_route;
	string pose_run_route;
	string pose_jump_route;
	string pose_void_route;

public:
	void Init();
	void myUpdate() override;

	void UpdateInputActions();

	// Speedy specific state
	void DoubleJump();
	void Jumping();
	void DoubleFalling();
	void Falling();
	void Dashing();
	void Blinking();
	void Blink();
	// Speedy auxiliar functions
	bool dashFront();
	bool collisionWall();
	bool collisionBlink(float& distCollision);
	bool rayCastToFront(int types, float reach, float& distRay);
	// Timer control functions
	void resetDashTimer();
	void updateDashTimer();
	void resetBlinkTimer();
	void updateBlinkTimer();
	void updateDropWaterTimer();
	void resetDropWaterTimer();

	void UpdateUnpossess() override;

	void DisabledState();
	void InitControlState();
	CEntity* getMyEntity();

	//Cambio Malla
	//void ChangePose(CHandle new_pos_h);
	void ChangePose(string new_pose_route);

	void SetCharacterController();

	//Overload function for handler_manager
	player_controller_speedy& player_controller_speedy::operator=(player_controller_speedy arg) { return arg; }
};

#endif