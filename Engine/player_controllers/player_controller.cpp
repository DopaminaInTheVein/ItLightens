#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
#include "components/comp_msgs.h"
#include "components/comp_generator.h"
#include "components/comp_life.h"
#include "components/comp_polarized.h"
#include "render/static_mesh.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "app_modules/gui/gui.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/sound_manager/sound_manager.h"

#include "handle/handle.h"

#include "components/comp_charactercontroller.h"
#include "logic/polarity.h"

using namespace std;

#define DELTA_YAW_SELECTION		deg2rad(10)

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_PLAYER(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_PLAYER_P(state) SET_ANIM_STATE_P(animController, state)

map<string, statehandler> player_controller::statemap = {};
float player_controller::SPEED_JUMP_PENALIZE = 0.2f;

void player_controller::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		CPlayerBase::initBaseAttributes();

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

bool player_controller::getUpdateInfo()
{
	if (!CPlayerBase::getUpdateInfo()) return false;
	animController = GETH_MY(SkelControllerPlayer);

	if (!handle_pol_q.isValid()) handle_pol_q = tags_manager.getFirstHavingTag("ui_pol_q");
	if (!handle_pol_e.isValid()) handle_pol_e = tags_manager.getFirstHavingTag("ui_pol_e");
	//if (!handle_pol_q.isValid() || !handle_pol_e.isValid()) return false;

	return true;
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
	//animController.init(myEntity);

	setLife(init_life);

	ChangeState("idle");
	animController = GETH_MY(SkelControllerPlayer);
	SET_ANIM_PLAYER(AST_IDLE);
	setControllable(true);
	____TIMER__SET_ZERO_(timerDamaged);
}

bool player_controller::isDamaged() {
	PROFILE_FUNCTION("player controller: is damaged");
	return !____TIMER__END_(timerDamaged);
}

void player_controller::rechargeEnergy()
{
	PROFILE_FUNCTION("recharge_energy");
	GET_COMP(gen, generatorNear, TCompGenerator);
	float life_recover = gen->use();
	gainLife(life_recover);
	SET_ANIM_PLAYER_P(AST_RECHARGE);
	logic_manager->throwEvent(logic_manager->OnUseGenerator, GET_NAME(generatorNear));
}

void player_controller::gainLife(float amoung)
{
	float next_life = getLife();
	next_life += amoung;
	if (next_life > max_life) next_life = max_life;
	setLife(next_life);
	curr_evol = (next_life > evolution_limit) ? eEvol::second : eEvol::first;
}

//void player_controller::createEvolveLight() {
//	TCompTransform * trans = myEntity->get<TCompTransform>();
//
//	auto hm = CHandleManager::getByName("entity");
//	CHandle new_hp = hm->createHandle();
//	CEntity* entity = new_hp;
//
//	auto hm1 = CHandleManager::getByName("name");
//	CHandle new_hn = hm1->createHandle();
//	MKeyValue atts1;
//	atts1.put("name", "light_evolve");
//	new_hn.load(atts1);
//	entity->add(new_hn);
//	auto hm2 = CHandleManager::getByName("transform");
//	CHandle new_ht = hm2->createHandle();
//	MKeyValue atts2;
//	VEC3 positio = trans->getPosition(), scal = trans->getScale();
//	VEC4 rota = trans->getRotation();
//	atts2["pos"] = std::to_string(positio.x) + " " + std::to_string(positio.y + 1.0f) + " " + std::to_string(positio.z);
//	atts2["quat"] = std::to_string(rota.x) + " " + std::to_string(rota.y) + " " + std::to_string(rota.z) + " " + std::to_string(rota.w);
//	atts2["scale"] = std::to_string(scal.x) + " " + std::to_string(scal.y) + " " + std::to_string(scal.z);
//	new_ht.load(atts2);
//	entity->add(new_ht);
//	auto hm3 = CHandleManager::getByName("light_fadable");
//	CHandle new_hl = hm3->createHandle();
//	MKeyValue atts3;
//	atts3["color"] = "0 255 0 255";
//	atts3["in_radius"] = "1.0";
//	atts3["out_radius"] = "1.5";
//	atts3["ttl"] = "3.5";
//	new_hl.load(atts3);
//	entity->add(new_hl);
//	TCompLightFadable * tclp = new_hl;
//	tclp->activate();
//}
//void player_controller::createDevolveLight() {
//	TCompTransform * trans = myEntity->get<TCompTransform>();
//
//	auto hm = CHandleManager::getByName("entity");
//	CHandle new_hp = hm->createHandle();
//	CEntity* entity = new_hp;
//	IdEntities::saveIdEntity(CHandle(entity), -1900);
//
//	auto hm1 = CHandleManager::getByName("name");
//	CHandle new_hn = hm1->createHandle();
//	MKeyValue atts1;
//	atts1.put("name", "light_evolve");
//	new_hn.load(atts1);
//	entity->add(new_hn);
//	auto hm2 = CHandleManager::getByName("transform");
//	CHandle new_ht = hm2->createHandle();
//	MKeyValue atts2;
//	VEC3 positio = trans->getPosition(), scal = trans->getScale();
//	VEC4 rota = trans->getRotation();
//	atts2["pos"] = std::to_string(positio.x) + " " + std::to_string(positio.y + 1.0f) + " " + std::to_string(positio.z);
//	atts2["quat"] = std::to_string(rota.x) + " " + std::to_string(rota.y) + " " + std::to_string(rota.z) + " " + std::to_string(rota.w);
//	atts2["scale"] = std::to_string(scal.x) + " " + std::to_string(scal.y) + " " + std::to_string(scal.z);
//	new_ht.load(atts2);
//	entity->add(new_ht);
//	auto hm3 = CHandleManager::getByName("light_fadable");
//	CHandle new_hl = hm3->createHandle();
//	MKeyValue atts3;
//	atts3["color"] = "255 0 0 255";
//	atts3["in_radius"] = "1.0";
//	atts3["out_radius"] = "1.5";
//	atts3["ttl"] = "3.5";
//	new_hl.load(atts3);
//	entity->add(new_hl);
//	TCompLightFadable * tclp = new_hl;
//	tclp->activate();
//}

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
		{
			SET_ANIM_PLAYER(AST_RUN);
			sound_manager->playSound("event:/OnRaijinMoving", 0.5f, false);
		}
		else if (player_curr_speed > 0.f)
		{
			sound_manager->stopSound("event:/OnRaijinMoving");
			SET_ANIM_PLAYER(AST_MOVE);
		}
		else {
			sound_manager->stopSound("event:/OnRaijinMoving");
			SET_ANIM_PLAYER(AST_IDLE);
		}
	}
	else {
		sound_manager->stopSound("event:/OnRaijinMoving");
	}
}

