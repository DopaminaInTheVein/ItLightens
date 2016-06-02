#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
#include "components/comp_msgs.h"
#include "render/static_mesh.h"
#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "app_modules/gui/gui.h"

#include "handle/handle.h"
#include "ui/ui_interface.h"

#include "components/comp_charactercontroller.h"
#include "logic/polarity.h"

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
	animController.init(myEntity);

	setLife(init_life);
	//pose_run = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	//pose_jump = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	//pose_idle = getHandleManager<TCompRenderStaticMesh>()->createHandle();

	//pose_no_ev = myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	//actual_render = pose_no_ev;

	//pose_no_ev.setOwner(myEntity);
	//pose_idle.setOwner(myEntity);
	//pose_run.setOwner(myEntity);
	//pose_jump.setOwner(myEntity);

	//TCompRenderStaticMesh *mesh;

	//mesh = pose_idle;
	//mesh->static_mesh = Resources.get("static_meshes/player_idle.static_mesh")->as<CStaticMesh>();

	//mesh = pose_jump;
	//mesh->static_mesh = Resources.get("static_meshes/player_jump.static_mesh")->as<CStaticMesh>();

	//mesh = pose_run;
	//mesh->static_mesh = Resources.get("static_meshes/player_run.static_mesh")->as<CStaticMesh>();

	lastForces = VEC3(0, 0, 0);

	//actual_render->registerToRender();

	ChangeState("idle");
	animController.setState(AST_IDLE);
	controlEnabled = true;
	____TIMER__SET_ZERO_(timerDamaged);
}

bool player_controller::isDamaged() {
	PROFILE_FUNCTION("player controller: is damaged");
	return !____TIMER__END_(timerDamaged);
}

float player_controller::getLife()
{
	CEntity * eMe = CHandle(this).getOwner();
	assert(eMe);
	TCompLife * life = eMe->get<TCompLife>();
	assert(life || fatal("Player doesnt have life component!"));
	return life->getCurrent();
}

void player_controller::setLife(float new_life)
{
	CEntity * eMe = CHandle(this).getOwner();
	assert(eMe);
	TCompLife * life = eMe->get<TCompLife>();
	assert(life || fatal("Player doesnt have life component!"));
	life->setCurrent(new_life);
}

void player_controller::rechargeEnergy()
{
	PROFILE_FUNCTION("recharge_energy");
	Evolve(eEvol::second);
	//ChangePose(pose_idle);
}

/*
void player_controller::ChangePose(CHandle new_pos_h)
{
	PROFILE_FUNCTION("player controller: change pose player");
	SetMyEntity();
	TCompLife *life = myEntity->get<TCompLife>();
	if (life->currentlife > evolution_limit && curr_evol == 0) {
		createEvolveLight();
	}

	if (life->currentlife < evolution_limit && curr_evol > 0) {
		SetCharacterController();
		new_pos_h = pose_no_ev;
		cc->GetController()->resize(min_height);	//update collider size to new form
		curr_evol = 0;

		actual_render->unregisterFromRender();
		TCompRenderStaticMesh *new_pose = new_pos_h;
		actual_render = new_pose;
		actual_render->registerToRender();
		createDevolveLight();
	}
	else if (curr_evol > 0) {
		TCompRenderStaticMesh *new_pose = new_pos_h;
		if (new_pose == actual_render) return;		//no change

		actual_render->unregisterFromRender();
		actual_render = new_pose;
		actual_render->registerToRender();
	}
}
*/

