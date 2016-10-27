#include "mcv_platform.h"
#include "player_controller_mole.h"

#include <windows.h>
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "components/comp_msgs.h"
#include "logic/comp_box.h"

#include "ui/ui_interface.h"
#include "components/comp_charactercontroller.h"
#include "components/comp_physics.h"
#include "components/comp_tasklist.h"

map<string, statehandler> player_controller_mole::statemap = {};

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_MOLE(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_MOLE_P(state) SET_ANIM_STATE_P(animController, state)

//State names
#define ST_MOLE_GRAB "grabbedBox"
#define ST_MOLE_PUSH "pushingBox"
#define ST_MOLE_PILA "grabbedPila"
#define ST_MOLE_GRAB_GO "goToGrab"
#define ST_MOLE_PILA_GO "goToPila"
#define ST_MOLE_PUT_GO "goToPut"
#define ST_MOLE_GRAB_FACE "faceToGrab"
#define ST_MOLE_PILA_FACE "faceToPila"
#define ST_MOLE_PUT_FACE "faceToPut"

#define ST_MOLE_GRABBING_1 "grabbing1"
#define ST_MOLE_GRABBING_2 "grabbing2"
#define ST_MOLE_PILING_1 "grabbingPila1"
#define ST_MOLE_PILING_2 "grabbingPila2"
#define ST_MOLE_GRABBING_IMPACT "grabbing_impact"
#define ST_MOLE_GRABBING_IMPACT_1 "grabbing_impact_1" //Lose box control
#define ST_MOLE_GRABBING_IMPACT_2 "grabbing_impact_2" //Recovery box control
#define ST_MOLE_UNGRAB "leaveBox"
#define ST_MOLE_UNPILA "leavePila"
#define ST_MOLE_UNGRABBING "leavingBox"
#define ST_MOLE_UNPILING "leavingPila"
#define ST_MOLE_PUTTING "puttingPila"
#define ST_MOLE_DESTROY "destroyWall"
#define ST_MOLE_PUSH_PREP "pushBoxPreparation"

#define MOLE_TIME_OUT_GO_GRAB	4.f
#define MOLE_BOX_MIN_DISTANCE	2.1f
#define MOLE_BOX_MAX_DISTANCE	3.5f

void player_controller_mole::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_mole")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields_base = readIniAtrData(file_ini, "controller_base");

			assignValueToVar(player_max_speed, fields_base);
			assignValueToVar(player_rotation_speed, fields_base);
			assignValueToVar(jimpulse, fields_base);
			assignValueToVar(left_stick_sensibility, fields_base);
			assignValueToVar(camera_max_height, fields_base);
			assignValueToVar(camera_min_height, fields_base);

			map<std::string, float> fields_mole = readIniAtrData(file_ini, "controller_mole");

			assignValueToVar(grab_box_energy, fields_mole);
			assignValueToVar(destroy_wall_energy, fields_mole);
			assignValueToVar(push_box_force, fields_mole);
			assignValueToVar(pull_box_force, fields_mole);
		}
	}
}

#define AddStMole(name, func) AddState(name, (statehandler)&player_controller_mole::func)
void player_controller_mole::Init() {
	getUpdateInfoBase(CHandle(this).getOwner());

	// read main attributes from file
	readIniFileAttr();
	mole_max_speed = player_max_speed;
	____TIMER_REDEFINE_(t_grab_hit, 0.1f);
	____TIMER__SET_ZERO_(t_grab_hit);

	om = getHandleManager<player_controller_mole>();	//player

	if (statemap.empty()) {
		//States from controller base and poss controller
		addBasicStates();
		addPossStates();
		AddStMole(ST_MOLE_GRAB_GO, GoToGrab);
		AddStMole(ST_MOLE_PILA_GO, GoToPila);
		AddStMole(ST_MOLE_GRAB_FACE, FaceToGrab);
		AddStMole(ST_MOLE_PILA_FACE, FaceToPila);
		AddStMole(ST_MOLE_GRABBING_1, GrabbingBox1);
		AddStMole(ST_MOLE_PILING_1, GrabbingPila1);
		AddStMole(ST_MOLE_GRABBING_2, GrabbingBox2);
		AddStMole(ST_MOLE_PILING_2, GrabbingPila2);
		AddStMole(ST_MOLE_GRABBING_IMPACT, GrabbingImpact);
		AddStMole(ST_MOLE_GRABBING_IMPACT_1, GrabbingImpact1);
		AddStMole(ST_MOLE_GRABBING_IMPACT_2, GrabbingImpact2);
		AddStMole(ST_MOLE_GRAB, GrabbedBox);
		AddStMole(ST_MOLE_PILA, GrabbedPila);
		AddStMole(ST_MOLE_UNGRAB, LeaveBox);
		AddStMole(ST_MOLE_UNPILA, LeavePila);
		AddStMole(ST_MOLE_UNGRABBING, LeavingBox);
		AddStMole(ST_MOLE_UNPILING, LeavingPila);
		AddStMole(ST_MOLE_PUTTING, PuttingPila);
		AddStMole(ST_MOLE_DESTROY, DestroyWall);
		AddStMole(ST_MOLE_PUSH_PREP, PushBoxPreparation);
		AddStMole(ST_MOLE_PUSH, PushBox);

		AddStMole(ST_MOLE_PUT_GO, GoToPilaContainer);
		AddStMole(ST_MOLE_PUT_FACE, FaceToPilaContainer);
	}

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
	SET_ANIM_MOLE(AST_IDLE);

	pushing_box = false;
	pulling_box = false;

	init_poss();
}