void player_controller::Idle() {
	CPlayerBase::Idle();
	myExtraIdle();
}

void player_controller::myExtraIdle() {
}

void player_controller::UpdateDamage()
{
	CEntity * raijin = tags_manager.getFirstHavingTag("raijin");
	TCompLife * life = raijin->get<TCompLife>();
	if (life->energyDamageScale > 0.2f) {
		____TIMER_RESET_(timerDamaged);
	}
}

void player_controller::DoubleJump()
{
	PROFILE_FUNCTION("player controller: double jump");
	UpdateDirection();
	UpdateMovDirection();

	if (cc->GetYAxisSpeed() < 0.0f) {
		ChangeState("doublefalling");
		SET_ANIM_PLAYER(AST_FALL);
	}
}

void player_controller::DoubleFalling() {
	SET_ANIM_PLAYER(AST_FALL);
	PROFILE_FUNCTION("player controller: double falling");
	UpdateDirection();
	UpdateMovDirection();
	if (cc->OnGround()) {
		ChangeState("idle");
		logic_manager->throwEvent(logic_manager->OnJumpLand, "");
		SET_ANIM_PLAYER(AST_IDLE);
	}
}

bool player_controller::canJump() {
	if (polarityForces.size() > 0 && pol_state != NEUTRAL) return false;
	if (!controlEnabled || only_sense) return false;
	return true;
}