void player_controller::createEvolveLight() {
	TCompTransform * trans = myEntity->get<TCompTransform>();

	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;

	auto hm1 = CHandleManager::getByName("name");
	CHandle new_hn = hm1->createHandle();
	MKeyValue atts1;
	atts1.put("name", "light_evolve");
	new_hn.load(atts1);
	entity->add(new_hn);
	auto hm2 = CHandleManager::getByName("transform");
	CHandle new_ht = hm2->createHandle();
	MKeyValue atts2;
	VEC3 positio = trans->getPosition(), scal = trans->getScale();
	VEC4 rota = trans->getRotation();
	atts2["pos"] = std::to_string(positio.x) + " " + std::to_string(positio.y + 1.0f) + " " + std::to_string(positio.z);
	atts2["quat"] = std::to_string(rota.x) + " " + std::to_string(rota.y) + " " + std::to_string(rota.z) + " " + std::to_string(rota.w);
	atts2["scale"] = std::to_string(scal.x) + " " + std::to_string(scal.y) + " " + std::to_string(scal.z);
	new_ht.load(atts2);
	entity->add(new_ht);
	auto hm3 = CHandleManager::getByName("light_fadable");
	CHandle new_hl = hm3->createHandle();
	MKeyValue atts3;
	atts3["color"] = "0 255 0 255";
	atts3["in_radius"] = "1.0";
	atts3["out_radius"] = "1.5";
	atts3["ttl"] = "3.5";
	new_hl.load(atts3);
	entity->add(new_hl);
	TCompLightFadable * tclp = new_hl;
	tclp->activate();
}
void player_controller::createDevolveLight() {
	TCompTransform * trans = myEntity->get<TCompTransform>();

	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;
	IdEntities::saveIdEntity(CHandle(entity), -1900);

	auto hm1 = CHandleManager::getByName("name");
	CHandle new_hn = hm1->createHandle();
	MKeyValue atts1;
	atts1.put("name", "light_evolve");
	new_hn.load(atts1);
	entity->add(new_hn);
	auto hm2 = CHandleManager::getByName("transform");
	CHandle new_ht = hm2->createHandle();
	MKeyValue atts2;
	VEC3 positio = trans->getPosition(), scal = trans->getScale();
	VEC4 rota = trans->getRotation();
	atts2["pos"] = std::to_string(positio.x) + " " + std::to_string(positio.y + 1.0f) + " " + std::to_string(positio.z);
	atts2["quat"] = std::to_string(rota.x) + " " + std::to_string(rota.y) + " " + std::to_string(rota.z) + " " + std::to_string(rota.w);
	atts2["scale"] = std::to_string(scal.x) + " " + std::to_string(scal.y) + " " + std::to_string(scal.z);
	new_ht.load(atts2);
	entity->add(new_ht);
	auto hm3 = CHandleManager::getByName("light_fadable");
	CHandle new_hl = hm3->createHandle();
	MKeyValue atts3;
	atts3["color"] = "255 0 0 255";
	atts3["in_radius"] = "1.0";
	atts3["out_radius"] = "1.5";
	atts3["ttl"] = "3.5";
	new_hl.load(atts3);
	entity->add(new_hl);
	TCompLightFadable * tclp = new_hl;
	tclp->activate();
}

void player_controller::myUpdate() {
	PROFILE_FUNCTION("player controller: MY_update");
	UpdateDamage();
	____TIMER__UPDATE_(timerDamaged);
	if (isDamaged()) {
		UpdateOverCharge();
	}
	else {
		UpdatePossession();
	}

	if (cc->OnGround() && state == "moving") {
		if (player_curr_speed >= player_max_speed - 0.1f)
			animController.setState(AST_RUN);
		else
			animController.setState(AST_MOVE);
	}
}

void player_controller::Idle() {
	CPlayerBase::Idle();
	if (cc->GetYAxisSpeed() < -0.1f) {
		animController.setState(AST_FALL);
	}
	else {
		animController.setState(AST_IDLE);
	}
	myExtraIdle();
}

void player_controller::myExtraIdle() {
	//if (pol_state != NEUTRAL) {
	//	SetCharacterController();
	//	ChangeState("falling");
	//	if (!cc->OnGround()) animController.setState(AST_FALL);
	//}
}

void player_controller::UpdateDamage()
{
	SetMyEntity();
	TCompLife * life = myEntity->get<TCompLife>();
	if (life->energyDamageScale > 0.2f) {
		____TIMER_RESET_(timerDamaged);
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
		animController.setState(AST_FALL);
	}
}

void player_controller::DoubleFalling() {
	if (pol_orbit && !pol_orbit_prev) {
		ChangeState("falling");
		animController.setState(AST_FALL);
	}
	PROFILE_FUNCTION("player controller: double falling");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();
	if (cc->OnGround()) {
		ChangeState("idle");
		animController.setState(AST_IDLE);
	}
}

