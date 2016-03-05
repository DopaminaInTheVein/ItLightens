#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
#include "render\static_mesh.h"
#include "app_modules\io\io.h"
#include "components/comp_msgs.h"
#include "ui\ui_interface.h"

#define DELTA_YAW_SELECTION		deg2rad(10)

void player_controller::Init() {
	om = getHandleManager<player_controller>();	//player

	DeleteState("jumping");
	DeleteState("falling");
	DeleteState("idle");

	AddState("idle", (statehandler)&player_controller::Idle);		// Idle Redo

	AddState("doublefalling", (statehandler)&player_controller::DoubleFalling);		//needed to disable double jump on falling
	AddState("doublejump", (statehandler)&player_controller::DoubleJump);

	AddState("falling", (statehandler)&player_controller::Falling);
	AddState("jumping", (statehandler)&player_controller::Jumping);
	AddState("toplus", (statehandler)&player_controller::AttractToPlus);
	AddState("tominus", (statehandler)&player_controller::AttractToMinus);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
	myEntity = myParent;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	starting_player_y = player_transform->getPosition().y + 2;
	player_y = starting_player_y;

	pose_run = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_jump = getHandleManager<TCompRenderStaticMesh>()->createHandle();

	pose_idle = myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	actual_render = pose_idle;

	pose_idle.setOwner(myEntity);
	pose_run.setOwner(myEntity);
	pose_jump.setOwner(myEntity);

	TCompRenderStaticMesh *mesh;

	mesh = pose_jump;
	mesh->static_mesh = Resources.get("static_meshes/player_jump.static_mesh")->as<CStaticMesh>();

	mesh = pose_run;
	mesh->static_mesh = Resources.get("static_meshes/player_run.static_mesh")->as<CStaticMesh>();

	actual_render->registerToRender();

	ChangeState("idle");
	controlEnabled = true;
	____TIMER__SET_ZERO_(timerDamaged);
}

bool player_controller::isDamaged() {
	return !____TIMER__END_(timerDamaged);
}

void player_controller::ChangePose(CHandle new_pos_h)
{
	TCompRenderStaticMesh *new_pose = new_pos_h;
	if (new_pose == actual_render) return;		//no change

	actual_render->unregisterFromRender();
	actual_render = new_pose;
	//CEntity *me = myParent;
	//me->del<TCompRenderStaticMesh>();
	//me->add(new_pos_h);
	actual_render->registerToRender();
}

void player_controller::myUpdate() {
	____TIMER__UPDATE_(timerDamaged);
	if (!isDamaged()) {
		UpdatePossession();
	}
}

void player_controller::Idle() {
	if (!checkDead()) {
		if (io->keys['1'].isPressed() && nearMinus()) {
			energyDecreasal(getDeltaTime()*0.05f);
			ChangeState("tominus");
		}
		else if (io->keys['2'].isPressed() && nearPlus()) {
			energyDecreasal(getDeltaTime()*0.05f);
			ChangeState("toplus");
		}
		else if (polarizedCurrentSpeed > .2f) {
			energyDecreasal(getDeltaTime()*0.1f);
			polarizedMove = false;
			CEntity * entPoint = nullptr;
			if (tominus) {
				entPoint = this->getMinusPointHandle(topolarizedminus);
			}
			else if (toplus) {
				entPoint = this->getPlusPointHandle(topolarizedplus);
			}
			AttractMove(entPoint);
		}
		else {
			topolarizedplus = -1;
			topolarizedminus = -1;
			polarizedCurrentSpeed = 0.0f;
			CPlayerBase::Idle();
		}
	}
}

void player_controller::DoubleJump()
{
	UpdateDirection();
	UpdateMovDirection();

	if (jspeed <= 0.1f) {
		jspeed = 0.0f;
		ChangeState("doublefalling");
	}
}

void player_controller::DoubleFalling() {
	UpdateDirection();
	UpdateMovDirection();

	if (onGround) {
		jspeed = 0.0f;
		ChangeState("idle");
	}
}

void player_controller::Jumping()
{
	UpdateDirection();
	UpdateMovDirection();

	if (onGround) {
		jspeed = 0.0f;
		ChangeState("idle");
	}

	if (io->keys[VK_SPACE].becomesPressed()) {
		jspeed = jimpulse;
		if (onGround) {
			energyDecreasal(1.0f);
		}
		else {
			energyDecreasal(5.0f);
		}
		ChangeState("doublejump");
	}
}

void player_controller::Falling()
{
	UpdateDirection();
	UpdateMovDirection();

	if (io->keys[VK_SPACE].becomesPressed()) {
		jspeed = jimpulse;
		energyDecreasal(5.0f);
		ChangeState("doublejump");
	}

	if (onGround) {
		jspeed = 0.0f;
		ChangeState("idle");
	}
}

