#include "mcv_platform.h"
#include "player_controller_base.h"

#include <windows.h>
#include "handle\object_manager.h"

#include "windows/app.h"

#include "components\comp_transform.h"
#include "components\comp_life.h"
#include "components\entity.h"
#include "components\entity_tags.h"

#include "components\comp_msgs.h"
#include "components\comp_camera.h"

#include "app_modules\io\io.h"
#include "utils/utils.h"

#include "physics\physics.h"

CPlayerBase::CPlayerBase() {
	AddState("idle", (statehandler)&CPlayerBase::Idle);
	AddState("moving", (statehandler)&CPlayerBase::Moving);
	AddState("falling", (statehandler)&CPlayerBase::Falling);
	AddState("jumping", (statehandler)&CPlayerBase::Jumping);
	AddState("die", (statehandler)&CPlayerBase::Die);
	AddState("win", (statehandler)&CPlayerBase::Win);
}

bool CPlayerBase::checkDead() {
	SetMyEntity();
	TCompLife * player_life = myEntity->get<TCompLife>();
	if (player_life->currentlife <= 0) {
		ChangeState("die");
		return true;
	}
	CEntity * victoryPoint = tags_manager.getFirstHavingTag(getID("victory_point"));
	TCompTransform * player_transform = myEntity->get<TCompTransform>();
	TCompTransform * victoryPoint_transform = victoryPoint->get<TCompTransform>();
	if (0.5f > simpleDist(victoryPoint_transform->getPosition(), player_transform->getPosition())) {
		ChangeState("win");
		return true;
	}
	return false;
}
void CPlayerBase::onSetCamera(const TMsgSetCamera& msg) {
	camera = msg.camera;
}

void CPlayerBase::update(float elapsed) {
	if (controlEnabled) {
		if (io->keys[VK_ESCAPE].becomesPressed() || io->joystick.button_BACK.becomesPressed()) {
			CApp& app = CApp::get();
			app.exitGame();
		}
		if (io->keys[VK_RETURN].becomesPressed() || io->joystick.button_START.becomesPressed()) {
			CApp& app = CApp::get();
			app.restart();
		}
		UpdateInputActions();
		Recalc();
		UpdateMoves();
		myUpdate();
		update_msgs();
	}
}

//##########################################################################
//Movement
//##########################################################################
#pragma region Movement

void CPlayerBase::UpdateMoves()
{
	SetMyEntity();

	ApplyGravity();

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

	//set final position
	if (onGround) {
		player_position = player_position + direction*getDeltaTime()*player_curr_speed;
	}
	else {
		player_position = player_position + direction*getDeltaTime()*(player_curr_speed / 2.0f);
	}

	player_transform->executeMovement(player_position);
	UpdateMovingWithOther();
}
#pragma endregion
//##########################################################################

//##########################################################################
//Inputs
//##########################################################################
#pragma region Inputs

bool CPlayerBase::UpdateMovDirection() {
	moving = false;

	bool horizontal = false;
	bool vertical = false;

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

	if (!vertical && moving)
		directionForward = VEC3(0, 0, 0);

	else if (!horizontal && moving)
		directionLateral = VEC3(0, 0, 0);

	return moving;
}

void CPlayerBase::UpdateJumpState() {
	if (io->keys[VK_SPACE].isPressed() || io->joystick.button_A.isPressed()) {
		Jump();
	}
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
	SetMyEntity();
	TMsgDamage msg;
	msg.points = howmuch;
	msg.dmgType = ENERGY_DECREASE;
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

void CPlayerBase::Win()
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

void CPlayerBase::ApplyGravity() {
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	ray_cast_query floor_query = ray_cast_query(player_position, VEC3(0, -1, 0), 15.0f, COL_TAG_SOLID);
	ray_cast_result res = Physics::calcRayCast(floor_query);
	VEC3 ground = res.positionCollision;
	float d = simpleDist(player_position, ground);

	if (d > 0.1f || jspeed > 0.1f) {
		jspeed -= gravity*getDeltaTime();
		player_position = player_position + VEC3(0, 1, 0)*getDeltaTime()*jspeed;
		//player_transform->setPosition(player_position);
		if (!player_transform->executeMovement(player_position)) {
			onGround = true;
			jspeed = 0.0f;
			ChangeState("idle");
		}
		else {
			if (state != "doublefalling" && jspeed < 0.1f) {
				if (state == "doublejump")
					ChangeState("doublefalling");
				else
					ChangeState("falling");
			}

			onGround = false;
		}
	}
	else {
		onGround = true;
	}
}

void CPlayerBase::Falling()
{
	UpdateDirection();
	UpdateMovDirection();

	if (onGround) {
		jspeed = 0.0f;
		ChangeState("idle");
	}
}

void CPlayerBase::Jumping()
{
	UpdateDirection();
	UpdateMovDirection();
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

	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	ImGui::Text("NODE: %s\n", state.c_str());
	ImGui::Text("position: %.4f, %.4f, %.4f\n", player_position.x, player_position.y, player_position.z);
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
	//nothing, to do on child
}