bool player_controller::canJump() {
	bool can_jump = true;
	//if (pol_orbit) can_jump = false;
	if (polarityForces.size() > 0 && pol_state != NEUTRAL) can_jump = false;
	return can_jump;
}

void player_controller::Jump()
{
	PROFILE_FUNCTION("jump player");
	SetCharacterController();
	bool ascending = cc->GetLastSpeed().y > 0.1f;
	VEC3 jumpVector;
	if (isMoving()) {
		forward_jump = true;
		//set false when on ground
		//-------------------------------------
		VEC3 curSpeed = cc->GetLastSpeed();
		jumpVector = VEC3(
			-curSpeed.x * 0.1f,
			clamp(jimpulse - curSpeed.Length()*0.2f, 0.5f * jimpulse, 0.9f * jimpulse),
			-curSpeed.z * 0.1f
		);
		//--------------------------------------
	}
	else {
		jumpVector = VEC3(0.f, jimpulse, 0.f);
	}
	cc->AddImpulse(jumpVector);
	energyDecreasal(jump_energy);
	if (ascending) {
		ChangeState("doublejump");
		animController.setState(AST_JUMP2);
	}
	else {
		ChangeState("jumping");
		animController.setState(AST_JUMP);
	}
}

void player_controller::Jumping()
{
	PROFILE_FUNCTION("player controller: jumping");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();

	if (cc->GetYAxisSpeed() <= 0.0f) {
		ChangeState("falling");
		animController.setState(AST_FALL);
	}
	if (cc->OnGround() && !(cc->GetYAxisSpeed() > 0.0f)) {
		ChangeState("idle");
		animController.setState(AST_IDLE);
	}

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f), true);
		energyDecreasal(jump_energy);
		logic_manager->throwEvent(logic_manager->OnDoubleJump, "");
		ChangeState("doublejump");
		animController.setState(AST_JUMP2);
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
		cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f), true);
		energyDecreasal(jump_energy);
		logic_manager->throwEvent(logic_manager->OnDoubleJump, "");
		ChangeState("doublejump");
		animController.setState(AST_JUMP2);
	}

	if (cc->OnGround()) {
		ChangeState("idle");
		animController.setState(AST_IDLE);
	}
}

void player_controller::RecalcAttractions()
{
	PROFILE_FUNCTION("player controller: recalc attraction");

	// Calc all_forces & Find Orbit force if exists
	VEC3 all_forces = VEC3(0, 0, 0); //Regular forces sum
	if (pol_state != NEUTRAL && polarityForces.size() > 0) {
		TCompTransform * t = myEntity->get<TCompTransform>();
		VEC3 posPlayer = t->getPosition();
		PolarityForce nearestForce; //Orbit force (if exists)
		float lowestDist = FLT_MAX;
		for (auto forceHandle : polarityForces) {
			PolarityForce force = getPolarityForce(forceHandle);
			if (force.polarity == NEUTRAL) continue;		//if pol is neutral shouldnt have any effect

			VEC3 localForce = calcForceEffect(force);
			assert(isValid(localForce));
			all_forces += localForce;

			// The first near force discard other forces (we asume no points too close)
			if (force.distance < lowestDist) {
				nearestForce = force;
				lowestDist = force.distance;
			}
		}

		VEC3 final_forces = calcFinalForces(all_forces, nearestForce);
		polarityMoveResistance(nearestForce);

		// Stop inertia if enter with attraction
		if (pol_orbit && !pol_orbit_prev) {
			cc->ChangeSpeed(POL_SPEED_ORBITA);
		}
		// Otherwise apply inertia
		//else {
			//final_forces = (lastForces * POL_INERTIA) + ( final_forces * (1 - POL_INERTIA));
		//}

		//Apply and save forces
		cc->AddSpeed(final_forces * getDeltaTime());

		//Anular gravedad
		VEC3 antigravity = VEC3(0.f, 10.f, 0.f);
		if (nearestForce.distance < POL_RCLOSE) {
			// Near (if press button go up)
			if (io->keys[VK_SPACE].isPressed()) antigravity.y += 5.f;
		}
		else {
			// Far (Interpolate antigravity from 10 to 0)
			antigravity.y = clamp(10.f - pow(nearestForce.distance - POL_RCLOSE, 2), 0.f, 10.f);
		}
		cc->AddSpeed(antigravity * getDeltaTime());
		lastForces = final_forces;
	}

	//Last forces (util for inertia, not apllied now!)
	lastForces = all_forces;
}

