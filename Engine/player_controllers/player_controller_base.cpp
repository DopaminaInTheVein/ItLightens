#include "mcv_platform.h"
#include "player_controller_base.h"

#include <windows.h>
#include "handle/object_manager.h"

#include "windows/app.h"

#include "components/comp_transform.h"
#include "components/comp_life.h"
#include "components/entity.h"
#include "components/entity_tags.h"

#include "components/comp_msgs.h"
#include "components/comp_camera_main.h"

#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "utils/utils.h"

#include "components/comp_charactercontroller.h"

map<int, string> CPlayerBase::out = {};

CPlayerBase::CPlayerBase() {
}

void CPlayerBase::addBasicStates() {
	AddState("idle", (statehandler)&CPlayerBase::Idle);
	AddState("moving", (statehandler)&CPlayerBase::Moving);
	AddState("falling", (statehandler)&CPlayerBase::Falling);
	AddState("jumping", (statehandler)&CPlayerBase::Jumping);
	AddState("die", (statehandler)&CPlayerBase::Die);
	AddState("win", (statehandler)&CPlayerBase::Win);
}

bool CPlayerBase::getUpdateInfo() {
	myEntity = compBaseEntity;

	cc = compBaseEntity->get<TCompCharacterController>();
	if (!cc) return false;

	transform = compBaseEntity->get<TCompTransform>();
	if (!transform) return false;

	return true;
}

bool CPlayerBase::checkDead() {
	PROFILE_FUNCTION("checkdead");

	if (GameController->GetGameState() == CGameController::LOSE) {
		ChangeState("die");
		ChangeCommonState("die");
		return true;
	}

	if (GameController->GetGameState() == CGameController::VICTORY) {
		ChangeState("win");
		ChangeCommonState("win");
		return true;
	}
	return false;
}

void CPlayerBase::onSetCamera(const TMsgSetCamera& msg) {
	camera = msg.camera;
}

void CPlayerBase::onSetControllable(const TMsgSetControllable& msg) {
	controlEnabled = msg.control;
}

void CPlayerBase::onGoAndLook(const TMsgGoAndLook& msg) {
	if (msg.target.isValid()) {
		CEntity* e = msg.target;
		//TCompTransform * transform = e->get<TCompTransform>();
		GET_COMP(transform, msg.target, TCompTransform);
		if (transform) {
			float pitch;
			transform->getAngles(&cinematicTargetYaw, &pitch);
			cinematicTargetPos = transform->getPosition();
			cinematicEndCode = msg.code_arrived;
			onCinematic = true;
		}
	}
}

void CPlayerBase::stopMovement() {
	moving = false;
	directionForward = directionLateral = VEC3(0.f, 0.f, 0.f);
}

void CPlayerBase::update(float elapsed) {
	PROFILE_FUNCTION("update base");
	if (camera.isValid()) {
		if (onCinematic) {
			UpdateCinematic(elapsed);
		}
		else if (controlEnabled) {
			bool alive = !checkDead();
			if (alive && inputEnabled) {
				UpdateMoves();
				UpdateInputActions();
			}
			Recalc();
			if (alive) {
				//UpdateMoves();
				myUpdate();
				update_msgs();
			}
		}
		//UpdateAnimation();
	}
}

void CPlayerBase::UpdateCinematic(float elapsed) {
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	float yaw, pitch;
	transform->getAngles(&yaw, &pitch);
	float dist = simpleDistXZ(cc->GetPosition(), cinematicTargetPos);
	if (dist < epsilonPos) {
		// Reach position
		float deltaYaw = cinematicTargetYaw - yaw;
		if (abs(deltaYaw) < epsilonYaw) {
			//In position and Oriented
			onCinematic = false;
			logic_manager->throwUserEvent(cinematicEndCode);
		}
		else {
			//Orientation to target
			transform->setAngles(yaw * 0.9f + 0.1f * cinematicTargetYaw, pitch);
		}
	}
	else {
		// Go to target
		float deltaYaw = transform->getDeltaYawToAimTo(cinematicTargetPos);
		if (deltaYaw > epsilonYaw) transform->setAngles(yaw + 0.1f * deltaYaw, pitch);
		VEC3 dir = cinematicTargetPos - cc->GetPosition();
		dir.Normalize();
		cc->AddMovement(dir, player_max_speed * getDeltaTime());
		moving = true;
		ChangeCommonState("moving");
	}
}