void player_controller::Jump()
{
	PROFILE_FUNCTION("jump player");
	bool ascending = cc->GetLastSpeed().y > 0.1f;
	VEC3 jumpVector;
	if (isMoving()) {
		forward_jump = true;
		//set false when on ground
		//-------------------------------------
		VEC3 curSpeed = cc->GetLastSpeed();
		jumpVector = VEC3(
			-curSpeed.x * 0.1f,
			clamp(jimpulse - sqrtf(curSpeed.Length())*SPEED_JUMP_PENALIZE, 0.f, jimpulse),//, //0.8f * jimpulse, jimpulse),
			//jimpulse,
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
		SET_ANIM_PLAYER(AST_JUMP2);
	}
	else {
		ChangeState("jumping");
		SET_ANIM_PLAYER(AST_JUMP);
	}
}

void player_controller::Jumping()
{
	PROFILE_FUNCTION("player controller: jumping");
	UpdateDirection();
	UpdateMovDirection();

	if (cc->GetYAxisSpeed() <= 0.0f) {
		ChangeState("falling");
		SET_ANIM_PLAYER(AST_FALL);
	}
	if (cc->OnGround() && !(cc->GetYAxisSpeed() > 0.0f)) {
		ChangeState("idle");
		SET_ANIM_PLAYER(AST_IDLE);
	}

	if ((controller->JumpButtonBecomesPressed()) && gravity_active) {
		if (gravity_active) {
			cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f), true);
			energyDecreasal(jump_energy);
			logic_manager->throwEvent(logic_manager->OnDoubleJump, "");
			ChangeState("doublejump");
			SET_ANIM_PLAYER(AST_JUMP2);
		}
	}
}

void player_controller::Falling()
{
	PROFILE_FUNCTION("player controller: falling");
	UpdateDirection();
	UpdateMovDirection();
	//Debug->LogRaw("%s\n", io->keys[VK_SPACE].becomesPressed() ? "true" : "false");

	SET_ANIM_PLAYER(AST_FALL);
	if ((controller->JumpButtonBecomesPressed()) && gravity_active) {
		cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f), true);
		energyDecreasal(jump_energy);
		logic_manager->throwEvent(logic_manager->OnDoubleJump, "");
		ChangeState("doublejump");
		SET_ANIM_PLAYER(AST_JUMP2);
	}

	if (cc->OnGround()) {
		ChangeState("idle");
		SET_ANIM_PLAYER(AST_IDLE);
	}
}

bool player_controller::affectedByPolarity()
{
	for (auto forceHandle : polarityForces) {
		PolarityForce force = getPolarityForce(forceHandle);
		if (force.polarity != NEUTRAL) return true;
	}
	return false;
}

void player_controller::RecalcAttractions()
{
	PROFILE_FUNCTION("player controller: recalc attraction");

	//gravity_active = true;

	// Calc all_forces & Find Orbit force if exists
	if (pol_state != NEUTRAL && affectedByPolarity()/*polarityForces.size() > 0*/) {
		// Remove gravity, we will control the player
		inertia_time = 0.f;
		cc->SetGravity(false);
		gravity_active = false;
		cc->ResetMovement();
		ChangeState("idle");

		for (auto forceHandle : polarityForces) {
			PolarityForce force = getPolarityForce(forceHandle);

			if (force.polarity == NEUTRAL) continue;		//if pol is neutral shouldnt have any effect
			gravity_active = false;

			VEC3 localForce = calcForceEffect(force);
			assert(isValid(localForce));
			all_forces.push_back(localForce);
			force_ponderations.push_back(1.0f);
		}

		// Calculo de la fuerza resultante
		VEC3 final_forces = calcFinalForces(all_forces, force_ponderations);

		//Apply and save forces
		cc->AddMovement(final_forces * getDeltaTime());

		inertia_force = final_forces;
	}
	else {
		cc->AddMovement(inertia_force * getDeltaTime());
		cc->SetGravity(true);
		gravity_active = true;
		if (inertia_time > POL_INERTIA_TIME) {
			inertia_force = VEC3(0, 0, 0);
		}
		else {
			inertia_time += getDeltaTime();
		}
	}
	//cc->SetGravity(gravity_active);
	all_forces.clear();
	force_ponderations.clear();
}

// Calculo de la fuerza que ejerce 1 objeto sobre el player
VEC3 player_controller::calcForceEffect(const PolarityForce& force) {
	PROFILE_FUNCTION("player controller: attract move");

	VEC3 forceEffect;
	// Attraction?
	bool atraction = (force.polarity != pol_state);

	//Distance and direction
	VEC3 direction = VEC3(0, 0, 0);

	if (atraction) {
		direction = force.deltaPos;
	}
	else {
		direction = force.deltaPos - force.offset;
	}

	assert(isNormal(direction));

	// Different scenario depending on the distance
	if (force.distance <= POL_MIN_DISTANCE) {
		if (atraction) {
			forceEffect = VEC3(0, 0, 0);
		}
		else {
			forceEffect = POL_INTENSITY * direction;
		}
	}
	else if (force.distance <= POL_MAX_DISTANCE) {
		//Regular force calc
		forceEffect = POL_INTENSITY * direction / (force.distance);

		assert(isValid(forceEffect));
	}
	else {
		forceEffect = VEC3(0, 0, 0);
	}

	//Repulsion -> Invertimos fuerza
	if (!atraction) {
		dbg("REPULSION!\n");
		forceEffect = -forceEffect * POL_REPULSION;
	}

	return forceEffect;
}