VEC3 player_controller::calcForceEffect(const PolarityForce& force) {
	PROFILE_FUNCTION("player controller: attract move");

	{
		//Debug
		SetCharacterController();
		Debug->DrawLine(cc->GetPosition(), cc->GetPosition() + force.deltaPos);
	}

	//Force Effect (result)
	VEC3 forceEffect = VEC3(0, 0, 0);
	pol_orbit = false;

	//Distance and direction
	VEC3 direction = force.deltaPos;
	assert(isNormal(direction));
	direction.Normalize();
	// Si la direccion es bastante horizontal, lo acentuamos más
	if (direction.y < POL_HORIZONTALITY) {
		//direction.x *= 2; direction.z *= 2;
	}

	// Attraction?
	bool atraction = (force.polarity != pol_state);

	// In orbit space
	if (force.distance == 0) {
		if (atraction) pol_orbit = true;
		else forceEffect = POL_INTENSITY * direction;
	}
	else {
		//Regular force calc
		forceEffect = POL_INTENSITY * direction / (force.distance); //We know is not zero
		//dbg("forceEffect: %f %f %f\n", forceEffect.x, forceEffect.y, forceEffect.z);
		assert(isValid(forceEffect));
	}

	//Repulsion -> Invertimos fuerza
	if (!atraction) {
		forceEffect = -forceEffect * POL_REPULSION;
	}

	Debug->DrawLine(cc->GetPosition(), cc->GetPosition() + forceEffect, VEC3(1, 1, 0));

	return forceEffect;
}

VEC3 player_controller::calcFinalForces(const VEC3& all_forces, const PolarityForce& nearestForce) {
	//Result
	VEC3 finalForce;

	if (nearestForce.polarity != pol_state) {
		//Orbit Force
		VEC3 orbitForce;
		orbitForce.y = nearestForce.deltaPos.y * POL_OSCILE_Y;//sinf(getDeltaTime() * POL_OSCILE_Y);
		if (nearestForce.distance < POL_RCLOSE) {
			finalForce = orbitForce;

			//Not orbit force
		}
		else if (nearestForce.distance > POL_RFAR) {
			finalForce = all_forces;
		}

		//Both forces interpolation
		else {
			float deltaClose = nearestForce.distance - POL_RCLOSE;
			float range = POL_RFAR - POL_RCLOSE;
			float alfa = deltaClose / range;
			finalForce = (1 - alfa) * orbitForce + (alfa)* all_forces;
		}
	}
	else {
		finalForce = all_forces;
	}
	return finalForce;
}

void player_controller::polarityMoveResistance(const PolarityForce& force) {
	if (force.distance > 0.1f
		&& force.distance < POL_RCLOSE // very near (with margin)
		&& force.polarity != pol_state // attraction
		) {
		SetCharacterController();
		VEC3 movementPlayer = cc->GetMovement();
		VEC3 movementAtraction = VEC3(0, 0, 0);
		bool movementApplied = false;
		float moveAmoung = movementPlayer.LengthSquared();
		if (moveAmoung > 0.f) {
			//Playing trying to go away?
			if (movementPlayer.x != 0) {
				if (std::signbit(movementPlayer.x) != std::signbit(force.deltaPos.x)) {
					//movementAtraction.x = -movementPlayer.x * POL_NO_LEAVING_FORCE * nearFactor;
					movementAtraction.x = -movementPlayer.x * POL_RESISTANCE;
					movementApplied = true;
				}
				if (std::signbit(movementPlayer.z) != std::signbit(force.deltaPos.z)) {
					//movementAtraction.z = -movementPlayer.z * POL_NO_LEAVING_FORCE * nearFactor;
					movementAtraction.z = -movementPlayer.z * POL_RESISTANCE;
					movementApplied = true;
				}
			}

			//Playing getting closer?
			bool gettingCloser = false;
			if (abs(movementPlayer.x) > abs(movementPlayer.z)) {
				gettingCloser = (std::signbit(movementPlayer.x) == std::signbit(force.deltaPos.x));
			}
			else {
				gettingCloser = (std::signbit(movementPlayer.z) == std::signbit(force.deltaPos.z));
			}

			//Getting closer -> player up a little
			if (gettingCloser) {
				cc->AddMovement(VEC3(0, 1, 0), POL_ORBITA_UP_EXTRA_FORCE * max(abs(movementPlayer.z), abs(movementPlayer.x)));
			}

			if (movementApplied) {
				cc->AddMovement(movementAtraction);
			}
		}
	}
}