bool player_controller_mole::getUpdateInfo()
{
	if (!CPlayerBase::getUpdateInfo()) return false;
	myParent = CHandle(this).getOwner();
	animController = GETH_MY(SkelControllerMole);
	return true;
}

void player_controller_mole::myUpdate()
{
	//dbg("mole frame\n");

	//if (!boxGrabbed.isValid() && this->nearToBox()) {
	//	VEC3 left, right, front_dir, pos_grab;
	//	VEC3 nleft, nright;
	//	GET_COMP(box, boxNear, TCompBox);
	//	box->getGrabPoints(transform, left, right, front_dir, pos_grab, nleft, nright);
	//	Debug->DrawLine(pos_grab, left, VEC3(1, 1, 0));
	//	Debug->DrawLine(pos_grab, right, VEC3(0, 0, 1));
	//	Debug->DrawLine(left, nleft, 0.2f, VEC3(1, 1, 0));
	//	Debug->DrawLine(right, nright, 0.2f, VEC3(0, 0, 1));
	//}
	if (pilaGrabbed.isValid() && this->nearToPilaContainer()) {
		VEC3 myPos = transform->getPosition();
		GET_COMP(tmxPila, pilaGrabbed, TCompTransform);
		Debug->DrawLine(myPos, tmxPila->getPosition(), VEC3(1, 1, 0));
		Debug->DrawLine(myPos, pilaContainerPos);
	}
	//Debug->DrawLine(transform->getPosition(), transform->getFront(), 1.f);

	if (cc->OnGround() && state == "moving" && !pushing_box) {
		if (player_curr_speed >= player_max_speed - 0.1f)
		{
			SET_ANIM_MOLE(AST_RUN);
		}
		else
		{
			SET_ANIM_MOLE(AST_MOVE);
		}
	}
}

void player_controller_mole::UpdateMoves() {
	if (pushing_box) {
		VEC3 direction = push_pull_direction;

		if (pulling_box)
			direction = -direction;

		//Set current velocity with friction
		float drag = 2.5f*getDeltaTime();
		float drag_i = (1 - drag);

		if (moving) player_curr_speed = drag_i*player_curr_speed + drag*player_max_speed;
		else player_curr_speed = 0.f;

		if (player_curr_speed < 0) {
			player_curr_speed = 0.0f;
			directionForward = directionLateral = VEC3(0, 0, 0);
		}

		cc->AddMovement(direction, player_curr_speed*getDeltaTime());
		if (moving) UpdateMovingWithOther();
	}
	else {
		CPlayerBase::UpdateMoves();
	}
}

bool player_controller_mole::UpdateMovDirection() {
	if (pushing_box) {
		GET_COMP(box_t, boxPushed, TCompTransform);
		float distance_to_box = simpleDistXZ(box_t->getPosition(), getEntityTransform()->getPosition());

		if (distance_to_box < MOLE_BOX_MIN_DISTANCE && !pulling_box) {
			directionForward = VEC3(0, 0, 0);
			directionLateral = VEC3(0, 0, 0);
			directionVertical = VEC3(0, 0, 0);
			return false;
		}
	}

	return CPlayerBase::UpdateMovDirection();
}