VEC3 player_controller::calcFinalForces(vector<VEC3>& forces, vector<float>& ponderations) {
	VEC3 total_force = VEC3(0, 0, 0);

	for (int i = 0; i < forces.size(); i++) {
		total_force += forces.at(i) * ponderations.at(i);
	}

	return total_force;
}

void player_controller::UpdateMoves()
{
	PROFILE_FUNCTION("player controller: update moves");
	//Check Player Enabled
	if (!controlEnabled) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = directionVertical = VEC3(0, 0, 0);
		return;
	}

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	VEC3 direction = directionForward + directionLateral + directionVertical;
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
	float yaw_to_break = deg2rad(40.f);
	float new_yaw = player_transform->getDeltaYawToAimDirection(direction);

	player_transform->getAngles(&yaw, &pitch);
	player_transform->setAngles(new_yaw * player_rotation_speed * getDeltaTime() + yaw, pitch);		//control rotate transfom, not needed for the character controller

	if (abs(new_yaw) >= yaw_to_stop) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = directionVertical = VEC3(0, 0, 0);
	}
	else if (abs(new_yaw) >= yaw_to_break) {
		player_curr_speed *= 0.9f;
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
		directionForward = directionLateral = directionVertical = VEC3(0, 0, 0);
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

//Test CLH naveshes
//---------
VEC3 startPoint;
VEC3 endPoint;
#include "logic/bt_guard.h"
#include "app_modules/io/io.h"
//---------
void player_controller::UpdateInputActions()
{
	PROFILE_FUNCTION("update input actions");

	//Test CLH navmeshes
	//----------------------
#ifndef NDEBUG
	if (io->keys['8'].becomesPressed()) {
		CHandle player = tags_manager.getFirstHavingTag("raijin");
		GET_COMP(tPlayer, player, TCompTransform);
		startPoint = tPlayer->getPosition();
		dbg("Set StartPoint\n");
	}
	if (io->keys['9'].becomesPressed()) {
		CHandle player = tags_manager.getFirstHavingTag("raijin");
		GET_COMP(tPlayer, player, TCompTransform);
		endPoint = tPlayer->getPosition();
		dbg("Set EndPoint\n");
	}
	if (io->keys['0'].becomesPressed()) {
		bt_guard bttest;
		int res = bttest.getPathDebug(startPoint, endPoint);
		if (res > 0) {
			auto wpts = bttest.getPathWpts();
			for (int i = 1; i < wpts.size(); i++) {
				if (i > 0) {
					Debug->DrawLine(wpts[i - 1] + VEC3_UP* .2f, wpts[i] + VEC3_UP*.2f, GREEN, 10.f);
				}
				i++;
			}
		}
		else {
			Debug->DrawLine(startPoint + VEC3_UP* .2f, endPoint + VEC3_UP*.2f, RED, 10.f);
		}
		dbg("get path result: %d", res);
		dbg("\n", res);
	}
	//Test vibracion camara
	//if (io->keys['5'].becomesPressed()) {
	//	logic_manager->throwEvent(logic_manager->OnStartVibration, "");
	//}
	//if (io->keys['6'].becomesPressed()) {
	//	logic_manager->throwEvent(logic_manager->OnStopVibration, "");
	//}
	//---------------------
#endif
	//if (isDamaged()) {
	//	pol_state = NEUTRAL;
	//}
	//else {
	if ((controller->IsMinusPolarityPressed())) {
		if (pol_state == PLUS) {
			setPolState(NEUTRAL);
			//pol_state = NEUTRAL;
			cc->SetGravity(true);
			gravity_active = true;
		}
		else {
			setPolState(PLUS);
			//pol_state = PLUS;
			dbg("POLARIDAD POSITIVA!\n");
		}
	}
	else if ((controller->IsPlusPolarityPressed())) {
		if (pol_state == MINUS) {
			setPolState(NEUTRAL);
			//pol_state = NEUTRAL;
			cc->SetGravity(true);
			gravity_active = true;
		}
		else {
			setPolState(MINUS);
			//pol_state = MINUS;
			dbg("POLARIDAD NEGATIVA!\n");
		}
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

	if ((controller->ActionButtonBecomesPessed()) && nearStunable()) {
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

	if (canRecharge()) {
		if (controller->ActionButtonBecomesPessed()) {
			rechargeEnergy();
		}
		else {
			Gui->setActionAvailable(eAction::RECHARGE);
		}
	}
	else if (canPassWire) {
		if (controller->ActionButtonBecomesPessed()) {
			cc->GetController()->setPosition(PhysxConversion::Vec3ToPxExVec3(endPointWire));
			logic_manager->throwEvent(logic_manager->OnUseCable, "");
		}
	}
	else if (canRechargeDrone) {
		if (controller->ActionButtonBecomesPessed()) {
			TMsgActivate msg;
			CEntity *drone_e = drone;
			drone_e->sendMsg(msg);
			logic_manager->throwEvent(logic_manager->OnRechargeDrone, "");
		}
		else {
			Gui->setActionAvailable(eAction::RECHARGE_DRONE);
		}
	}
	else if (canNotRechargeDrone) {
		if (controller->ActionButtonBecomesPessed()) {
			logic_manager->throwEvent(logic_manager->OnNotRechargeDrone, "");
		}
		else {
			Gui->setActionAvailable(eAction::RECHARGE_DRONE);
		}
	}
}

float CPlayerBase::possessionCooldown;
//Possession
void player_controller::UpdatePossession() {
	PROFILE_FUNCTION("update poss");
	recalcPossassable();
	if (currentPossessable.isValid()) {
		if (controlEnabled && (controller->PossessionButtonBecomesPressed())) {
			// Se avisa el ai_poss que ha sido pose\EDdo
			CEntity* ePoss = currentPossessable;
			TMsgAISetPossessed msg;
			msg.possessed = true;
			ePoss->sendMsg(msg);
			possessionCooldown = 1.0f;

			//Se desactiva el player
			setControllable(false);
			SBB::postBool("possMode", true);
			TMsgSetTag msgTag;
			msgTag.add = false;
			msgTag.tag = "player";
			compBaseEntity->sendMsg(msgTag);
			msgTag.add = true;

			//TODO: Desactivar render
			cc->SetActive(false);
			//cc->GetController()->setPosition(PxExtendedVec3(0, 200, 0));
			//TCompTransform *t = myEntity->get<TCompTransform>();
			//t->setPosition(VEC3(0, 200, 0));
			player_curr_speed = 0;

			logic_manager->throwEvent(logic_manager->OnPossess, ePoss->getName());
		}
	}
}

// Recalcula el mejor candidato para poseer
void player_controller::recalcPossassable() {
	PROFILE_FUNCTION("recalc possessable");
	currentPossessable = CHandle();
	if (isDamaged()) return;
	float minDeltaYaw = FLT_MAX;
	float minDistance = FLT_MAX;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
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

//TODO: near Stunneable, currentStunneable es LO MISMO que possessable, dejar s\F3lo una de ellas
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
	getUpdateInfo();
	// Handles y entities necesarias
	CHandle  hMe = CHandle(this).getOwner();
	CEntity* eMe = hMe;
	CHandle hCamera = tags_manager.getFirstHavingTag(getID("camera_main"));
	CEntity* eCamera = hCamera;

	//Colocamos el player
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
	setControllable(true);

	//Notificamos presencia de Player
	SBB::postBool("possMode", false);
}

void player_controller::UpdateOverCharge() {
	if (damageFonts[Damage::ABSORB] > 0) {
		float currentLife = getLife();

		if (currentLife > evolution_limit) {
			if (controller->ActionButtonBecomesPessed()) {
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
	SET_ANIM_PLAYER(AST_SHOOT);
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
			msg.guard_names = damage_source;
			eGuard->sendMsg(msg);
		}
	}
	damage_source.clear();
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

void player_controller::onWirePass(const TMsgWirePass & msg)
{
	canPassWire = msg.range;
	endPointWire = msg.dst;
}

void player_controller::onCanRec(const TMsgCanRec & msg)
{
	generatorNear = msg.generator;
}

bool player_controller::canRecharge()
{
	if (isDamaged()) return false;
	if (generatorNear.isValid()) {
		GET_COMP(gen, generatorNear, TCompGenerator);
		if (gen) return gen->isUsable();
	}
	return false;
}

void player_controller::onCanRechargeDrone(const TMsgCanRechargeDrone & msg)
{
	canRechargeDrone = msg.range;
	drone = msg.han;
}
void player_controller::onCanNotRechargeDrone(const TMsgCanNotRechargeDrone & msg)
{
	canNotRechargeDrone = msg.range;
	drone = msg.han;
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
	CEntity* eMe = tags_manager.getFirstHavingTag("raijin");

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
		float dmg = DMG_PER_SECOND(type) * signDamage;
		damageCurrent += dmg;
		damageAdded += dmg;
		TMsgDamage msgDamagePerSecond;
		msgDamagePerSecond.modif = damageCurrent;
		eMe->sendMsg(msgDamagePerSecond);
		if (type == Damage::ABSORB) {
			if (msg.actived) {
				damage_source.push_back(msg.source);
			}
			//LogicManager
			if (damageFonts[type] > 0) {
				//TMsgDamageSave msgDamagePerSecond;
				//msgDamagePerSecond.modif = 0.1f;
				//eMe->sendMsg(msgDamagePerSecond);
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

void player_controller::onDifficultyChanged(const TMsgDifficultyChanged& msg)
{
	damageCurrent -= damageAdded;
	damageAdded = 0;
	for (int i = 0; i < Damage::SIZE; i++) {
		damageCurrent += DMG_PER_SECOND(i) * damageFonts[i];
		damageAdded += DMG_PER_SECOND(i) * damageFonts[i];
	}
	TMsgDamage msgDamagePerSecond;
	msgDamagePerSecond.modif = damageCurrent;
	MY_OWNER.sendMsg(msgDamagePerSecond);
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
	msg.who_string = "Player";
}

// Set pol state
void player_controller::setPolState(pols new_pol)
{
	pol_state = new_pol;
	GET_COMP(ui_pol_q, handle_pol_q, TCompGui);
	GET_COMP(ui_pol_e, handle_pol_e, TCompGui);
	float pol_q_val = new_pol == NEUTRAL ? 0.f : new_pol == MINUS ? 1.f : 0.f;
	float pol_e_val = new_pol == NEUTRAL ? 0.f : 1.f - pol_q_val;
	ui_pol_q->setRenderTarget(pol_q_val, 5.f);
	ui_pol_e->setRenderTarget(pol_e_val, 5.f);
}

//Render In Menu
void player_controller::renderInMenu() {
	char stateTxt[256];
	sprintf(stateTxt, "STATE: %s", getState().c_str());
	ImGui::Text(stateTxt);
	ImGui::Text("Editable values (polarity):\n");
	ImGui::SliderFloat("Radius1", &POL_MIN_DISTANCE, 1.f, 10.f);
	ImGui::SliderFloat("Radius2", &POL_MAX_DISTANCE, 10.f, 100.f);
	ImGui::SliderFloat("Intensity", &POL_INTENSITY, 100.f, 5000.f);
	ImGui::SliderFloat("Repulsion Factor", &POL_REPULSION, 0.f, 5.f);

	ImGui::Separator();

	ImGui::Text(GetPolarity().c_str());
	for (auto force : all_forces) {
		ImGui::Text("force: %f - %f - %f\n", force.x, force.y, force.z);
	}

	for (auto pond : force_ponderations) {
		ImGui::Text("Ponderation: %f\n", pond);
	}

	ImGui::Separator();
	ImGui::Text("Absorb fonts: %d", damageFonts[Damage::ABSORB]);
	ImGui::Separator();
	if (cc) IMGUI_SHOW_FLOAT(cc->GetLastSpeed().Length());
	ImGui::DragFloat("Jump speed penalize", &SPEED_JUMP_PENALIZE, 0.01f, 0.f, 1.f);

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
		SET_ANIM_PLAYER(AST_MOVE);
	}
	else if (state == "running") {
		SET_ANIM_PLAYER(AST_RUN);
	}
	else if (state == "jumping") {
		SET_ANIM_PLAYER(AST_JUMP);
	}
	else if (state == "idle") {
		SET_ANIM_PLAYER(AST_IDLE);
	}
	else if (state == "dead") {
		SET_ANIM_PLAYER_P(AST_DEATH);
	}
}

map<string, statehandler>* player_controller::getStatemap() {
	return &statemap;
}