//##########################################################################
//Movement
//##########################################################################
#pragma region Movement

void CPlayerBase::UpdateMoves()
{
	PROFILE_FUNCTION("update moves base");

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	VEC3 direction = directionForward + directionLateral;

	CEntity * camera_e = camera;
	TCompTransform* camera_comp = camera_e->get<TCompTransform>();

	direction.Normalize();

	float yaw, pitch;
	camera_comp->getAngles(&yaw, &pitch);
	float new_x, new_z;

	new_x = direction.x * cosf(yaw) + direction.z*sinf(yaw);
	new_z = -direction.x * sinf(yaw) + direction.z*cosf(yaw);

	direction.x = new_x;
	direction.z = new_z;

	direction.Normalize();

	float new_yaw = player_transform->getDeltaYawToAimDirection(direction);
	clampAbs_me(new_yaw, player_rotation_speed * getDeltaTime());
	player_transform->getAngles(&yaw, &pitch);

	player_transform->setAngles(new_yaw + yaw, pitch);

	//Set current velocity with friction
	float drag = 2.5f*getDeltaTime();
	float drag_i = (1 - drag);

	if (moving) player_curr_speed = drag_i*player_curr_speed + drag*player_max_speed;
	else player_curr_speed = drag_i*player_curr_speed - drag*player_max_speed;

	if (player_curr_speed < 0) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = VEC3(0, 0, 0);
	}

	cc->AddMovement(direction, player_curr_speed*getDeltaTime());
	if (moving) UpdateMovingWithOther();
}
#pragma endregion
//##########################################################################

//##########################################################################
//Inputs
//##########################################################################
#pragma region Inputs

bool CPlayerBase::UpdateMovDirection() {
	PROFILE_FUNCTION("update direction base");
	moving = false;

	bool horizontal = false;
	bool vertical = false;
	if (!GameController->GetFreeCamera()) {
		if (io->keys['W'].isPressed() || io->joystick.ly > left_stick_sensibility) {
			directionForward = VEC3(0, 0, 1);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}
		if (io->keys['S'].isPressed() || io->joystick.ly < -left_stick_sensibility) {
			directionForward = VEC3(0, 0, -1);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}

		if (io->keys['A'].isPressed() || io->joystick.lx < -left_stick_sensibility) {
			directionLateral = VEC3(1, 0, 0);
			//TODO: xbobx
			moving = true;
			horizontal = true;
		}
		if (io->keys['D'].isPressed() || io->joystick.lx > left_stick_sensibility) {
			directionLateral = VEC3(-1, 0, 0);
			//TODO: xbobx
			moving = true;
			horizontal = true;
		}
		if ((io->keys[VK_SPACE].isPressed() || io->joystick.button_A.isPressed()) && !gravity_active) {
			directionVertical = VEC3(0, 0.25, 0);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}
		if ((io->keys[VK_MENU].isPressed() || io->joystick.button_A.isPressed()) && !gravity_active) {
			directionVertical = VEC3(0, -0.25, 0);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}

		if (!vertical && moving)
			directionForward = VEC3(0, 0, 0);

		else if (!horizontal && moving)
			directionLateral = VEC3(0, 0, 0);

		else if (gravity_active)
			directionVertical = VEC3(0, 0, 0);
	}
	return moving;
}

void CPlayerBase::UpdateJumpState() {
	PROFILE_FUNCTION("update jump state base");
	if (!canJump()) return;
	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.isPressed()) {
		logic_manager->throwEvent(logic_manager->OnJump, "");
		Jump();
	}
}

bool CPlayerBase::canJump() {
	return false;
}

void CPlayerBase::UpdateDirection() {
	/*	if (Input.IsOrientLeftPressed())
			rotate = 1;
		else if (Input.IsOrientRightPressed())
			rotate = -1;
		else {
			rotate = 0;
		}*/
}

void CPlayerBase::UpdateInputActions() {
	//TODO: actions
}

void CPlayerBase::UpdateMovingWithOther() {
	//TODO: actions
}
#pragma endregion

//##########################################################################