void player_controller_mole::UpdateInputActions() {
	if (controller->ActionButtonBecomesPessed()) {
		if (boxGrabbed.isValid()) {
			logic_manager->throwEvent(logic_manager->OnLeaveBox, "");
			ChangeState(ST_MOLE_UNGRAB);
		}
		else if (pilaGrabbed.isValid()) {
			//logic_manager
			if (this->nearToPilaContainer()) {
				inputEnabled = false;
				ChangeState(ST_MOLE_PUT_GO);
			}
			else ChangeState(ST_MOLE_UNPILA);
		}
		else {
			bool nearToPila = this->nearToPila();
			//GET_COMP(pilaComp, pilaNear, TCompPila);
			if (nearToPila && !this->nearToPilaContainerWithPilaCharged()){//!pilaComp->isCharged()) {
				float pitch_dummy;
				getYawPitchFromVector(grabInfo.dir_to_grab, &grabInfo.yaw, &pitch_dummy);
				inputEnabled = false;
				ChangeState(ST_MOLE_PILA_GO);
			}
			else if (this->nearToBox()) {
				//ChangePose(pose_box_route);
				GET_COMP(box, boxNear, TCompBox);
				VEC3 h_target_dummy;
				box->getGrabPoints(transform
					, h_target_dummy
					, h_target_dummy
					, grabInfo.dir_to_grab
					, grabInfo.pos_to_grab
					, h_target_dummy
					, h_target_dummy
				);
				float pitch_dummy;
				getYawPitchFromVector(grabInfo.dir_to_grab, &grabInfo.yaw, &pitch_dummy);
				inputEnabled = false;
				ChangeState(ST_MOLE_GRAB_GO);
			}
			else if (this->nearToWall()) {
				//ChangePose(pose_wall_route);
				logic_manager->throwEvent(logic_manager->OnBreakWall, "");
				ChangeState(ST_MOLE_DESTROY);
			}
		}
	}

	if (pushing_box) {
		GET_COMP(box_t, boxPushed, TCompTransform);
		GET_COMP(box_p, boxPushed, TCompPhysics);
		animController->setState(AST_PUSH_IDLE);
		// comprobacion de distancia
		float distance_to_box = simpleDistXZ(box_t->getPosition(), getEntityTransform()->getPosition());

		//if the box is too near of the player, we leave it
		if (distance_to_box < MOLE_BOX_MIN_DISTANCE) {
			LeaveBox();
			logic_manager->throwEvent(logic_manager->OnPushBoxIdle, "");
		}
		//if somehow the box splits from the player, we leave it
		else if (distance_to_box > MOLE_BOX_MAX_DISTANCE ||
			!getEntityTransform()->isHalfConeVision(box_t->getPosition(), deg2rad(10))) {
			LeaveBox();
			logic_manager->throwEvent(logic_manager->OnPushBoxIdle, "");
		}
		// pushing box
		else if (controller->IsMoveForward()) {
			player_curr_speed = player_max_speed / 2.f;
			pulling_box = false;
			animController->setState(AST_PUSH_WALK);
			box_p->AddMovement(push_pull_direction*push_box_force*player_curr_speed*getDeltaTime());
			logic_manager->throwEvent(logic_manager->OnPushBox, "", boxGrabbed);
		}
		// pulling box
		else if (controller->IsMoveBackWard()) {
			player_curr_speed = player_max_speed / 2.f;
			pulling_box = true;
			//animController->setState(AST_PULL_WALK);
			animController->setState(AST_PUSH_WALK);
			box_p->AddMovement(-push_pull_direction*push_box_force*player_curr_speed*getDeltaTime());
			logic_manager->throwEvent(logic_manager->OnPushBox, "", boxGrabbed);
		}
		else if (controller->IsMoveLeft() || controller->IsMoveRight()) {
			LeaveBox();
			logic_manager->throwEvent(logic_manager->OnPushBoxIdle, "");
		}
		else {
			// If we are pushing box in idle state, we just stop the sound
			player_curr_speed = 0.f;
			animController->setState(AST_IDLE);
			//animController->setState(AST_PUSH_IDLE);
			logic_manager->throwEvent(logic_manager->OnPushBoxIdle, "");
		}
	}
}

void player_controller_mole::UpdateJumpState() {
	PROFILE_FUNCTION("update jump state base");
	if (!canJump()) return;
	if (controller->JumpButtonBecomesPressed()) {
		logic_manager->throwEvent(logic_manager->OnJump, "Mole");
		Jump();
	}
}

