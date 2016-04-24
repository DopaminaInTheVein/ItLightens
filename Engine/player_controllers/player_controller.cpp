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

	pose_run = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_jump = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_idle = getHandleManager<TCompRenderStaticMesh>()->createHandle();

	pose_no_ev = myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	actual_render = pose_no_ev;

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

	lastForces = VEC3(0, 0, 0);

	actual_render->registerToRender();

	ChangeState("idle");
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
void player_controller::createEvolveLight() {
	/*
	Need to create light:
	<entity>
	<name name="light_green" />
	<transform pos="player_pos" quat="player_rot" scale="player_scale" />
	<light_point color="0 0 1 0.7" in_radius="2" out_radius="3" />
	</entity>
	*/
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
	atts2["pos"] = std::to_string(positio.x) + " " + std::to_string(positio.y + 1.5f) + " " + std::to_string(positio.z);
	atts2["quat"] = std::to_string(rota.x) + " " + std::to_string(rota.y) + " " + std::to_string(rota.z) + " " + std::to_string(rota.w);
	atts2["scale"] = std::to_string(scal.x) + " " + std::to_string(scal.y) + " " + std::to_string(scal.z);
	new_ht.load(atts2);
	entity->add(new_ht);
	auto hm3 = CHandleManager::getByName("light_point");
	CHandle new_hl = hm3->createHandle();
	MKeyValue atts3;
	atts3["color"] = "0 0 1 0.7";
	atts3["in_radius"] = "2";
	atts3["out_radius"] = "4";
	atts3["ttl"] = "2.5";
	new_hl.load(atts3);
	entity->add(new_hl);
	TCompLightPoint * tclp = new_hl;
	tclp->activate();
}
void player_controller::createDevolveLight() {
	/*
	Need to create light:
	<entity>
	<name name="light_green" />
	<transform pos="player_pos" quat="player_rot" scale="player_scale" />
	<light_point color="0 0 1 0.7" in_radius="2" out_radius="3" />
	</entity>
	*/
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
	atts2["pos"] = std::to_string(positio.x) + " " + std::to_string(positio.y + 1.5f) + " " + std::to_string(positio.z);
	atts2["quat"] = std::to_string(rota.x) + " " + std::to_string(rota.y) + " " + std::to_string(rota.z) + " " + std::to_string(rota.w);
	atts2["scale"] = std::to_string(scal.x) + " " + std::to_string(scal.y) + " " + std::to_string(scal.z);
	new_ht.load(atts2);
	entity->add(new_ht);
	auto hm3 = CHandleManager::getByName("light_point");
	CHandle new_hl = hm3->createHandle();
	MKeyValue atts3;
	atts3["color"] = "1 0 0 0.7";
	atts3["in_radius"] = "2";
	atts3["out_radius"] = "4";
	atts3["ttl"] = "2.5";
	new_hl.load(atts3);
	entity->add(new_hl);
	TCompLightPoint * tclp = new_hl;
	tclp->activate();
}

void player_controller::myUpdate() {
	PROFILE_FUNCTION("player controller: MY_update");
	SetMyEntity();

	TCompTransform *m = myEntity->get<TCompTransform>();

	____TIMER__UPDATE_(timerDamaged);
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 pos = player_transform->getPosition();
	if (isDamaged()) {
		UpdateOverCharge();
	}
	else {
		UpdatePossession();
	}
}

void player_controller::Idle() {
	CPlayerBase::Idle();
	myExtraIdle();
}