//##########################################################################
// Player States
//##########################################################################
#pragma region Player States
void CPlayerBase::energyDecreasal(float howmuch) {
	PROFILE_FUNCTION("player base: energy dec function");

	TMsgSetDamage msg;
	msg.dmg = howmuch;
	this->myEntity->sendMsg(msg);
}

void CPlayerBase::Idle()
{
	PROFILE_FUNCTION("idle base");
	ChangeCommonState("idle");
	if (!checkDead()) {
		UpdateDirection();
		UpdateJumpState();
		if (UpdateMovDirection()) {
			ChangeCommonState("moving");
			ChangeState("moving");
		}
	}
}

void CPlayerBase::Jump()
{
	PROFILE_FUNCTION("jump base");

	cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f));
	ChangeState("jumping");
	ChangeCommonState("jumping");
}

void CPlayerBase::Die()
{
	PROFILE_FUNCTION("die base");

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	if (!cc->OnGround()) {
		Falling();
	}
	orbitCameraDeath();
	ChangeState("idle");
	ChangeCommonState("idle");
}

void CPlayerBase::Win()
{
	PROFILE_FUNCTION("win base");

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	if (!cc->OnGround()) {
		Falling();
	}
	orbitCameraDeath();
	ChangeState("idle");
	ChangeCommonState("idle");
}

void CPlayerBase::Falling()
{
	PROFILE_FUNCTION("falling base");
	UpdateDirection();
	UpdateMovDirection();

	if (cc->OnGround()) {
		jspeed = 0.0f;
		ChangeState("idle");
		ChangeCommonState("idle");
	}
}

void CPlayerBase::Jumping()
{
	PROFILE_FUNCTION("jumping base");
	UpdateDirection();
	UpdateMovDirection();

	if (cc->GetYAxisSpeed() < 0) {
		ChangeState("falling");
		ChangeCommonState("falling");
	}
}

void CPlayerBase::Moving()
{
	PROFILE_FUNCTION("moving base");
	checkFalling();
	UpdateDirection();
	UpdateJumpState();
	if (!UpdateMovDirection()) {
		ChangeState("idle");
		ChangeCommonState("idle");
	}
}

void CPlayerBase::checkFalling() {
	if (!cc->OnGround() && cc->GetYAxisSpeed() < -.5f) {
		ChangeState("falling");
		ChangeCommonState("falling");
	}
}

#pragma endregion

//##########################################################################

void CPlayerBase::renderInMenu()
{
	PROFILE_FUNCTION("render in menu base");
	VEC3 direction = directionForward + directionLateral + directionVertical;
	direction.Normalize();
	direction = direction + directionJump;

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	ImGui::Text("NODE: %s\n", state.c_str());
	ImGui::Text("position: %.4f, %.4f, %.4f\n", player_position.x, player_position.y, player_position.z);
	ImGui::Text("direction: %.4f, %.4f, %.4f", direction.x, direction.y, direction.z);
	ImGui::Text("jump: %.5f", jspeed);
}

void CPlayerBase::orbitCameraDeath() {
	PROFILE_FUNCTION("orbit camera dead base");
	float angle = getDeltaTime();
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	TCompTransform* target_transform = myEntity->get<TCompTransform>();

	CEntity * camera_e = camera;
	//TCompCamera* camera_transform = camera_e->get<TCompCamera>();
	TCompTransform* player_transform = camera_e->get<TCompTransform>();
	TCompCameraMain* player_cam = camera_e->get<TCompCameraMain>();

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
	//nothing, to do on child
}

map<string, statehandler>* CPlayerBase::getStatemap() {
	//Must implement in subclasses
	return nullptr;
}

//Aux
bool CPlayerBase::turnTo(TCompTransform * t)
{
	float yaw, pitch;
	transform->getAngles(&yaw, &pitch);
	float deltaYaw = transform->getDeltaYawToAimTo(t->getPosition());
	if (abs(deltaYaw) > epsilonYaw) {
		float yaw_added = deltaYaw * getDeltaTime() * player_rotation_speed;
		clampAbs_me(yaw_added, abs(deltaYaw));
		float new_yaw = yaw + yaw_added;
		transform->setAngles(new_yaw, pitch);
		return abs(yaw_added) >= (abs(deltaYaw) - epsilonYaw);
	}
	return true;
}