void player_controller::UpdateMoves()
{
	PROFILE_FUNCTION("player controller: update moves");
	SetMyEntity();
	SetCharacterController();

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	VEC3 direction = directionForward + directionLateral;
	assert(isValid(direction));
	CEntity * camera_e = camera;
	TCompTransform* camera_comp = camera_e->get<TCompTransform>();

	direction.Normalize();

	float yaw, pitch;
	camera_comp->getAngles(&yaw, &pitch);
	float new_x, new_z;

	new_x = direction.x * cosf(yaw) + direction.z*sinf(yaw);
	new_z = -direction.x * sinf(yaw) + direction.z*cosf(yaw);

	direction.x = new_x;
	if (!isValid(direction)) return; //TEST
	assert(isValid(direction));

	direction.z = new_z;
	assert(isValid(direction));

	direction.Normalize();
	assert(isValid(direction));

	float yaw_to_stop = deg2rad(80.f);
	float new_yaw = player_transform->getDeltaYawToAimDirection(direction);

	player_transform->getAngles(&yaw, &pitch);
	player_transform->setAngles(new_yaw*0.1f + yaw, pitch);		//control rotate transfom, not needed for the character controller

	if (abs(new_yaw) >= yaw_to_stop) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = VEC3(0, 0, 0);
	}

	//Set current velocity with friction
	float player_prev_speed = player_curr_speed;
	float drag = 2.5f*getDeltaTime();
	if (!cc->OnGround()) drag *= 0.33f;
	float drag_i = (1 - drag);

	if (moving) player_curr_speed = drag_i*player_curr_speed + drag*player_max_speed;
	else player_curr_speed = drag_i*player_curr_speed - drag*player_max_speed;

	if (player_curr_speed < 0) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = VEC3(0, 0, 0);
	}
	//else {
	//	float inertia = 0.7f;
	//	player_curr_speed = player_prev_speed * inertia + player_curr_speed
	//}

	//if (cc->OnGround() && player_curr_speed != 0.0f) {
	//	ChangePose(pose_run);
	//}
	//else if (player_curr_speed != 0.0f) {
	//	ChangePose(pose_jump);
	//}
	//else if (player_curr_speed == 0.0f) ChangePose(pose_idle);
	assert(isValid(direction));
	VEC3 newMovement = direction*player_curr_speed;
	cc->AddMovement(newMovement * getDeltaTime());
}