void player_controller::AttractToMinus() {
	CEntity * entPoint = this->getMinusPointHandle(topolarizedminus);
	tominus = true;
	toplus = false;
	AttractMove(entPoint);
	ChangeState("idle");
}
void player_controller::AttractToPlus() {
	CEntity * entPoint = this->getPlusPointHandle(topolarizedplus);
	tominus = false;
	toplus = true;
	AttractMove(entPoint);
	ChangeState("idle");
}

bool player_controller::nearMinus() {
	if (topolarizedminus != -1) {
		return true;
	}
	else {
		bool found = false;
		if (SBB::readHandlesVector("wptsMinusPoint").size() > 0) {
			float distMax = 10.0f;
			for (int i = 0; !found && i < SBB::readHandlesVector("wptsMinusPoint").size(); i++) {
				CEntity * entTransform = this->getMinusPointHandle(i);
				TCompTransform * transformBox = entTransform->get<TCompTransform>();
				VEC3 wpt = transformBox->getPosition();
				float disttowpt = simpleDist(wpt, getEntityTransform()->getPosition());
				if (disttowpt < distMax) {
					distMax = disttowpt;
					topolarizedminus = i;
					found = true;
					polarizedMove = true;
				}
			}
		}
		return found;
	}
}
bool player_controller::nearPlus() {
	if (topolarizedplus != -1) {
		return true;
	}
	else {
		bool found = false;
		if (SBB::readHandlesVector("wptsPlusPoint").size() > 0) {
			float distMax = 10.0f;
			for (int i = 0; !found && i < SBB::readHandlesVector("wptsPlusPoint").size(); i++) {
				CEntity * entTransform = this->getPlusPointHandle(i);
				TCompTransform * transformBox = entTransform->get<TCompTransform>();
				VEC3 wpt = transformBox->getPosition();
				float disttowpt = simpleDist(wpt, getEntityTransform()->getPosition());
				if (disttowpt < distMax) {
					distMax = disttowpt;
					topolarizedplus = i;
					found = true;
					polarizedMove = true;
				}
			}
		}
		return found;
	}
}

void player_controller::AttractMove(CEntity * entPoint) {
	if (entPoint == nullptr) {
		return;
	}
	TCompTransform * entPointTransform = entPoint->get<TCompTransform>();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	VEC3 direction = entPointTransform->getPosition() - player_position;
	float drag = 0.001f;
	float drag_i = (1 - drag);

	if (polarizedMove) polarizedCurrentSpeed = drag_i*polarizedCurrentSpeed + drag*player_max_speed;
	else polarizedCurrentSpeed = drag_i*polarizedCurrentSpeed - drag*player_max_speed;

	float multiplier = getDeltaTime()*polarizedCurrentSpeed * 1.5f;

	float tox = min(fabsf(direction.x*multiplier), fabsf(player_position.x - entPointTransform->getPosition().x));
	float toy = min(fabsf(direction.y*multiplier), fabsf(player_position.y - entPointTransform->getPosition().y));
	float toz = min(fabsf(direction.z*multiplier), fabsf(player_position.z - entPointTransform->getPosition().z));

	if (direction.x < 0) {
		tox *= -1;
	}
	if (direction.z < 0) {
		toz *= -1;
	}
	if (direction.y < 0) {
		toy *= -1;
	}

	player_position.x += tox;

	jspeed = polarizedCurrentSpeed*direction.y;
	player_position.z += toz;
	player_transform->setPosition(player_position);
}

void player_controller::UpdateMoves()
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
	float drag = 0.002f;
	float drag_i = (1 - drag);

	if (moving) player_curr_speed = drag_i*player_curr_speed + drag*player_max_speed;
	else player_curr_speed = drag_i*player_curr_speed - drag*player_max_speed;

	if (player_curr_speed < 0) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = VEC3(0, 0, 0);
	}

	//set final position
	if (onGround) {
		ChangePose(pose_run);
		player_position = player_position + direction*getDeltaTime()*player_curr_speed;
	}
	else {
		ChangePose(pose_jump);
		player_position = player_position + direction*getDeltaTime()*(player_curr_speed / 2.0f);
	}

	if (player_curr_speed == 0.0f) ChangePose(pose_idle);

	player_transform->executeMovement(player_position);
}

