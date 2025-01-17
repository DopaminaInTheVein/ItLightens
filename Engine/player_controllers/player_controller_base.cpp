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
#include "components/comp_sense_vision.h"

#include "app_modules/io/input_wrapper.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "utils/utils.h"

#include "components/comp_charactercontroller.h"

map<int, string> CPlayerBase::out = {};
CHandle CPlayerBase::handle_player = CHandle();

CPlayerBase::CPlayerBase() {
}

void CPlayerBase::addBasicStates() {
	AddState("idle", (statehandler)&CPlayerBase::Idle);
	AddState("moving", (statehandler)&CPlayerBase::Moving);
	AddState("start_falling", (statehandler)&CPlayerBase::StartFalling);
	AddState("falling", (statehandler)&CPlayerBase::Falling);
	AddState("jumping", (statehandler)&CPlayerBase::Jumping);
	AddState("die", (statehandler)&CPlayerBase::Die);
	AddState("win", (statehandler)&CPlayerBase::Win);
	initBaseAttributes();
}

bool CPlayerBase::getUpdateInfo() {
	myEntity = compBaseEntity;

	cc = compBaseEntity->get<TCompCharacterController>();
	if (!cc) return false;

	transform = compBaseEntity->get<TCompTransform>();
	if (!transform) return false;

	if (!h_sense_vision.isValid()) h_sense_vision = tags_manager.getFirstHavingTag("game_controller");
	if (!h_sense_vision.isValid()) return false;
	sense_vision = GETH_COMP(h_sense_vision, TCompSenseVision);
	if (!sense_vision) return false;

	//Cache handle player
	CHandle myHandle = CHandle(compBaseEntity);
	if (myHandle.hasTag("player")) handle_player = myHandle;

	return true;
}

bool CPlayerBase::checkDead() {
	PROFILE_FUNCTION("checkdead");

	if (TCompLife::isDead()) {
		if (!dead) logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDead, CApp::get().getCurrentRealLevel());
		dead = true;
		ChangeState("die");
		ChangeCommonState("die");
		controlEnabled = false;
		return true;
	}

	if (GameController->GetGameState() == CGameController::VICTORY) {
		ChangeState("win");
		ChangeCommonState("win");
		controlEnabled = false;
		return true;
	}
	return false;
}

void CPlayerBase::onSetCamera(const TMsgSetCamera& msg) {
	camera = msg.camera;
}

void CPlayerBase::onSetControllable(const TMsgSetControllable& msg) {
	setControllable(msg.control);
}
void CPlayerBase::onSetOnlySense(const TMsgSetOnlySense& msg) {
	only_sense = msg.sense;
}

