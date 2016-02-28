#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"

#include "components/comp_msgs.h"

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

	ChangeState("idle");
	controlEnabled = true;
}

void player_controller::Idle() {
	if (!checkDead()) {
		UpdatePossession();
		if (Input.IsMinusPolarityPressedDown() && nearMinus()) {
			energyDecreasal(getDeltaTime()*0.05f);
			ChangeState("tominus");
		}
		else if (Input.IsPlusPolarityPressedDown() && nearPlus()) {
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
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	jspeed -= gravity*getDeltaTime();
	if (jspeed <= 0.1f) {
		jspeed = 0.0f;
		ChangeState("doublefalling");
	}
}

void player_controller::DoubleFalling() {
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

void player_controller::Jumping()
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
	if (Input.IsSpacePressedDown()) {
		jspeed = jimpulse;
		if (player_position.y <= 0) {
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
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	jspeed -= gravity*getDeltaTime();

	if (Input.IsSpacePressedDown()) {
		jspeed = jimpulse;
		energyDecreasal(5.0f);
		ChangeState("doublejump");
	}

	if (player_position.y <= 0) {
		onGround = true;
		jspeed = 0.0f;
		directionJump = VEC3(0, 0, 0);
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
			float distMax = 500.0f;
			for (int i = 0; !found && i < SBB::readHandlesVector("wptsMinusPoint").size(); i++) {
				CEntity * entTransform = this->getMinusPointHandle(i);
				TCompTransform * transformBox = entTransform->get<TCompTransform>();
				VEC3 wpt = transformBox->getPosition();
				float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
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
			float distMax = 500.0f;
			for (int i = 0; !found && i < SBB::readHandlesVector("wptsPlusPoint").size(); i++) {
				CEntity * entTransform = this->getPlusPointHandle(i);
				TCompTransform * transformBox = entTransform->get<TCompTransform>();
				VEC3 wpt = transformBox->getPosition();
				float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
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
	float toz = min(fabsf(direction.z*multiplier), fabsf(player_position.z - entPointTransform->getPosition().z));

	if (direction.x < 0) {
		tox *= -1;
	}if (direction.z < 0) {
		toz *= -1;
	}

	player_position.x += tox;
	player_position.z += toz;
	player_transform->setPosition(player_position);
}

//Possession
void player_controller::UpdatePossession() {
	recalcPossassable();
	if (currentPossessable.isValid()) {
		if (Input.IsLeftClickPressedDown()) {
			// Se avisa el ai_poss que ha sido poseído
			CEntity* ePoss = currentPossessable;
			TMsgAISetPossessed msg;
			msg.possessed = true;
			ePoss->sendMsg(msg);

			//Se desactiva el player
			controlEnabled = false;

			//TODO: Desactivar render
			CEntity* eMe = CHandle(this).getOwner();
			TCompTransform* tMe = eMe->get<TCompTransform>();
			tMe->setPosition(VEC3(0, 100, 0));
		}
		else {
			____TIMER_CHECK_DO_(timeShowAblePossess);
			dbg("Press to POSSESS!\n");
			____TIMER_CHECK_DONE_(timeShowAblePossess);
		}
	}
	else {
		____TIMER_CHECK_DO_(timeShowAblePossess);
		dbg("_\n");
		____TIMER_CHECK_DONE_(timeShowAblePossess);
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
		float dist = realDistXZ(player_position, posPoss);
		if (dist < possessionReach) {
			float yaw = player_transform->getDeltaYawToAimTo(posPoss);
			if (abs(yaw) > deg2rad(90)) continue;
			if (yaw < minDeltaYaw) {
				bool isBetter = false;
				if (minDeltaYaw - yaw > deg2rad(2)) {
					isBetter = true;
				}
				else if (dist < minDistance) {
					isBetter = true;
				}
				if (isBetter) {
					currentPossessable = hPoss;
					minDeltaYaw = yaw;
					minDistance = dist;
				}
			}
		}
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
}