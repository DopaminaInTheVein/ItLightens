#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
#include "components/comp_msgs.h"
#include "render\static_mesh.h"
#include "app_modules\io\io.h"
#include "app_modules\logic_manager\logic_manager.h"
#include "handle\handle.h"
#include "ui\ui_interface.h"

#include "components\comp_charactercontroller.h"

#define DELTA_YAW_SELECTION		deg2rad(10)

map<string, statehandler> player_controller::statemap = {};

void player_controller::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("player")) {

			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields_base = readIniAtrData(file_ini, "controller_base");

			assignValueToVar(player_max_speed, fields_base);
			assignValueToVar(player_rotation_speed, fields_base);
			assignValueToVar(jimpulse, fields_base);
			assignValueToVar(left_stick_sensibility, fields_base);
			assignValueToVar(camera_max_height, fields_base);
			assignValueToVar(camera_min_height, fields_base);

			map<std::string, float> fields_player = readIniAtrData(file_ini, "controller_player");

			assignValueToVar(full_height, fields_player);
			assignValueToVar(min_height, fields_player);
			assignValueToVar(DIST_LEAVING_POSSESSION, fields_player);
			assignValueToVar(possessionReach, fields_player);
			assignValueToVar(evolution_limit, fields_player);
			assignValueToVar(max_life, fields_player);
			assignValueToVar(init_life, fields_player);
			assignValueToVar(jump_energy, fields_player);
			assignValueToVar(stun_energy, fields_player);

		}
	}
}

void player_controller::Init() {

	//read main attributes from file
	readIniFileAttr();

	om = getHandleManager<player_controller>();	//player

	if (statemap.empty()) {
		//States from controller base
		addBasicStates();

		DeleteState("jumping");
		DeleteState("falling");

		AddState("doublefalling", (statehandler)&player_controller::DoubleFalling);		//needed to disable double jump on falling
		AddState("doublejump", (statehandler)&player_controller::DoubleJump);

		AddState("falling", (statehandler)&player_controller::Falling);
		AddState("jumping", (statehandler)&player_controller::Jumping);
	}

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
	myEntity = myParent;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();

	pose_run	= getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_jump	= getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_idle	= getHandleManager<TCompRenderStaticMesh>()->createHandle();

	pose_no_ev		= myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	actual_render	= pose_no_ev;

	pose_no_ev.setOwner(myEntity);
	pose_idle.setOwner(myEntity);
	pose_run.setOwner(myEntity);
	pose_jump.setOwner(myEntity);

	TCompRenderStaticMesh *mesh;

	mesh = pose_idle;
	mesh->static_mesh = Resources.get("static_meshes/player_idle.static_mesh")->as<CStaticMesh>();

	mesh = pose_jump;
	mesh->static_mesh = Resources.get("static_meshes/player_jump.static_mesh")->as<CStaticMesh>();

	mesh = pose_run;
	mesh->static_mesh = Resources.get("static_meshes/player_run.static_mesh")->as<CStaticMesh>();

	lastForces = VEC3(0,0,0);

	actual_render->registerToRender();

	ChangeState("idle");
	controlEnabled = true;
	____TIMER__SET_ZERO_(timerDamaged);
}

bool player_controller::isDamaged() {
	PROFILE_FUNCTION("player controller: is damaged");
	return !____TIMER__END_(timerDamaged);
}

void player_controller::rechargeEnergy()
{
	PROFILE_FUNCTION("recharge_energy");
	TCompLife *life = myEntity->get<TCompLife>();
	life->setMaxLife(max_life);
	TCompCharacterController *p = myEntity->get<TCompCharacterController>();
	PxController *cc = p->GetController();
	cc->resize(full_height);
	ChangePose(pose_idle);
}


void player_controller::ChangePose(CHandle new_pos_h)
{
	PROFILE_FUNCTION("player controller: change pose player");
	SetMyEntity();
	TCompLife *life = myEntity->get<TCompLife>();

	if (life->currentlife < evolution_limit && curr_evol > 0) {
		SetCharacterController();
		new_pos_h = pose_no_ev;
		cc->GetController()->resize(min_height);	//update collider size to new form
		curr_evol = 0;
		
		actual_render->unregisterFromRender();
		TCompRenderStaticMesh *new_pose = new_pos_h;
		actual_render = new_pose;
		actual_render = new_pose;
		actual_render->registerToRender();
	}
	else if (curr_evol > 0) {

		TCompRenderStaticMesh *new_pose = new_pos_h;
		if (new_pose == actual_render) return;		//no change

		actual_render->unregisterFromRender();
		actual_render = new_pose;
		actual_render->registerToRender();

	}
}