void player_controller_mole::UpdateMovingWithOther() {
	if (boxGrabbed.isValid()) {
		GET_COMP(box_t, boxGrabbed, TCompTransform);
		GET_COMP(box_p, boxGrabbed, TCompPhysics);

		if (____TIMER_IS_RUNNING(t_grab_hit)) {
			//VEC3 speed = cc->GetLastSpeed();
			//float amoung = speed.Dot(grabInfo.impact);
			//cc->AddSpeed(-grabInfo.impact * amoung * 10);
			//grabInfo.yaw = transform->getYaw();
		}
	}
}

void player_controller_mole::UpdateUnpossess() {
	CHandle h = CHandle(this);
	tags_manager.removeTag(h.getOwner(), getID("player"));

	if (boxGrabbed.isValid()) {
		LeaveBox();
		//Codigo de Leaving Box sin espera
		GET_COMP(box_p, boxGrabbed, TCompPhysics);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);
		boxGrabbed = CHandle();
		boxPushed = CHandle();
	}
	if (pilaGrabbed.isValid()) {
		LeavePila();
		//Codigo de Leaving Pil sin espera
		GET_COMP(pila_p, pilaGrabbed, TCompPhysics);
		pila_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false);
		pilaGrabbed = CHandle();
	}
	if (boxPushed.isValid()) {
		LeaveBox();
		GET_COMP(box_p, boxPushed, TCompPhysics);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);
		boxGrabbed = CHandle();
		boxPushed = CHandle();
	}
	inputEnabled = true;
	pushing_box = false;
	SET_ANIM_MOLE(AST_STUNNED);
	ChangeState("idle");
}

void player_controller_mole::DestroyWall() {
	energyDecreasal(10.0f);
	SET_ANIM_MOLE(AST_SHOOT);
	vector<CHandle> handles = SBB::readHandlesVector("wptsBreakableWall");
	handles.erase(handles.begin() + selectedWallToBreaki);
	getHandleManager<CEntity>()->destroyHandle(getEntityWallHandle(selectedWallToBreaki));
	SBB::postHandlesVector("wptsBreakableWall", handles);
	//ChangePose(pose_idle_route);
	ChangeState("idle");
}

void player_controller_mole::LeaveBox() {
	// if we were pushing a box, we just stop
	if (pushing_box) {
		boxGrabbed = boxPushed;
		pushing_box = false;
		pulling_box = false;
		//animController->unpushObject();
		logic_manager->throwEvent(logic_manager->OnLeaveBox, "");
	}
	else {
		animController->ungrabObject();
	}

	SBB::postBool(selectedBox, false);
	if (player_max_speed < mole_max_speed)
		player_max_speed *= 2;
	//ChangePose(pose_idle_route);
	ChangeState(ST_MOLE_UNGRABBING);
	stopMovement();
	inputEnabled = false;
}

void player_controller_mole::LeavePila() {
	animController->ungrabPila();
	if (player_max_speed < mole_max_speed)
		player_max_speed *= 2;
	ChangeState(ST_MOLE_UNPILING);
	stopMovement();
	inputEnabled = false;
}
void player_controller_mole::LeavingBox()
{
	static float t_ungrab = SK_MOLE_TIME_TO_UNGRAB;
	t_ungrab -= getDeltaTime();
	if (t_ungrab < 0) {
		t_ungrab = SK_MOLE_TIME_TO_UNGRAB;
		ChangeState("idle");
		inputEnabled = true;
		GET_COMP(box_p, boxGrabbed, TCompPhysics);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);
		boxGrabbed = CHandle();
		boxPushed = CHandle();
	}
}

void player_controller_mole::LeavingPila()
{
	static float t_ungrab_pila = SK_MOLE_TIME_TO_UNGRAB;
	t_ungrab_pila -= getDeltaTime();
	if (t_ungrab_pila < 0) {
		t_ungrab_pila = SK_MOLE_TIME_TO_UNGRAB;
		ChangeState("idle");
		inputEnabled = true;

		GET_COMP(pila_p, pilaGrabbed, TCompPhysics);
		GET_COMP(pila_t, pilaGrabbed, TCompTransform);

		pila_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false);
		pilaGrabbed = CHandle();
		animController->setState(AST_IDLE);
	}
}

void player_controller_mole::FaceToPilaContainer()
{
	bool faced = turnTo(pilaContainerPos);
	if (faced) {
		animController->setState(AST_PUT_PILA);
		ChangeState(ST_MOLE_PUTTING);
		animController->ungrabPila();
	}
}

