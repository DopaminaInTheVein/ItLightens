#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"

#include "components\entity_tags.h"

void player_controller::Init() {
	om = getHandleManager<player_controller>();	//player

	AddState("idle", (statehandler)&player_controller::Idle);
	AddState("moveleft", (statehandler)&player_controller::MoveLeft);
	AddState("moveright", (statehandler)&player_controller::MoveRight);
	AddState("moveup", (statehandler)&player_controller::MoveUp);
	AddState("movedown", (statehandler)&player_controller::MoveDown);
	AddState("orientleft", (statehandler)&player_controller::OrientLeft);
	AddState("orientright", (statehandler)&player_controller::OrientRight);
	AddState("action", (statehandler)&player_controller::Action);
	AddState("jump", (statehandler)&player_controller::Jump);
	AddState("stun", (statehandler)&player_controller::Stun);
	AddState("possess", (statehandler)&player_controller::Possess);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
}

void player_controller::onSetCamera(const TMsgSetCamera& msg) {
	camera = msg.camera;
}

void player_controller::update(float elapsed) {
	Input.Frame();
	Recalc();
}

// Player States

void player_controller::Idle()
{
	string next_state = ParseInput();
	ChangeState(next_state);
}

void player_controller::MoveLeft()
{
	VEC3 player_position = GetPlayerPosition();
	VEC3 player_front = GetPlayerFront();

	player_position.x += player_front.z * dt * player_speed;
	player_position.z -= player_front.x * dt * player_speed;

	SetPlayerPosition(player_position);

	ChangeState("idle");
}

void player_controller::MoveRight()
{
	VEC3 player_position = GetPlayerPosition();
	VEC3 player_front = GetPlayerFront();

	player_position.x -= player_front.z * dt * player_speed;
	player_position.z += player_front.x * dt * player_speed;

	SetPlayerPosition(player_position);

	ChangeState("idle");
}

void player_controller::MoveUp()
{
	VEC3 player_position = GetPlayerPosition();
	VEC3 player_front = GetPlayerFront();

	player_position.x += player_front.x * dt * player_speed;
	player_position.z += player_front.z * dt * player_speed;

	SetPlayerPosition(player_position);

	ChangeState("idle");
}

void player_controller::MoveDown()
{
	VEC3 player_position = GetPlayerPosition();
	VEC3 player_front = GetPlayerFront();

	player_position.x -= player_front.x * dt * player_speed;
	player_position.z -= player_front.z * dt * player_speed;

	SetPlayerPosition(player_position);

	ChangeState("idle");
}

void player_controller::Action()
{
	dbg("ACTION!\n");
	ChangeState("idle");
}

void player_controller::Jump()
{
	dbg("JUMP!\n");
	ChangeState("idle");
}

void player_controller::Stun()
{
	dbg("STUN!\n");
	ChangeState("idle");
}

void player_controller::Possess()
{
	dbg("POSSESS!\n");
	ChangeState("idle");
}

void player_controller::OrientLeft()
{
	float yaw, pitch;
	GetPlayerAngles(yaw, pitch);
	SetPlayerAngles(yaw + dt * player_rotation_speed, pitch);

	OrbitCamera(dt);
	ChangeState("idle");
}

void player_controller::OrientRight()
{
	float yaw, pitch;
	GetPlayerAngles(yaw, pitch);
	SetPlayerAngles(yaw - dt * player_rotation_speed, pitch);

	OrbitCamera(-dt);
	ChangeState("idle");
}

// Sets the entity
void player_controller::SetMyEntity() {
	myEntity = myParent;
}

// Returns the position of the player
VEC3 player_controller::GetPlayerPosition() {
	SetMyEntity(); 
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	return player_position;
}

// Returns the front of the player
VEC3 player_controller::GetPlayerFront() {
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_front = player_transform->getFront();
	return player_front;
}

// Changes the position of the player
void player_controller::SetPlayerPosition(VEC3 new_position) {
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	player_transform->setPosition(new_position);
}

// Returns the front of the player
void  player_controller::GetPlayerAngles(float &yaw, float &pitch) {
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	player_transform->getAngles(&yaw, &pitch);
}

// Changes the angles of the player
void player_controller::SetPlayerAngles(float new_yaw, float new_pitch) {
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	player_transform->setAngles(new_yaw, new_pitch);
}

// Orbits the camera with the given angle
void player_controller::OrbitCamera(float angle) {
	SetMyEntity();

	float s = sin(angle);
	float c = cos(angle);

	CEntity * camera_e = camera;
	CEntity * player_e = myEntity;

	// translate point back to origin:
	TCompTransform* camera_transform = camera_e->get<TCompTransform>();
	TCompTransform* target_transform = player_e->get<TCompTransform>();

	VEC3 entPos = camera_transform->getPosition();
	entPos.x -= target_transform->getPosition().x;
	entPos.z -= target_transform->getPosition().z;

	// rotate point
	float xnew = entPos.x * c - entPos.z * s;
	float ynew = entPos.x * s + entPos.z * c;

	// translate point back:
	entPos.x = xnew + target_transform->getPosition().x;
	entPos.z = ynew + target_transform->getPosition().z;

	camera_transform->setPosition(entPos);

	angle = camera_transform->getDeltaYawToAimTo(target_transform->getPosition());
	float yaw = 0.0f, pitch = 0.0f;
	camera_transform->getAngles(&yaw, &pitch);
	camera_transform->setAngles(yaw + angle, 0.0f);
}

// Decides which state to go next
string player_controller::ParseInput() {

	dt = getDeltaTime();

	if (!ImGui::GetIO().WantCaptureKeyboard) {
		if (Input.IsLeftPressed()) {
			return "moveleft";
		}
		if (Input.IsRightPressed()) {
			return "moveright";
		}
		if (Input.IsUpPressed()) {
			return "moveup";
		}
		if (Input.IsDownPressed()) {
			return "movedown";
		}
		if (Input.IsSpacePressed()) {
			return "jump";
		}
		if (Input.IsLeftClickPressed()) {
			return "action";
		}
		if (Input.IsRightClickPressed()) {
			return "possess";
		}
		if (Input.IsOrientLeftPressed())
		{
			return "orientleft";
		}
		if (Input.IsOrientRightPressed())
		{
			return "orientright";
		}
	}
	else {
		Input.Unacquire();
	}

	return "idle";
}