float CPlayerBase::possessionCooldown;
//Possession
void player_controller::UpdatePossession() {
	recalcPossassable();
	if (currentPossessable.isValid()) {
		if (io->keys[VK_SHIFT].becomesPressed()) {
			// Se avisa el ai_poss que ha sido poseído
			CEntity* ePoss = currentPossessable;
			TMsgAISetPossessed msg;
			msg.possessed = true;
			ePoss->sendMsg(msg);
			possessionCooldown = 1.0f;
			// Camara Nueva
			CEntity * player_e = tags_manager.getFirstHavingTag(getID("player"));
			TMsgSetTarget msgTarg;
			msgTarg.target = ePoss;
			player_e->sendMsg(msgTarg);

			//Se desactiva el player
			controlEnabled = false;
			SBB::postBool("possMode", true);

			//TODO: Desactivar render
			CEntity * eMe = CHandle(this).getOwner();
			TCompTransform* tMe = eMe->get<TCompTransform>();
			tMe->setPosition(VEC3(0, 200, 0));
			player_curr_speed = 0;
		}
		if (io->mouse.left.becomesPressed()) {
			energyDecreasal(5.0f);
			// Se avisa el ai_poss que ha sido stuneado
			CEntity* ePoss = currentPossessable;
			TMsgAISetStunned msg;
			msg.stunned = true;
			ePoss->sendMsg(msg);
		}
	}
	else if (io->mouse.left.isPressed()) {
		SetMyEntity();
		TCompTransform* player_transform = myEntity->get<TCompTransform>();
		vector<CHandle> ptsRecover = SBB::readHandlesVector("wptsRecoverPoint");
		for (CEntity * ptr : ptsRecover) {
			TCompTransform * ptr_trn = ptr->get<TCompTransform>();
			if (3 > simpleDist(ptr_trn->getPosition(), player_transform->getPosition())) {
				energyDecreasal(-5.0f*getDeltaTime());
			}
		}
	}
}

// Recalcula el mejor candidato para poseer
void player_controller::recalcPossassable() {
	float minDeltaYaw = FLT_MAX;
	float minDistance = FLT_MAX;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	currentPossessable = CHandle();
	VHandles possessables = tags_manager.getHandlesByTag(getID("AI_poss"));
	for (CHandle hPoss : possessables) {
		CEntity* ePoss = hPoss;
		TCompTransform* tPoss = ePoss->get<TCompTransform>();
		VEC3 posPoss = tPoss->getPosition();
		float dist = realDist(player_position, posPoss);
		if (dist < possessionReach) {
			Debug->LogRaw("-----\n");
			float yaw = player_transform->getDeltaYawToAimTo(posPoss);
			yaw = abs(yaw);
			Debug->LogRaw("Yaw: %f\n", rad2deg(yaw));
			if (yaw > deg2rad(90)) continue;

			float improvementDeltaYaw = minDeltaYaw - yaw;
			Debug->LogRaw("Improvement Yaw: %f\n", rad2deg(improvementDeltaYaw));
			bool isBetter = false;
			if (improvementDeltaYaw > DELTA_YAW_SELECTION) {
				isBetter = true;
			}
			else if (improvementDeltaYaw < DELTA_YAW_SELECTION) {
				isBetter = false;
			}
			else {
				isBetter = dist < minDistance;
			}
			if (isBetter) {
				currentPossessable = hPoss;
				minDeltaYaw = abs(yaw);
				minDistance = dist;
			}
		}
	}

	//Debug
	if (currentPossessable.isValid()) {
		CEntity* ePoss = currentPossessable;
		TCompTransform* tPoss = ePoss->get<TCompTransform>();
		VEC3 posPoss = tPoss->getPosition();
		Debug->DrawLine(posPoss + VEC3(-0.1f, 1.5f, -0.1f), posPoss + VEC3(0.1f, 1.5f, 0.1f), BLUE);
		Debug->DrawLine(posPoss + VEC3(0.1f, 1.5f, -0.1f), posPoss + VEC3(-0.1f, 1.5f, 0.1f), BLUE);
	}
}

void player_controller::onLeaveFromPossession(const TMsgPossessionLeave& msg) {
	// Handles y entities necesarias
	CHandle  hMe = CHandle(this).getOwner();
	CEntity* eMe = hMe;
	CHandle hPlayer = tags_manager.getFirstHavingTag(getID("player"));
	CEntity* ePlayer = hPlayer;

	//Colocamos el player
	TCompTransform* tMe = eMe->get<TCompTransform>();
	tMe->lookAt(msg.npcPos, msg.npcPos + msg.npcFront * 1);
	tMe->setPosition(msg.npcPos + msg.npcFront * DIST_LEAVING_POSSESSION);

	//Set 3rd Person Controller
	TMsgSetTarget msg3rdController;
	msg3rdController.target = hMe;
	ePlayer->sendMsg(msg3rdController);

	//Set Camera
	camera = CHandle(ePlayer);

	//Habilitamos control
	controlEnabled = true;

	//Notificamos presencia de Player
	SBB::postBool("possMode", false);
}

void player_controller::update_msgs()
{
	ui.addTextInstructions("Press 'shift' to possess someone\n");
}

void player_controller::onDamage(const TMsgDamage& msg) {
	switch (msg.dmgType) {
	case LASER:
		____TIMER_RESET_(timerDamaged);
		break;
	}
}