void player_controller_mole::PuttingPila()
{
	static float t_ungrab_pila = SK_MOLE_TIME_TO_UNGRAB;
	t_ungrab_pila -= getDeltaTime();
	if (t_ungrab_pila < 0) {
		t_ungrab_pila = SK_MOLE_TIME_TO_UNGRAB;
		ChangeState("idle");
		inputEnabled = true;

		GET_COMP(pila_p, pilaGrabbed, TCompPhysics);
		GET_COMP(pila_t, pilaGrabbed, TCompTransform);
		GET_COMP(pila, pilaGrabbed, TCompPila);

		//pila_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false);
		pila->PutIn(pilaContainer);
		CEntity * pilaEntity = pilaContainer;
		/*
#ifdef TASK_LIST_ENABLED
		if (pilaEntity->getId() == 303) {
			CHandle tasklist = tags_manager.getFirstHavingTag(getID("tasklist"));
			CEntity * tasklist_e = tasklist;
			Tasklist * tasklist_comp = tasklist_e->get<Tasklist>();
			tasklist_comp->completeTask(TASKLIST_LEAVE_PILA_ON_CHARGER);
		}
#endif
		*/
		pilaGrabbed = CHandle();
		player_max_speed *= 2;
		animController->setState(AST_IDLE);
	}
}