void CPlayerBase::setControllable(bool control)
{
	controlEnabled = control;
	stopMovement();
	ChangeState("idle");
	ChangeCommonState("idle");
	if (!control) {
		if (!h_sense_vision.isValid()) h_sense_vision = tags_manager.getFirstHavingTag("game_controller");
		sense_vision = GETH_COMP(h_sense_vision, TCompSenseVision);
		assert(sense_vision);
		if (!sense_vision) return;
		sense_vision->unregisterHandle(myHandle);
	}
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

float CPlayerBase::getLife()
{
	CEntity * eMe = tags_manager.getFirstHavingTag("raijin");
	assert(eMe);
	TCompLife * life = eMe->get<TCompLife>();
	assert(life || fatal("Player doesnt have life component!"));
	return life->getCurrent();
}

void CPlayerBase::setLife(float new_life)
{
	CEntity * eMe = tags_manager.getFirstHavingTag("raijin");
	assert(eMe);
	TCompLife * life = eMe->get<TCompLife>();
	assert(life || fatal("Player doesnt have life component!"));
	if (!life) return;
	life->setCurrent(new_life);
}

void CPlayerBase::update(float elapsed) {
	PROFILE_FUNCTION("update base");
	if (camera.isValid()) {
		if (onCinematic) {
			UpdateCinematic(elapsed);
		}
		else if (controlEnabled || only_sense) {
			bool alive = !checkDead();
			if (alive && inputEnabled) {
				energy_decrease = energy_default_decrease; // Default if nobody change that this frame
				UpdateSenseVision();
				if (!only_sense) {
					UpdateMoves();
					UpdateInputActions();
				}
				setLife(getLife() - getDeltaTime() * energy_decrease);
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

void CPlayerBase::UpdateSenseVision() {
	if (controller->IsSenseButtonPressed()) {
		energy_decrease = energy_sense_decrease;
		sense_vision->registerHandle(myHandle);
	}
	else {
		sense_vision->unregisterHandle(myHandle);
	}
}

void CPlayerBase::UpdateCinematic(float elapsed) {
	static float time_out = 2.f;
	time_out -= getDeltaTime();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	float yaw, pitch;
	transform->getAngles(&yaw, &pitch);
	float dist = simpleDistXZ(cc->GetPosition(), cinematicTargetPos);
	if (dist < epsilonPos || time_out <= 0.f) {
		if (time_out <= 0.f) cc->teleport(cinematicTargetPos);
		time_out = 2.f;
		// Reach position
		float deltaYaw = cinematicTargetYaw - yaw;
		if (abs(deltaYaw) < epsilonYaw) {
			//In position and Oriented
			onCinematic = false;
			logic_manager->throwUserEvent(cinematicEndCode);
			ChangeCommonState("idle");
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
		dir.y = 0;
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
	if (!GameController->GetFreeCamera() || io->keys[VK_CONTROL].isPressed()) {
		if (controller->IsMoveForward()) {
			float zzz = controller->MoveYNormalized();
			directionForward = VEC3(0, 0, zzz);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}
		if (controller->IsMoveBackWard()) {
			float zzz = controller->MoveYNormalized();
			directionForward = VEC3(0, 0, zzz);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}

		if (controller->IsMoveLeft()) {
			float zzz = controller->MoveXNormalized();
			directionLateral = VEC3(zzz, 0, 0);
			//TODO: xbobx
			moving = true;
			horizontal = true;
		}
		if (controller->IsMoveRight()) {
			float zzz = controller->MoveXNormalized();
			directionLateral = VEC3(zzz, 0, 0);
			//TODO: xbobx
			moving = true;
			horizontal = true;
		}
		if ((controller->IsImpulseUpButtonPressed()) && !gravity_active) {
			directionVertical = VEC3(0, 0.25, 0);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}
		/*
		if ((io->keys[VK_MENU].isPressed() || io->joystick.button_A.isPressed()) && !gravity_active) {
			directionVertical = VEC3(0, -0.25, 0);
			//TODO: xbobx
			moving = true;
			vertical = true;
		}
		*/
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
	if (controller->JumpButtonBecomesPressed()) {
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
// Player States
//##########################################################################
#pragma region Player States
void CPlayerBase::energyDecreasal(float howmuch) {
	PROFILE_FUNCTION("player base: energy dec function");
	CEntity * raijin = tags_manager.getFirstHavingTag(getID("raijin"));

	TMsgSetDamage msg;
	msg.dmg = howmuch;
	raijin->sendMsg(msg);
}

void CPlayerBase::Idle()
{
	PROFILE_FUNCTION("idle base");
	ChangeCommonState("idle");
	if (!checkDead()) {
		UpdateDirection();
		UpdateJumpState();
		if (UpdateMovDirection() && (state == "moving" || state == "idle")) {
			ChangeCommonState("moving");
			ChangeState("moving");
		}
	}
}

void CPlayerBase::initBaseAttributes()
{
	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr_json;
	map<std::string, float> fields_base = readIniAtrData(file_ini, "controller_base");
	assignValueToVar(energy_damage, fields_base);
	assignValueToVar(energy_default_decrease, fields_base);
	assignValueToVar(energy_sense_decrease, fields_base);
}

void CPlayerBase::Jump()
{
	PROFILE_FUNCTION("jump base");

	cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f));
	ChangeState("jumping");
	ChangeCommonState("jumping");
	time_start_falling = 0.f;
}

void CPlayerBase::Die()
{
	PROFILE_FUNCTION("die base");

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	if (!cc->OnGround()) {
		Falling();
	}
	//orbitCameraDeath();
	ChangeState("idle");
	ChangeCommonState("dead");
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
	updateFalling();
}

void CPlayerBase::StartFalling()
{
	PROFILE_FUNCTION("start falling base");
	updateFalling();
	if (time_start_falling > max_time_start_falling) {
		time_start_falling = 0.f;
		ChangeState("falling");
	}
	else {
		time_start_falling += getDeltaTime();
		UpdateJumpState();
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

void CPlayerBase::updateFalling()
{
	UpdateDirection();
	UpdateMovDirection();

	if (cc->OnGround()) {
		jspeed = 0.0f;
		ChangeState("idle");
		ChangeCommonState("idle");
	}
}

void CPlayerBase::Moving()
{
	PROFILE_FUNCTION("moving base");
	if (checkFalling()) {
		time_start_falling = 0;
		ChangeState("start_falling");
		ChangeCommonState("start_falling");
	}
	UpdateDirection();
	UpdateJumpState();
	if (!UpdateMovDirection()) {
		ChangeState("idle");
		ChangeCommonState("idle");
	}
}

bool CPlayerBase::checkFalling() {
	if (!cc->OnGround() && cc->GetYAxisSpeed() < -.5f) {
		return true;
		//ChangeState("falling");
		//ChangeCommonState("falling");
	}
	return false;
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
	ImGui::DragFloat("time_start_falling", &time_start_falling);
	ImGui::DragFloat("max_time_start_falling", &max_time_start_falling);
}

void CPlayerBase::orbitCameraDeath() {
	PROFILE_FUNCTION("orbit camera dead base");
	static float angle_orbit_player = 0.f;
	angle_orbit_player += getDeltaTime();
	float s = sin(angle_orbit_player);
	float c = cos(angle_orbit_player);

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
	return turnTo(t->getPosition());
}

bool CPlayerBase::turnTo(VEC3 target)
{
	float yaw, pitch;
	transform->getAngles(&yaw, &pitch);
	float deltaYaw = transform->getDeltaYawToAimTo(target);
	if (abs(deltaYaw) > epsilonYaw) {
		float yaw_added = deltaYaw * getDeltaTime() * player_rotation_speed;
		clampAbs_me(yaw_added, abs(deltaYaw));
		float new_yaw = yaw + yaw_added;
		transform->setAngles(new_yaw, pitch);
		return abs(yaw_added) >= (abs(deltaYaw) - epsilonYaw);
	}
	return true;
}