void player_controller::myUpdate() {
	PROFILE_FUNCTION("player controller: MY_update");
	SetMyEntity();

	TCompTransform *m = myEntity->get<TCompTransform>();

	____TIMER__UPDATE_(timerDamaged);
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 pos = player_transform->getPosition();
	if (!isDamaged()) {
		UpdatePossession();
	}
}

void player_controller::DoubleJump()
{
	PROFILE_FUNCTION("player controller: double jump");
	UpdateDirection();
	UpdateMovDirection();

	SetCharacterController();

	if (cc->GetYAxisSpeed() < 0.0f) {
		ChangeState("doublefalling");
	}
}

void player_controller::DoubleFalling() {
	PROFILE_FUNCTION("player controller: double falling");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();
	if (cc->OnGround()) {
		ChangeState("idle");
	}
}

void player_controller::Jumping()
{
	PROFILE_FUNCTION("player controller: jumping");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();

	if (cc->GetYAxisSpeed() <= 0.0f)
		ChangeState("falling");

	if (cc->OnGround()) {
		ChangeState("idle");
	}

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		cc->AddImpulse(VEC3(0.0f,jimpulse,0.0f));
		energyDecreasal(5.0f);
		logic_manager->throwEvent(logic_manager->OnDoubleJump, "");
		ChangeState("doublejump");
	}
}

void player_controller::Falling()
{
	PROFILE_FUNCTION("player controller: falling");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();

	//Debug->LogRaw("%s\n", io->keys[VK_SPACE].becomesPressed() ? "true" : "false");

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		cc->AddImpulse(VEC3(0.0f,jimpulse,0.0f));
		energyDecreasal(5.0f);
		logic_manager->throwEvent(logic_manager->OnDoubleJump, "");
		ChangeState("doublejump");
	}

	if (cc->OnGround()) {
		ChangeState("idle");
	}
}


void player_controller::RecalcAttractions()
{
	PROFILE_FUNCTION("player controller: recalc attraction");
	if (pol_state == NEUTRAL) return;	//check if polarized is neutral, no effect
	VEC3 forces = VEC3(0,0,0);

	for (auto force : force_points) {		//sum of all forces
		if (force.pol == NEUTRAL) continue;		//if pol is neutral shouldnt have any effect
		if(pol_state == force.pol) forces += AttractMove(force.point);
		else if (pol_state != force.pol) forces -= AttractMove(force.point);
	}

	SetCharacterController();
	//forces.Normalize();


	//float drag = getDeltaTime();
	//float drag_i = 1 - drag;
	//pol_speed = drag_i*pol_speed + drag*player_max_speed;

	forces = (lastForces + forces) / 2;	//smooth change of forces
	lastForces = forces;
	cc->AddSpeed(forces*getDeltaTime());
	//cc->AddImpulse(forces);
}

VEC3 player_controller::AttractMove(VEC3 point_pos) {
	PROFILE_FUNCTION("player controller: attract move");
	
	SetMyEntity();
	point_pos.y += 0.5f;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	VEC3 direction = point_pos - player_position;
	direction.Normalize();
	//return 50*10*direction/squared(simpleDist(player_position,point_pos));
	return 50*direction / (simpleDist(player_position, point_pos)/2.0f);
}

void player_controller::UpdateMoves()
{
	PROFILE_FUNCTION("player controller: update moves");
	SetMyEntity();
	SetCharacterController();

	//tests
	if (io->keys['B'].becomesPressed()) {
		cc->SetCollisions(false);
		Debug->LogRaw("collisions false\n");
	}

	if (io->keys['V'].becomesPressed()) {
		cc->SetCollisions(true);
		Debug->LogRaw("collisions true\n");
	}

	//endtests

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

	player_transform->setAngles(new_yaw + yaw, pitch);		//control rotate transfom, not needed for the character controller

	//Set current velocity with friction
	float drag = 2.5f*getDeltaTime();
	float drag_i = (1 - drag);

	if (moving) player_curr_speed = drag_i*player_curr_speed + drag*player_max_speed;
	else player_curr_speed = drag_i*player_curr_speed - drag*player_max_speed;

	if (player_curr_speed < 0) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = VEC3(0, 0, 0);
	}

	if (cc->OnGround() && player_curr_speed != 0.0f) {
		ChangePose(pose_run);
	}
	else if(player_curr_speed != 0.0f) {
		ChangePose(pose_jump);
	}else if (player_curr_speed == 0.0f) ChangePose(pose_idle);

	if(cc->OnGround())
		cc->AddMovement(direction*player_curr_speed*getDeltaTime());
	else {
		cc->AddMovement(direction*player_curr_speed*getDeltaTime()/2);
	}
}