bool player_controller_mole::nearToWall() {
	bool found = false;
	if (SBB::readHandlesVector("wptsBreakableWall").size() > 0) {
		float distMaxx = 6.0f;
		float distMaxz = 10.5f;
		for (int i = 0; !found && i < SBB::readHandlesVector("wptsBreakableWall").size(); i++) {
			CEntity * entTransform = this->getEntityWallHandle(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowptx = fabsf(fabsf(wpt.x) - fabsf(getEntityTransform()->getPosition().x));
			float disttowptz = fabsf(fabsf(wpt.z) - fabsf(getEntityTransform()->getPosition().z));
			if (disttowptx < distMaxx && disttowptz < distMaxz) {
				distMaxx = disttowptx;
				distMaxz = disttowptz;
				selectedWallToBreaki = i;
				found = true;
			}
		}
	}
	return found;
}
bool player_controller_mole::nearToBox() {
	bool found = false;
	float distMax = 2.0f;
	float higher = -999.9f;
	string key_final = "";
	for (auto h : TCompBox::all_boxes) {
		if (!h.isValid()) continue;
		GET_COMP(tBox, h, TCompTransform);
		string key = ((CEntity*)h)->getName();
		VEC3 wpt = tBox->getPosition();
		float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
		if (disttowpt < distMax + 2 && wpt.y >= higher) {
			distMax = disttowpt;
			higher = wpt.y;
			boxNear = h;
			selectedBox = key;
			found = true;
		}
	}
	return found;
}

bool player_controller_mole::nearToPila() {
	bool found = false;
	float distMax = 4.f;
	float highest = -999.9f;
	for (auto h : TCompPila::all_pilas) {
		if (!h.isValid()) continue;
		GET_COMP(tPila, h, TCompTransform);
		VEC3 pilaPos = tPila->getPosition();
		VEC3 myPos = transform->getPosition();
		if (distY(myPos, pilaPos) < 5.f) {
			float disttowpt = simpleDistXZ(pilaPos, getEntityTransform()->getPosition());
			if (disttowpt < distMax && pilaPos.y >= highest) {
				highest = pilaPos.y;
				pilaNear = h;
				found = true;
			}
		}
	}
	return found;
}

bool player_controller_mole::nearToPilaContainer() {
	float distMax = 4.f;
	pilaContainer = CHandle();
	for (auto h : TCompPilaContainer::all_pila_containers) {
		if (!h.isValid()) continue;
		GET_COMP(tContainer, h, TCompTransform);
		VEC3 containerPos = tContainer->getPosition();
		VEC3 myPos = transform->getPosition();
		if (distY(myPos, containerPos) < 5.f) {
			float disttowpt = simpleDistXZ(containerPos, getEntityTransform()->getPosition());
			if (disttowpt < distMax) {
				pilaContainer = h;
				pilaContainerPos = containerPos;
			}
		}
	}
	return pilaContainer.isValid();
}

bool player_controller_mole::nearToPilaContainerWithPilaCharged() {
	float distMax = 4.f;
	pilaContainer = CHandle();
	for (auto h : TCompPilaContainer::all_pila_containers) {
		if (!h.isValid()) continue;
		GET_COMP(tContainer, h, TCompTransform);
		GET_COMP(pContainer, h, TCompPilaContainer);
		VEC3 containerPos = tContainer->getPosition();
		VEC3 myPos = transform->getPosition();
		if (distY(myPos, containerPos) < 5.f) {
			float disttowpt = simpleDistXZ(containerPos, getEntityTransform()->getPosition());

			if (disttowpt < distMax && pContainer->HasPilaChargedAndInterruptor()) {
				pilaContainer = h;
				pilaContainerPos = containerPos;
			}
		}
	}
	return pilaContainer.isValid();
}

void player_controller_mole::GoToGrab()
{
	static float time_out_to_grab = MOLE_TIME_OUT_GO_GRAB;
	time_out_to_grab -= getDeltaTime();

	float yaw, pitch;
	transform->getAngles(&yaw, &pitch);
	float dist = simpleDistXZ(cc->GetPosition(), grabInfo.pos_to_grab);
	if (dist > epsilonPos && time_out_to_grab > 0) {
		// Go to target
		float deltaYaw = transform->getDeltaYawToAimTo(grabInfo.pos_to_grab);
		if (deltaYaw > epsilonYaw) transform->setAngles(yaw + 0.1f * deltaYaw, pitch);
		VEC3 dir = grabInfo.pos_to_grab - cc->GetPosition();
		dir.Normalize();
		cc->AddMovement(dir, player_max_speed * getDeltaTime());
		moving = true;
		ChangeCommonState("moving");
	}
	else {
		ChangeState(ST_MOLE_GRAB_FACE);
		time_out_to_grab = MOLE_TIME_OUT_GO_GRAB;
	}
}

void player_controller_mole::GoToPila()
{
	GET_COMP(pila_t, pilaNear, TCompTransform);
	VEC3 pila_pos = pila_t->getPosition();
	if (goAndTurnTo(pila_pos)) {
		ChangeState(ST_MOLE_PILA_FACE);
	}
}

void player_controller_mole::GoToPilaContainer()
{
	if (goAndTurnTo(pilaContainerPos)) {
		ChangeState(ST_MOLE_PUT_FACE);
	}
}

bool player_controller_mole::goAndTurnTo(VEC3 target)
{
	static float time_out_to_reach = MOLE_TIME_OUT_GO_GRAB;
	time_out_to_reach -= getDeltaTime();

	float dist = simpleDistXZ(cc->GetPosition(), target);
	if (dist > 1.f && time_out_to_reach > 0) {
		float yaw, pitch;
		transform->getAngles(&yaw, &pitch);
		// Go to target
		float deltaYaw = transform->getDeltaYawToAimTo(target);
		if (deltaYaw > epsilonYaw) transform->setAngles(yaw + 0.1f * deltaYaw, pitch);
		VEC3 dir = target - cc->GetPosition();
		dir.Normalize();
		cc->AddMovement(dir, player_max_speed * getDeltaTime());
		moving = true;
		ChangeCommonState("moving");
		return false;
	}

	time_out_to_reach = MOLE_TIME_OUT_GO_GRAB;
	return true;
}

void player_controller_mole::FaceToGrab()
{
	bool faced = turnTo(GETH_COMP(boxNear, TCompTransform));
	if (faced) {
		GET_COMP(box, boxNear, TCompBox);
		// if the box is MEDIUM (1) we go to "push mode"
		if (box->type_box == 1) {
			boxPushed = boxNear;
			//animController->pushObject(boxNear);
			animController->setState(AST_PUSH_PREP);
			ChangeState(ST_MOLE_PUSH_PREP);
		}
		// if the box is SMALL, we go to grab mode
		else {
			animController->grabObject(boxNear);
			animController->setState(AST_GRAB_DOWN);
			ChangeState(ST_MOLE_GRABBING_1);
		}
	}
}

void player_controller_mole::PushBoxPreparation() {
	static float t_grab2 = SK_MOLE_TIME_TO_GRAB;
	t_grab2 -= getDeltaTime();
	if (t_grab2 < 0) {
		t_grab2 = SK_MOLE_TIME_TO_GRAB;
		ChangeState(ST_MOLE_PUSH);
		animController->setState(AST_PUSH_PREP);
		GET_COMP(box_p, boxPushed, TCompPhysics);
		box_p->getActor()->isRigidBody()->setMass(250.f);
		box_p->getRigidActor()->isRigidBody()->setMass(250.f);

		//push-pull direction
		GET_COMP(box_t, boxPushed, TCompTransform);
		push_pull_direction = box_t->getPosition() - getEntityTransform()->getPosition();
		push_pull_direction.y = 0.f;
		push_pull_direction.Normalize();
		inputEnabled = true;

		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnBoxMode, CApp::get().getCurrentRealLevel(), MY_OWNER);
	}
}

