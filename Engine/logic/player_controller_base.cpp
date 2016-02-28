#include "mcv_platform.h"
#include "player_controller_base.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\comp_life.h"
#include "components\entity.h"
#include "components\entity_tags.h"

#include "components\comp_msgs.h"
#include "components\comp_camera.h"

CPlayerBase::CPlayerBase() {
	AddState("idle", (statehandler)&CPlayerBase::Idle);
	AddState("moving", (statehandler)&CPlayerBase::Moving);
	AddState("falling", (statehandler)&CPlayerBase::Falling);
	AddState("jumping", (statehandler)&CPlayerBase::Jumping);
	AddState("die", (statehandler)&CPlayerBase::Die);
}

bool CPlayerBase::checkDead() {
	SetMyEntity();
	TCompLife * player_life = myEntity->get<TCompLife>();
	if (player_life->currentlife <= 0) {
		ChangeState("die");
		return true;
	}
	return false;
}
void CPlayerBase::onSetCamera(const TMsgSetCamera& msg) {
	camera = msg.camera;
}

void CPlayerBase::update(float elapsed) {
	if (controlEnabled) {
		Input.Frame();
		UpdateInputActions();
		Recalc();
		UpdateMoves();
		myUpdate();
	}
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

	VEC3 direction = directionForward;

	CEntity * camera_e = camera;
	TCompCamera* camera_comp = camera_e->get<TCompCamera>();
	VEC3 camera_position = camera_comp->getPosition();

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
		float drag = 0.002f;
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

	if (rotateXY != 0) {
		player_y += rotateXY*player_rotation_speed*getDeltaTime();

		if (player_y > camera_max_height)
			player_y = camera_max_height;
		if (player_y < camera_min_height)
			player_y = camera_min_height;

		camera_comp->lookAt(camera_position, VEC3(player_position.x, player_y, player_position.z));
		camera_comp->update(getDeltaTime());
	}
	else {
		player_y = starting_player_y;
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

	if (Input.IsUpPressed() && !Input.IsDownPressed()) {
		directionForward = VEC3(0, 0, 1);
		moving = true;
	}
	else if (Input.IsDownPressed() && !Input.IsUpPressed()) {
		directionForward = VEC3(0, 0, -1);
		moving = true;
	}
	return moving;
}

void CPlayerBase::UpdateJumpState() {
	if (Input.IsSpacePressedDown()) {
		Jump();
	}
}

void CPlayerBase::UpdateDirection() {
	if (Input.IsOrientLeftPressed())
		rotate = 1;
	else if (Input.IsOrientRightPressed())
		rotate = -1;
	else {
		rotate = 0;
	}
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
void CPlayerBase::energyDecreasal(float howmuch) {
	TMsgDamage msg;
	msg.points = howmuch;
	this->myEntity->sendMsg(msg);
}

void CPlayerBase::Idle()
{
	if (!checkDead()) {
		energyDecreasal(getDeltaTime()*0.05f);
		UpdateDirection();
		UpdateJumpState();
		if (UpdateMovDirection()) ChangeState("moving");
	}
}

void CPlayerBase::Jump()
{
	if (onGround) {
		energyDecreasal(1.0f);
	}
	jspeed = jimpulse;
	directionJump = VEC3(0, 1, 0);
	onGround = false;
	ChangeState("jumping");
}

void CPlayerBase::Die()
{
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	if (player_position.y > 0.0f) {
		Falling();
	}
	else {
		onGround = true;
		jspeed = 0.0f;
		directionJump = VEC3(0, 0, 0);
	}
	orbitCameraDeath();
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
	energyDecreasal(getDeltaTime()*0.05f);
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

void CPlayerBase::orbitCameraDeath() {
	float angle = getDeltaTime();
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	TCompTransform* target_transform = myEntity->get<TCompTransform>();

	CEntity * camera_e = camera;
	//TCompCamera* camera_transform = camera_e->get<TCompCamera>();
	TCompTransform* player_transform = camera_e->get<TCompTransform>();
	TCompCamera* player_cam = camera_e->get<TCompCamera>();

	VEC3 entPos = player_transform->getPosition();
	entPos.x -= target_transform->getPosition().x;
	entPos.z -= target_transform->getPosition().z;

	// rotate point
	float xnew = entPos.x * c - entPos.z * s;
	float ynew = entPos.x * s + entPos.z * c;

	// translate point back:
	entPos.x = xnew + target_transform->getPosition().x;
	entPos.z = ynew + target_transform->getPosition().z;

	player_transform->setPosition(entPos);

	player_cam->update(getDeltaTime());
}

void CPlayerBase::myUpdate() {
}