void player_controller::myExtraIdle() {
	if (pol_state != NEUTRAL) {
		ChangeState("falling");
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

	if (cc->OnGround() && !(cc->GetYAxisSpeed() > 0.0f)) {
		ChangeState("idle");
	}

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f));
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
		cc->AddImpulse(VEC3(0.0f, jimpulse, 0.0f));
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
	VEC3 forces = VEC3(0, 0, 0);
	VEC3 orbitPoint;
	SetCharacterController();
	TCompTransform * t = myEntity->get<TCompTransform>();
	VEC3 posPlayer = t->getPosition();
	float lowestDist = FLT_MAX;

	if (pol_state != NEUTRAL) {
		for (auto force : force_points) {		//sum of all forces
			if (force.pol == NEUTRAL) continue;		//if pol is neutral shouldnt have any effect

			VEC3 localForce = PolarityForce(force.point, pol_state == force.pol);
			// The first near force discard other forces (we asume no points too close)
			if (pol_orbit) {
				float dist = simpleDist(posPlayer, force.point);
				if (dist < lowestDist) {
					forces = localForce;
					orbitPoint = force.point;
					lowestDist = dist;
				}
			}
			else forces += localForce;
		}
	}

	//Check if player is orbiting
	if (pol_orbit) {
		float dist = realDist(posPlayer, orbitPoint);
		float nearFactor = (1.f - (dist / POL_RADIUS_STRONG));
		VEC3 forceReal = forces;
		forces = VEC3(
			1 / forces.x,		//x
			forces.y * 0.05f,	//y
			1 / forces.z);		//z

		forces *= POL_ATRACTION_ORBITA;

		if (!pol_orbit_prev) {
			//cc->ResetMovement();
			cc->ChangeSpeed(POL_SPEED_ORBITA);
		}
		else {
			//Player moving?
			forceReal.Normalize();
			VEC3 movementPlayer = cc->GetMovement();
			VEC3 movementAtraction = VEC3(0, 0, 0);
			bool movementApplied = false;
			float moveAmoung = movementPlayer.LengthSquared();
			if (moveAmoung > 0.f) {
				//Playing trying to go away?
				if (movementPlayer.x != 0) {
					if (std::signbit(movementPlayer.x) != std::signbit(forceReal.x)) {
						movementAtraction.x = -movementPlayer.x * POL_NO_LEAVING_FORCE * nearFactor;
						movementApplied = true;
					}
					if (std::signbit(movementPlayer.z) != std::signbit(forceReal.z)) {
						movementAtraction.z = -movementPlayer.z * POL_NO_LEAVING_FORCE * nearFactor;
						movementApplied = true;
					}
				}
				//Playing getting closer?
				bool gettingCloser = false;
				if (abs(movementPlayer.x) > abs(movementPlayer.z)) {
					gettingCloser = (std::signbit(movementPlayer.x) == std::signbit(forceReal.x));
				}
				else {
					gettingCloser = (std::signbit(movementPlayer.z) == std::signbit(forceReal.z));
				}

				//Getting closer -> player up a little
				if (gettingCloser) {
					forces.y += POL_ORBITA_UP_EXTRA_FORCE;
				}

				if (movementApplied) {
					cc->AddMovement(movementAtraction);
				}
			}
		}
	}
	else {
		forces = (lastForces * POL_INERTIA) + (forces * (1 - POL_INERTIA));
	}

	//Smooth forces
	//forces = (lastForces * 0.95f) + (forces * 0.05f);

	//if (forces.y > 0) forces.y += forces.y;

	cc->AddSpeed(forces*getDeltaTime());
	lastForces = forces;
	//cc->AddImpulse(forces);
}

VEC3 player_controller::PolarityForce(VEC3 point_pos, bool atraction) {
	PROFILE_FUNCTION("player controller: attract move");

	//Esto deberian ser ctes o variables del punto de magnetismo
	float distMax = POL_RADIUS;
	float distOrbit = POL_RADIUS_STRONG;

	SetMyEntity();
	//point_pos.y += 0.5f;
	TCompCharacterController* cc = myEntity->get<TCompCharacterController>();
	VEC3 player_position = cc->GetPosition();
	float dist = realDist(player_position, point_pos);

	VEC3 force = VEC3(0, 0, 0);

	// Suficiente cerca?
	if (dist < distMax) {
		VEC3 direction = point_pos - player_position;
		// Si la direccion es bastante horizontal, lo acentuamos más
		if (direction.y < POL_HORIZONTALITY) {
			direction.x *= 2;
			direction.z *= 2;
		}
		direction.Normalize();
		force = POL_INTENSITY * direction / ((dist)+0.01f);
	}

	// Atraccion -> Si cerca, orbitar
	if (atraction) {
		pol_orbit = pol_orbit || (dist < distOrbit);
	}
	//Repulsion -> Invertimos fuerza
	else {
		force = -force * POL_REPULSION;
	}

	return force;
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
	else if (player_curr_speed != 0.0f) {
		ChangePose(pose_jump);
	}
	else if (player_curr_speed == 0.0f) ChangePose(pose_idle);

	if (cc->OnGround())
		cc->AddMovement(direction*player_curr_speed*getDeltaTime());
	else {
		cc->AddMovement(direction*player_curr_speed*getDeltaTime() / 2);
	}
}