void player_controller::UpdateInputActions()
{
	PROFILE_FUNCTION("update input actions");
	SetCharacterController();
	if ((io->keys['1'].isPressed() || io->joystick.button_L.isPressed())) {
		pol_state = PLUS;
		logic_manager->throwEvent(logic_manager->OnChangePolarity, "");
		if (!affectPolarized && force_points.size() != 0) {
			affectPolarized = true;
			pol_speed = 0;
			//cc->SetGravity(false);
		}
		else if (affectPolarized && force_points.size() == 0) {
			affectPolarized = false;
			//cc->SetGravity(true);
		}
		RecalcAttractions();
	}
	else if ((io->keys['2'].isPressed() || io->joystick.button_R.isPressed())) {
		pol_state = MINUS;
		logic_manager->throwEvent(logic_manager->OnChangePolarity, "");
		if (!affectPolarized && force_points.size() != 0) {
			affectPolarized = true;
			pol_speed = 0;
			//cc->SetGravity(false);
		}
		else if (affectPolarized && force_points.size() == 0) {
			affectPolarized = false;
			//cc->SetGravity(true);
		}

		RecalcAttractions();
		
	}
	else {
		pol_state = NEUTRAL;
		if (affectPolarized) {
			affectPolarized = false;
			//cc->SetGravity(true);
		}
	}
	
	if ((io->mouse.left.becomesReleased() || io->joystick.button_X.becomesReleased()) && nearStunable()) {
		energyDecreasal(5.0f);
		// Se avisa el ai_poss que ha sido stuneado
		CEntity* ePoss = currentStunable;
		TMsgAISetStunned msg;
		msg.stunned = true;
		ePoss->sendMsg(msg);
		logic_manager->throwEvent(logic_manager->OnStun, "");
	}
	

	if (last_pol_state != pol_state) {
		last_pol_state = pol_state;
		SendMessagePolarizeState();
	}

	UpdateActionsTrigger();
}

void player_controller::UpdateActionsTrigger() {
	PROFILE_FUNCTION("player_controller: update Actions trigger");

	if (canRecEnergy) {
		ui.addTextInstructions("\n Press 'E' to recharge energy\n");
		if (io->keys['E'].becomesPressed()) {
			rechargeEnergy();
			curr_evol = 1;
			logic_manager->throwEvent(logic_manager->OnUseGenerator, "");
		}
	}

	if (canPassWire) {
		ui.addTextInstructions("\n Press 'E' to pass by the wire\n");

		if (io->keys['E'].becomesPressed()) {
			SetMyEntity();
			SetCharacterController();
			cc->GetController()->setPosition(PhysxConversion::Vec3ToPxExVec3(endPointWire));
			logic_manager->throwEvent(logic_manager->OnUseCable, "");
		}
	}
}

void player_controller::SetCharacterController()
{
	PROFILE_FUNCTION("set cc");
	SetMyEntity();
	cc = myEntity->get<TCompCharacterController>();
}

float CPlayerBase::possessionCooldown;
//Possession
void player_controller::UpdatePossession() {
	PROFILE_FUNCTION("update poss");
	recalcPossassable();
	if (currentPossessable.isValid()) {
		if (io->keys[VK_SHIFT].becomesPressed() || io->joystick.button_Y.becomesPressed()) {
			// Se avisa el ai_poss que ha sido poseído
			CEntity* ePoss = currentPossessable;
			TMsgAISetPossessed msg;
			msg.possessed = true;
			ePoss->sendMsg(msg);
			possessionCooldown = 1.0f;
			// Camara Nueva
			CEntity * camera_e = tags_manager.getFirstHavingTag(getID("camera_main"));
			TMsgSetTarget msgTarg;
			msgTarg.target = ePoss;
			msgTarg.who = PLAYER;
			camera_e->sendMsg(msgTarg);

			//Se desactiva el player
			controlEnabled = false;
			SBB::postBool("possMode", true);

			//TODO: Desactivar render
			SetCharacterController();
			cc->SetActive(false);
			cc->GetController()->setPosition(PxExtendedVec3(0, 200, 0));
			TCompTransform *t = myEntity->get<TCompTransform>();
			t->setPosition(VEC3(0, 200, 0));
			player_curr_speed = 0;

			logic_manager->throwEvent(logic_manager->OnPossess, "");
		}
	}
}