void player_controller_mole::PushBox() {
	if (!SBB::readBool(selectedBox)) {
		SBB::postBool(selectedBox, true);
	}

	pushing_box = true;
	energyDecreasal(5.0f);
	TMsgDamage dmg;
	dmg.modif = 0.5f;
	myEntity->sendMsg(dmg);
	boxGrabbed = boxNear;
	player_max_speed /= 2.f;
	player_curr_speed = 0.f;

	ChangeState("idle");
}

void player_controller_mole::FaceToPila()
{
	bool faced = turnTo(GETH_COMP(pilaNear, TCompTransform));
	if (faced) {
		animController->grabPila(pilaNear);
		animController->setState(AST_GRAB_PILA1);
		ChangeState(ST_MOLE_PILING_1);
	}
}

void player_controller_mole::GrabbingBox1()
{
	static float t_grab1 = SK_MOLE_TIME_TO_GRAB;
	t_grab1 -= getDeltaTime();
	if (t_grab1 < 0) {
		t_grab1 = SK_MOLE_TIME_TO_GRAB;
		ChangeState(ST_MOLE_GRABBING_2);
		animController->setState(AST_GRAB_UP);
	}
}

void player_controller_mole::GrabbingPila1()
{
	static float t_grab_pila1 = SK_MOLE_TIME_TO_GRAB;
	t_grab_pila1 -= getDeltaTime();
	if (t_grab_pila1 < 0) {
		t_grab_pila1 = SK_MOLE_TIME_TO_GRAB;
		ChangeState(ST_MOLE_PILING_2);
		animController->setState(AST_GRAB_PILA2);
	}
}

void player_controller_mole::GrabbingBox2()
{
	//Player return to idlebox pose
	//End IK Stuff progressively
	//Box track player hands

	static float t_grab2 = SK_MOLE_TIME_TO_GRAB;
	t_grab2 -= getDeltaTime();
	if (t_grab2 < 0) {
		t_grab2 = SK_MOLE_TIME_TO_GRAB;
		ChangeState(ST_MOLE_GRAB);
		animController->setState(AST_GRAB_IDLE);
		GET_COMP(box_p, boxNear, TCompPhysics);
		box_p->setKinematic(true);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, true);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, true);
		box_p->setGravity(false);

		inputEnabled = true;
	}
}

void player_controller_mole::GrabbingPila2()
{
	//TODO
	ChangeState(ST_MOLE_PILA);

	GET_COMP(pila_p, pilaNear, TCompPhysics);
	pila_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, true);

	inputEnabled = true;
}

void player_controller_mole::GrabbedBox() {
	if (!SBB::readBool(selectedBox)) {
		SBB::postBool(selectedBox, true);
	}

	//TODO: Revisar esto, fisica caja
	GET_COMP(box_t, boxNear, TCompTransform);
	VEC3 posPlayer = transform->getPosition();
	VEC3 posBox = box_t->getPosition();
	grabInfo.dist = realDistXZ(posPlayer, posBox);
	grabInfo.y = posBox.y - posPlayer.y;
	grabInfo.yaw = transform->getDeltaYawToAimTo(posBox);
	grabInfo.last_correct_pos = posBox;

	energyDecreasal(5.0f);
	TMsgDamage dmg;
	dmg.modif = 0.5f;
	myEntity->sendMsg(dmg);
	boxGrabbed = boxNear;
	player_max_speed /= 2;

	ChangeState("idle");
	logic_manager->throwEvent(logic_manager->OnPickupBox, "");
}

void player_controller_mole::GrabbedPila()
{
	energyDecreasal(5.0f);
	TMsgDamage dmg;
	dmg.modif = 0.5f;
	myEntity->sendMsg(dmg);
	pilaGrabbed = pilaNear;
	player_max_speed /= 2;

	ChangeState("idle");
	animController->setState(AST_PILA_IDLE);

	//logic_manager->throwEvent(logic_manager->OnPickupBox, "");
}