void player_controller::UpdateInputActions()
{
	PROFILE_FUNCTION("update input actions");
	SetCharacterController();
	pol_orbit = false;
	if (isDamaged()) {
		pol_state = NEUTRAL;
	}
	else {
		if ((io->keys['1'].becomesPressed() || io->joystick.button_L.isPressed())) {
			if (pol_state == PLUS) pol_state = NEUTRAL;
			else pol_state = PLUS;
		}
		else if ((io->keys['2'].becomesPressed() || io->joystick.button_R.isPressed())) {
			if (pol_state == MINUS) pol_state = NEUTRAL;
			else pol_state = MINUS;
		}
	}

	if (pol_state == NEUTRAL) affectPolarized = false;
	else {
		affectPolarized = (force_points.size() != 0);
		RecalcAttractions();
	}

	cc->SetGravity(!pol_orbit);

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
		ui.addTextInstructions("\n Press 'E' to recharge energy\n");
		if (io->keys['E'].becomesPressed()) {
			rechargeEnergy();
			curr_evol = 1;
			logic_manager->throwEvent(logic_manager->OnUseGenerator, "");
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
			if (io->keys[VK_LSHIFT].becomesPressed()) {
				startOverCharge();
			}
			else {
				ui.addTextInstructions("Press 'L-SHIFT' to OVERCHARGE guard weapon\n");
			}
		}
	}
}

void player_controller::startOverCharge()
{
	//TODO - Estado intermedio OverCharging

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
	if (evolution == eEvol::first) {
		//Set Life
		setLife(evolution_limit);
	}
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
	}
	else {
		TForcePoint newForce = TForcePoint(msg.origin, msg.pol);
		force_points.push_back(newForce);
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

//Render In Menu
void player_controller::renderInMenu() {
	char stateTxt[256];
	sprintf(stateTxt, "STATE: %s", getState().c_str());
	ImGui::Text(stateTxt);
	ImGui::Text("Editable values (polarity):\n");
	ImGui::SliderFloat("Radius1", &POL_RADIUS, 1.f, 10.f);
	ImGui::SliderFloat("Radius2", &POL_RADIUS_STRONG, 1.f, 10.f);
	ImGui::SliderFloat("Horiz. Repulsion", &POL_HORIZONTALITY, 0.f, 1.f);
	ImGui::SliderFloat("Intensity", &POL_INTENSITY, 1.f, 500.f);
	ImGui::SliderFloat("Repulsion Factor", &POL_REPULSION, 0.f, 5.f);
	ImGui::SliderFloat("Inertia", &POL_INERTIA, 0.f, 0.99f);
	ImGui::SliderFloat("Speed OnEnter", &POL_SPEED_ORBITA, 0.f, 10.f);
	ImGui::SliderFloat("Force Atraction Factor in Orbita", &POL_ATRACTION_ORBITA, 0.f, 5.f);
	ImGui::SliderFloat("Factor allowing leave", &POL_NO_LEAVING_FORCE, 0.f, 1.5f);
	ImGui::SliderFloat("Extra Up Force in Orbita", &POL_ORBITA_UP_EXTRA_FORCE, 0.01f, 5.f);
	ImGui::SliderFloat("Extra Up Force in Orbita", &POL_REAL_FORCE_Y_ORBITA, 0.01f, 1.f);

	//ImGui::SliderFloat3("movement", &m_toMove.x, -1.0f, 1.0f,"%.5f");	//will be 0, cleaned each frame
}

map<string, statehandler>* player_controller::getStatemap() {
	return &statemap;
}