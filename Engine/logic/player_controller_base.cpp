#include "mcv_platform.h"
#include "player_controller_base.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"

#include "components\comp_msgs.h"

CPlayerBase::CPlayerBase() {
	AddState("idle", (statehandler)&CPlayerBase::Idle);
	AddState("moving", (statehandler)&CPlayerBase::Moving);
	AddState("falling", (statehandler)&CPlayerBase::Falling);
	AddState("jumping", (statehandler)&CPlayerBase::Jumping);
	AddState("die", (statehandler)&CPlayerBase::Die);
}

void CPlayerBase::onSetCamera(const TMsgSetCamera& msg) {
	camera = msg.camera;
}

void CPlayerBase::update(float elapsed) {
	Input.Frame();
	UpdateInputActions();
	Recalc();
	UpdateMoves();
}

//##########################################################################
//Movement
//##########################################################################
#pragma region Movement

void CPlayerBase::UpdateMoves()
{
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	VEC3 direction = directionForward + directionLateral;

	float yaw, pitch;
	player_transform->getAngles(&yaw, &pitch);

	float new_x, new_z;

	new_x = direction.x * cosf(yaw) + direction.z*sinf(yaw);
	new_z = -direction.x * sinf(yaw) + direction.z*cosf(yaw);

	direction.x = new_x;
	direction.z = new_z;

	direction.Normalize();

	if (onGround) {
		//Set current velocity with friction
		float drag = 0.001f;
		float drag_i = (1 - drag);

		if (moving) player_curr_speed = drag_i*player_curr_speed + drag*player_max_speed;
		else player_curr_speed = drag_i*player_curr_speed - drag*player_max_speed;

		if (player_curr_speed < 0) {
			player_curr_speed = 0.0f;
			directionForward = directionLateral = VEC3(0, 0, 0);
		}

		//set final position
		player_position = player_position + direction*getDeltaTime()*player_curr_speed;
		player_transform->setPosition(player_position);
	}
	else {
		player_position = player_position + direction*getDeltaTime()*(player_curr_speed / 2);

		player_transform->setPosition(player_position);
		player_position = player_transform->getPosition();
		player_position = player_position + directionJump*getDeltaTime()*jspeed;
		player_transform->setPosition(player_position);
	}

	if (rotate != 0) {
		player_transform->setAngles(yaw + rotate*player_rotation_speed*getDeltaTime(), pitch);
	}
}
#pragma endregion
//##########################################################################

//##########################################################################
//Inputs
//##########################################################################
#pragma region Inputs

bool CPlayerBase::UpdateMovDirection() {
	moving = false;
	bool moving_h = false;
	bool moving_v = false;

	if (Input.IsUpPressed() && !Input.IsDownPressed()) {
		directionForward = VEC3(0, 0, 1);
		moving = true;
		moving_v = true;
	}
	else if (Input.IsDownPressed() && !Input.IsUpPressed()) {
		directionForward = VEC3(0, 0, -1);
		moving = true;
		moving_v = true;
	}

	if (Input.IsLeftPressed() && !Input.IsRightPressed()) {
		directionLateral = VEC3(1, 0, 0);
		moving = true;
		moving_h = true;
	}
	else if (Input.IsRightPressed() && !Input.IsLeftPressed()) {
		directionLateral = VEC3(-1, 0, 0);
		moving = true;
		moving_h = true;
	}

	//TODO: depends on movemtent type, maybe wont be needed
	if (moving_h && !moving_v) {	//moving only one direction
		directionForward = VEC3(0, 0, 0);
	}
	else if (!moving_h && moving_v) {	//moving only one direction
		directionLateral = VEC3(0, 0, 0);
	}

	return moving;
}

void CPlayerBase::UpdateJumpState() {
	if (Input.IsKeyPressedDown(DIK_SPACE)) {
		Jump();
	}
}

void CPlayerBase::UpdateDirection() {
	if (Input.IsOrientLeftPressed())
		rotate = 1;
	else if (Input.IsOrientRightPressed())
		rotate = -1;
	else
		rotate = 0;
}

void CPlayerBase::UpdateInputActions() {
	//TODO: actions
}

#pragma endregion

//##########################################################################

//##########################################################################
// Player States
//##########################################################################
#pragma region Player States

void CPlayerBase::Idle()
{
	UpdateDirection();
	UpdateJumpState();
	if (UpdateMovDirection()) ChangeState("moving");
}

void CPlayerBase::Jump()
{
	jspeed = jimpulse;
	directionJump = VEC3(0, 1, 0);
	onGround = false;
	ChangeState("jumping");
}

void CPlayerBase::Die()
{
	dbg("die!\n");
	ChangeState("idle");
}

void CPlayerBase::Falling()
{
	UpdateDirection();
	UpdateMovDirection();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	jspeed -= gravity*getDeltaTime();

	if (player_position.y <= 0) {
		onGround = true;
		jspeed = 0.0f;
		directionJump = VEC3(0, 0, 0);
		ChangeState("idle");
	}
}

void CPlayerBase::Jumping()
{
	UpdateDirection();
	UpdateMovDirection();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	//dbg("deltatime: %f\n", getDeltaTime());
	jspeed -= gravity*getDeltaTime();
	//dbg("jspeed: %f\n", jspeed);

	if (jspeed <= 0.1f) {
		jspeed = 0.0f;
		ChangeState("falling");
	}
}

void CPlayerBase::Moving()
{
	UpdateDirection();
	UpdateJumpState();
	if (!UpdateMovDirection()) ChangeState("idle");
}

#pragma endregion

//##########################################################################

// Sets the entity
void CPlayerBase::SetMyEntity() {
	myEntity = myParent;
}

void CPlayerBase::renderInMenu()
{
	VEC3 direction = directionForward + directionLateral;
	direction.Normalize();
	direction = direction + directionJump;

	ImGui::Text("NODE: %s\n", state.c_str());
	ImGui::Text("direction: %.4f, %.4f, %.4f", direction.x, direction.y, direction.z);
	ImGui::Text("jump: %.5f", jspeed);
}