void player_controller_mole::onGrabHit(const TMsgGrabHit& msg)
{
	GET_COMP(box_p, boxGrabbed, TCompPhysics);
	GET_COMP(box_t, boxGrabbed, TCompTransform);
	VEC3 impact = VEC3(0.f, 0.f, 0.f);
	//dbg("--- IMPACT ------------------------------------------\n");
	for (int i = 0; i < msg.npoints; i++) {
		dbg("Point %d, pos = (%f,%f,%f)\n---\n", i, VEC3_VALUES(msg.points[i]));
		dbg("Normal = (%f,%f,%f)\n", VEC3_VALUES(msg.normals[i]));
		dbg("Separation = %f\n", msg.separations[i]);
		dbg("---\n");
		float amoung = msg.separations[i] > 0 ? msg.separations[i] : msg.separations[i] * -2.f;
		VEC3 local_corr = msg.normals[i] * amoung;
		impact = movementUnion(impact, local_corr);
		Debug->DrawLine(msg.points[i], msg.points[i] + local_corr + VEC3(0, 2.f, 0), VEC3(1, 1, 0), 10.f);
	}
	//dbg("------------------------------------- FIN IMPACT ----\n");
	VEC3 dif = grabInfo.last_correct_pos - box_t->getPosition();
	impact.y = 0;
	impact.Normalize();
	grabInfo.impact = impact;

	VEC3 boxPos = box_t->getPosition();
	impact.Normalize();
	Debug->DrawLine(boxPos, 1.f, impact + VEC3(0.f, 0.2f, 0.f), VEC3(0, 0, 1), 10.f);

	____TIMER_RESET_(t_grab_hit);
	ChangeState(ST_MOLE_GRABBING_IMPACT);
}

void player_controller_mole::GrabbingImpact()
{
	//GET_COMP(box_p, boxGrabbed, TCompPhysics);
	//cc->AddMovement(grabInfo.impact);
	//transform->addPosition(grabInfo.impact);
	//box_p->AddMovement(grabInfo.impact);
	//float yaw = transform->getYaw();
	//float deltaYaw = grabInfo.yaw - yaw;
	//transform->setYaw(yaw + deltaYaw*1.2f);
	//grabInfo.last_correct_pos = transform->getPosition();
	//grabInfo.impact = VEC3(0.f, 0.f, 0.f);
	ChangeState(ST_MOLE_GRABBING_IMPACT_1);
	cc->AddImpulse(grabInfo.impact* 4.f);
	stopMovement();
	inputEnabled = false;
}

void player_controller_mole::GrabbingImpact1()
{
	//stopMovement();
	bool recovered_control = ____TIMER__END_(t_grab_hit);
	if (recovered_control) {
		ChangeState(ST_MOLE_GRABBING_IMPACT_2);
	}
	else {
		____TIMER__UPDATE_(t_grab_hit);
	}
}

void player_controller_mole::GrabbingImpact2()
{
	//UpdateMovingWithOther();
	inputEnabled = true;
	ChangeState("idle");
}

void player_controller_mole::InitControlState() {
	CHandle h = CHandle(this);
	tags_manager.addTag(h.getOwner(), getID("player"));
	ChangeState("idle");
}

void player_controller_mole::ChangeCommonState(std::string st)
{
	if (!pushing_box) {
		if (st == "moving") {
			SET_ANIM_MOLE(AST_MOVE);
		}
		else if (st == "running") {
			SET_ANIM_MOLE(AST_RUN);
		}
		else if (st == "jumping") {
			SET_ANIM_MOLE(AST_JUMP);
		}
		else if (st == "falling") {
			SET_ANIM_MOLE(AST_FALL);
		}
		else if (st == "idle") {
			SET_ANIM_MOLE(AST_IDLE);
		}
	}
}

bool player_controller_mole::canJump() {
	if (!controlEnabled) return false; // Control enabled
	if (cc->GetLastSpeed().y > 0.1f) return false; //ascending
	if (cc->GetLastSpeed().y > 0.1f) return false; // descending
	return !boxGrabbed.isValid() && !pilaGrabbed.isValid(); // Not carrying objects
}

void player_controller_mole::Falling()
{
	PROFILE_FUNCTION("falling cientifico");
	UpdateDirection();
	UpdateMovDirection();

	if (cc->OnGround()) {
		jspeed = 0.0f;
		ChangeState("idle");
		ChangeCommonState("idle");

		// landing sound
		TCompRoom* room = myEntity->get<TCompRoom>();
		std::string params = "Mole";
		if (room && room->getSingleRoom() == 2)
			params = params + "Parquet";
		else
			params = params + "Baldosa";

		logic_manager->throwEvent(logic_manager->OnJumpLand, params);
	}
}

//Load and save
bool player_controller_mole::load(MKeyValue& atts)
{
	load_poss(atts);
	return true;
}
bool player_controller_mole::save(std::ofstream& os, MKeyValue& atts)
{
	save_poss(os, atts);
	return true;
}