// Recalcula el mejor candidato para poseer
void player_controller::recalcPossassable() {
	PROFILE_FUNCTION("recalc possessable");
	float minDeltaYaw = FLT_MAX;
	float minDistance = FLT_MAX;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	currentPossessable = CHandle();
	VHandles possessables = tags_manager.getHandlesByTag(getID("AI_poss"));
	for (CHandle hPoss : possessables) {
		if (!hPoss.isValid()) continue;
		CEntity* ePoss = hPoss;
		if (!ePoss) continue;
		TCompTransform* tPoss = ePoss->get<TCompTransform>();
		VEC3 posPoss = tPoss->getPosition();
		float dist = realDist(player_position, posPoss);
		if (dist < possessionReach) {
			float yaw = player_transform->getDeltaYawToAimTo(posPoss);
			yaw = abs(yaw);
			if (yaw > deg2rad(90)) continue;

			float improvementDeltaYaw = minDeltaYaw - yaw;
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

// Calcula el mejor candidato para stunear
bool player_controller::nearStunable() {
	PROFILE_FUNCTION("near stunnable");
	float minDeltaYaw = FLT_MAX;
	float minDistance = FLT_MAX;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	currentStunable = CHandle();
	VHandles stuneables = tags_manager.getHandlesByTag(getID("AI_poss"));
	for (CHandle hPoss : stuneables) {
		if (!hPoss.isValid()) continue;
		CEntity* ePoss = hPoss;
		if (!ePoss) continue;
		TCompTransform* tPoss = ePoss->get<TCompTransform>();
		VEC3 posPoss = tPoss->getPosition();
		float dist = realDist(player_position, posPoss);
		if (dist < possessionReach) {
			float yaw = player_transform->getDeltaYawToAimTo(posPoss);
			yaw = abs(yaw);
			if (yaw > deg2rad(90)) continue;

			float improvementDeltaYaw = minDeltaYaw - yaw;
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
				currentStunable = hPoss;
				minDeltaYaw = abs(yaw);
				minDistance = dist;
			}
		}
	}

	//Debug
	if (currentStunable.isValid()) {
		return true;
	}
	return false;
}

void player_controller::onLeaveFromPossession(const TMsgPossessionLeave& msg) {
	PROFILE_FUNCTION("on leave poss");
	// Handles y entities necesarias
	CHandle  hMe = CHandle(this).getOwner();
	CEntity* eMe = hMe;
	CHandle hCamera = tags_manager.getFirstHavingTag(getID("camera_main"));
	CEntity* eCamera = hCamera;
	

	//Colocamos el player
	SetCharacterController();
	VEC3 pos = msg.npcPos + VEC3(0.0f, cc->GetHeight(), 0.0f);	//to be sure the collider will be above the ground, add height from collider, origin on center shape
	TCompTransform* tMe = eMe->get<TCompTransform>();
	cc->GetController()->setPosition(PhysxConversion::Vec3ToPxExVec3(pos + msg.npcFront * DIST_LEAVING_POSSESSION));	//set collider position
	tMe->setPosition(msg.npcPos + msg.npcFront * DIST_LEAVING_POSSESSION);												//set render position
	cc->SetActive(true);


	//Set 3rd Person Controller
	TMsgSetTarget msg3rdController;
	msg3rdController.target = hMe;
	eCamera->sendMsg(msg3rdController);

	//Set Camera
	camera = CHandle(eCamera);

	//Habilitamos control
	controlEnabled = true;

	//Notificamos presencia de Player
	SBB::postBool("possMode", false);
}

void player_controller::update_msgs()
{
	PROFILE_FUNCTION("updat mesgs");
	ui.addTextInstructions("Press 'l-shift' to possess someone\n");
}

void player_controller::onWirePass(const TMsgWirePass & msg)
{
	canPassWire = msg.range;
	endPointWire = msg.dst;
}

void player_controller::onCanRec(const TMsgCanRec & msg)
{
	canRecEnergy = msg.range;
}

void player_controller::onPolarize(const TMsgPolarize & msg)
{
	if (!msg.range) {
		TForcePoint fp_remove = TForcePoint(msg.origin, msg.pol);
		force_points.erase(std::remove(force_points.begin(), force_points.end(), fp_remove), force_points.end());
	}else{
		TForcePoint newForce = TForcePoint(msg.origin, msg.pol);
		force_points.push_back(newForce);
	}
}

void player_controller::SendMessagePolarizeState()
{
	TMsgPlayerPolarize msg;
	msg.type = pol_state;
	VHandles hs = tags_manager.getHandlesByTag(getID("box"));
	for (CEntity *e : hs) {
		e->sendMsg(msg);
	}
}

map<string, statehandler>* player_controller::getStatemap() {
	return &statemap;
}