void player_controller::UpdateInputActions()
{
	PROFILE_FUNCTION("update input actions");
	SetCharacterController();
	pol_orbit = false;
	//if (isDamaged()) {
	//	pol_state = NEUTRAL;
	//}
	//else {
	if ((io->keys['1'].becomesPressed() || io->joystick.button_L.isPressed())) {
		if (pol_state == PLUS) pol_state = NEUTRAL;
		else pol_state = PLUS;
	}
	else if ((io->keys['2'].becomesPressed() || io->joystick.button_R.isPressed())) {
		if (pol_state == MINUS) pol_state = NEUTRAL;
		else pol_state = MINUS;
	}
	//}
	if (pol_state == NEUTRAL) affectPolarized = false;
	else {
		affectPolarized = (polarityForces.size() != 0);
		RecalcAttractions();
	}

	//cc->SetGravity(!pol_orbit);

	//Event onChangePolarity to LogicManager
	if (pol_state != pol_state_prev) {
		string pol_to_lua = polarize_name[pol_state];
		logic_manager->throwEvent(logic_manager->OnChangePolarity, pol_to_lua);
		pol_state_prev = pol_state;
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
	if (pol_orbit_prev != pol_orbit) {
		pol_orbit_prev = pol_orbit;
		//Logic Manager, extra behaviour/animation ...?
	}

	UpdateActionsTrigger();
}

void player_controller::UpdateActionsTrigger() {
	PROFILE_FUNCTION("player_controller: update Actions trigger");

	if (canRecEnergy) {
		//ui.addTextInstructions("\n Press 'E' to recharge energy\n");
		if (io->keys['E'].becomesPressed() || io->mouse.left.becomesPressed()) {
			rechargeEnergy();
			logic_manager->throwEvent(logic_manager->OnUseGenerator, "");
		}
		else {
			Gui->setActionAvailable(eAction::RECHARGE);
		}
	}
	else if (canPassWire) {
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

//TODO: near Stunneable, currentStunneable es LO MISMO que possessable, dejar sólo una de ellas
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

void player_controller::UpdateOverCharge() {
	if (damageFonts[Damage::ABSORB] > 0) {
		float currentLife = getLife();

		if (currentLife > evolution_limit) {
			if (io->keys['E'].becomesPressed() || io->mouse.left.becomesPressed()) {
				startOverCharge();
			}
			else {
				Gui->setActionAvailable(eAction::OVERCHARGE);
				//ui.addTextInstructions("Press ACTION to OVERCHARGE guard weapon\n");
			}
		}
	}
}

void player_controller::startOverCharge()
{
	//TODO - Estado intermedio OverCharging
	animController.setState(AST_SHOOT);
	//OverCharge Effect
	doOverCharge();
}

void player_controller::doOverCharge()
{
	VHandles guards = tags_manager.getHandlesByTag(getID("AI_guard"));
	TMsgOverCharge msg;
	logic_manager->throwEvent(logic_manager->OnOvercharge, "");
	for (auto guard : guards) {
		if (guard.isValid()) {
			CEntity* eGuard = guard;
			msg.guard_name = damage_source;
			eGuard->sendMsg(msg);
		}
	}
	Evolve(eEvol::first);
}

void player_controller::Evolve(eEvol evolution) {
	switch (evolution) {
	case eEvol::first:
		setLife(evolution_limit);
		break;
	case eEvol::second:
		TCompCharacterController *p = myEntity->get<TCompCharacterController>();
		PxController *cc = p->GetController();
		cc->resize(full_height);
		setLife(max_life);
		break;
	}
	curr_evol = evolution;
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
	//TODO
	if (!msg.range) {
		polarityForces.erase(
			std::remove(
				polarityForces.begin(),
				polarityForces.end(),
				msg.handle
			),
			polarityForces.end()
		);
		//TForcePoint fp_remove = TForcePoint(msg.origin, msg.pol);
		//force_points.erase(std::remove(force_points.begin(), force_points.end(), fp_remove), force_points.end());
	}
	else {
		polarityForces.push_back(msg.handle);
		//TForcePoint newForce = TForcePoint(msg.origin, msg.pol);
		//force_points.push_back(newForce);
	}
}

void player_controller::onSetDamage(const TMsgDamageSpecific& msg) {
	CEntity* eMe = CHandle(this).getOwner();

	assert(eMe);
	Damage::DMG_TYPE type = msg.type;

	int signDamage = msg.actived ? 1 : -1;

	//Damage Once
	float dmgOnce = DMG_ONCE(type);
	if (abs(dmgOnce) > 0.001f) {
		TMsgSetDamage msgDamageOnce;
		msgDamageOnce.dmg = dmgOnce * signDamage;
		eMe->sendMsg(msgDamageOnce);
	}

	//Update damage fonts
	if (DMG_IS_CUMULATIVE(type)) damageFonts[type] += signDamage;
	assert(damageFonts[type] >= 0); // Number fonts can't be negative

	//Cumulative add always, otherwise when change to 0 or 1
	if (DMG_IS_CUMULATIVE(type) || damageFonts[type] < 2) {
		damageCurrent += DMG_PER_SECOND(type) * signDamage;
		TMsgDamageSave msgDamagePerSecond;
		msgDamagePerSecond.modif = damageCurrent;
		eMe->sendMsg(msgDamagePerSecond);
		if (type == Damage::ABSORB) {
			//LogicManager
			if (msg.actived) {
				damage_source = msg.source;
			}
			else {
				damage_source = "none";
				TMsgDamageSave msgDamagePerSecond;
				msgDamagePerSecond.modif = 0.1f;
				eMe->sendMsg(msgDamagePerSecond);
			}
			if (damageFonts[type] > 0) {
				logic_manager->throwEvent(logic_manager->OnStartReceiveHit, "");
			}
			else {
				logic_manager->throwEvent(logic_manager->OnEndReceiveHit, "");
			}
		}
	}

	//Player is damaged (cant possess, etc.)
	____TIMER_RESET_(timerDamaged);
}

void player_controller::SendMessagePolarizeState()
{
	TMsgPlayerPolarize msg;
	msg.type = pol_state;
	VHandles hs = tags_manager.getHandlesByTag(getID("box"));
	for (CHandle h : hs) {
		if (h.isValid()) {
			h.sendMsg(msg);
		}
	}
}

//Gets
string player_controller::GetPolarity() {
	string res = "Neutral";
	switch (pol_state) {
	case PLUS:
		res = "Plus (RED)";
		break;
	case MINUS:
		res = "Minus (BLUE)";
	}
	return res;
}

void player_controller::onGetWhoAmI(TMsgGetWhoAmI& msg) {
	msg.who = PLAYER_TYPE::PLAYER;
}

//Render In Menu
void player_controller::renderInMenu() {
	char stateTxt[256];
	sprintf(stateTxt, "STATE: %s", getState().c_str());
	ImGui::Text(stateTxt);
	ImGui::Text("Editable values (polarity):\n");
	ImGui::SliderFloat("Radius1", &POL_RCLOSE, 1.f, 10.f);
	ImGui::SliderFloat("Radius2", &POL_RFAR, 10.f, 100.f);
	ImGui::SliderFloat("Intensity", &POL_INTENSITY, 100.f, 5000.f);
	ImGui::SliderFloat("Repulsion Factor", &POL_REPULSION, 0.f, 5.f);
	ImGui::SliderFloat("Inertia", &POL_INERTIA, 0.f, 0.99f);
	ImGui::SliderFloat("Speed OnEnter", &POL_SPEED_ORBITA, 0.f, 10.f);
	ImGui::SliderFloat("Force Atraction Factor in Orbita", &POL_ATRACTION_ORBITA, 0.f, 5.f);
	ImGui::SliderFloat("Extra Up Force in Orbita", &POL_ORBITA_UP_EXTRA_FORCE, 0.01f, 5.f);
	ImGui::SliderFloat("Resistence to leave", &POL_RESISTANCE, 0.f, 1.f);

	ImGui::Separator();

	ImGui::Text(GetPolarity().c_str());

	//ImGui::SliderFloat3("movement", &m_toMove.x, -1.0f, 1.0f,"%.5f");	//will be 0, cleaned each frame
}

PolarityForce player_controller::getPolarityForce(CHandle forceHandle) {
	PolarityForce pf;
	if (forceHandle.isValid()) {
		CEntity* eForce = forceHandle;
		TCompPolarized * polarized = eForce->get<TCompPolarized>();
		assert(polarized);
		pf = polarized->getForce();
	}
	return pf;
}

//Anims
void player_controller::ChangeCommonState(std::string state) {
	if (state == "moving") {
		animController.setState(AST_MOVE);
	}
	else if (state == "running") {
		animController.setState(AST_RUN);
	}
	else if (state == "jumping") {
		animController.setState(AST_JUMP);
	}
}

map<string, statehandler>* player_controller::getStatemap() {
	return &statemap;
}