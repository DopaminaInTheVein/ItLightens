#include "mcv_platform.h"
#include <windows.h>
#include "bt_guard.h"
#include "components/entity_tags.h"
#include "utils/XMLParser.h"
#include "utils/utils.h"
#include "logic/sbb.h"
#include "app_modules\io\io.h"
#include "app_modules\logic_manager\logic_manager.h"
#include "ui\ui_interface.h"

map<string, bt_guard::KptType> bt_guard::kptTypes = {
	  {"seek", KptType::Seek}
	, {"look", KptType::Look}
};

map<string, btnode *> bt_guard::tree = {};
map<string, btaction> bt_guard::actions = {};
map<string, btcondition> bt_guard::conditions = {};
map<string, btevent> bt_guard::events = {};
btnode* bt_guard::root = nullptr;

TCompTransform * bt_guard::getTransform() {
	PROFILE_FUNCTION("guard: get transform");
	CEntity * e = myParent;
	if (!e) return nullptr;
	TCompTransform * t = e->get<TCompTransform>();
	return t;
}

TCompCharacterController* bt_guard::getCC() {
	PROFILE_FUNCTION("guard: get cc");
	CEntity * e = myParent;
	if (!e) return nullptr;
	TCompCharacterController * cc = e->get<TCompCharacterController>();
	return cc;
}

CEntity* bt_guard::getPlayer() {
	PROFILE_FUNCTION("guard: get player");
	VHandles targets = tags_manager.getHandlesByTag(getID("player"));
	thePlayer = targets[targets.size() - 1];
	CEntity* player = thePlayer;
	return player;
}

void bt_guard::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_guard")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields = readIniAtrData(file_ini, "bt_guard");

			assignValueToVar(DIST_SQ_REACH_PNT, fields);
			assignValueToVar(DIST_SQ_SHOT_AREA_ENTER, fields);
			assignValueToVar(DIST_SQ_SHOT_AREA_LEAVE, fields);
			assignValueToVar(DIST_RAYSHOT, fields);
			assignValueToVar(DIST_SQ_PLAYER_DETECTION, fields);
			assignValueToVar(DIST_SQ_PLAYER_LOST, fields);
			assignValueToVar(SPEED_WALK, fields);
			assignValueToVar(CONE_VISION, fields);
			CONE_VISION = deg2rad(CONE_VISION);
			assignValueToVar(SPEED_ROT, fields);
			SPEED_ROT = deg2rad(SPEED_ROT);
			assignValueToVar(DAMAGE_LASER, fields);
			assignValueToVar(MAX_REACTION_TIME, fields);
			assignValueToVar(reduce_factor, fields);
			assignValueToVar(t_reduceStats_max, fields);
			assignValueToVar(t_reduceStats, fields);
		}
	}
}

/**************
* Init
**************/
void bt_guard::Init()
{
	// read main attributes from file
	readIniFileAttr();

	//Handles
	myHandle = CHandle(this);
	myParent = myHandle.getOwner();
	thePlayer = tags_manager.getFirstHavingTag(getID("player"));

	if (tree.empty()) {
		// insert all states in the map
		createRoot("guard", PRIORITY, NULL, NULL);
		addChild("guard", "stunned", ACTION, (btcondition)&bt_guard::playerStunned, (btaction)&bt_guard::actionStunned);
		//addChild("guard", "attack_decorator", DECORATOR, (btcondition)&bt_guard::playerDetected, (btaction)&bt_guard::actionReact);
		//addChild("attack_decorator", "attack", PRIORITY, NULL, NULL);
		addChild("guard", "attack", PRIORITY, (btcondition)&bt_guard::playerDetected, NULL);
		addChild("attack", "chase", ACTION, (btcondition)&bt_guard::playerOutOfReach, (btaction)&bt_guard::actionChase);
		addChild("attack", "absorbsequence", SEQUENCE, NULL, NULL);
		addChild("absorbsequence", "absorb", ACTION, NULL, (btaction)&bt_guard::actionAbsorb);
		addChild("absorbsequence", "shootwall", ACTION, NULL, (btaction)&bt_guard::actionShootWall);
		addChild("guard", "alertdetected", SEQUENCE, (btcondition)&bt_guard::guardAlerted, NULL);
		addChild("alertdetected", "search", ACTION, NULL, (btaction)&bt_guard::actionSearch);
		addChild("alertdetected", "lookaround", ACTION, NULL, (btaction)&bt_guard::actionLookAround);
		addChild("guard", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "nextWpt", ACTION, NULL, (btaction)&bt_guard::actionNextWpt);
		addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_guard::actionSeekWpt);
		addChild("patrol", "waitwpt", ACTION, NULL, (btaction)&bt_guard::actionWaitWpt);
	}

	curkpt = 0;

	//Other info
	____TIMER_REDEFINE_(timerShootingWall, 1);
	____TIMER_REDEFINE_(timerStunt, 15);
	timeWaiting = 0;
	deltaYawLookingArround = 0;
	stunned = false;

	//Mallas
	CEntity* myEntity = myParent;

	mesh = myEntity->get<TCompRenderStaticMesh>();

	pose_idle_route = "static_meshes/guard.static_mesh";
	pose_shoot_route = "static_meshes/guard_shoot.static_mesh";
	pose_run_route = "static_meshes/guard_run.static_mesh";
}

//conditions
bool bt_guard::playerStunned() {
	PROFILE_FUNCTION("guard: player stunned");
	return stunned == true;
}

bool bt_guard::playerDetected() {
	PROFILE_FUNCTION("guard: player detected");
	return playerVisible();
}

bool bt_guard::playerOutOfReach() {
	PROFILE_FUNCTION("guard: player out of reach");
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);
	if (distance > DIST_SQ_SHOT_AREA_ENTER) {
		ChangePose(pose_run_route);
		return true;
	}
	else {
		ChangePose(pose_shoot_route);
		return false;
	}
}

bool bt_guard::guardAlerted() {
	PROFILE_FUNCTION("guard: guardalert");
	if (playerLost || noiseHeard) {
		return true;
	}
	return false;
}

//actions
int bt_guard::actionStunned() {
	PROFILE_FUNCTION("guard: actionstunned");
	if (!myParent.isValid()) return false;
	if (timerStunt < 0) {
		stunned = false;
		return OK;
	}
	else {
		if (timerStunt > -1)
			timerStunt -= getDeltaTime();
		return STAY;
	}
}

int bt_guard::actionReact() {
	PROFILE_FUNCTION("guard: actionreact");
	if (!myParent.isValid()) return false;
	
	if (!player_detected_start) {
		// starting the reaction time decorator
		player_detected_start = true;
		reaction_time = rand() % (int)MAX_REACTION_TIME;
		// calling OnGuardAttackEvent
		logic_manager->throwEvent(logic_manager->OnGuardAttack, "");
	}

	// stay in this state until the reaction time is over
	if (reaction_time < 0.f) {
		player_detected_start = false;
		return OK;
	}
	else {
		if (reaction_time > -1)
			reaction_time -= getDeltaTime();
		return STAY;
	}

}

int bt_guard::actionChase() {
	PROFILE_FUNCTION("guard: chase");
	if (!myParent.isValid()) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);
	//player lost?
	if (distance > DIST_SQ_PLAYER_LOST || outJurisdiction(posPlayer)) {
		playerLost = true;
		ChangePose(pose_idle_route);
		return OK;
	}
	//player near?
	else if (distance < DIST_SQ_SHOT_AREA_ENTER) {
		ChangePose(pose_shoot_route);
		return OK;
	}
	else {
		getPath(myPos, posPlayer, "sala1");

		ChangePose(pose_run_route);
		goTo(posPlayer);
		return STAY;
	}
}

int bt_guard::actionAbsorb() {
	PROFILE_FUNCTION("guard: absorb");
	if (!myParent.isValid()) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float dist = squaredDistXZ(posPlayer, getTransform()->getPosition());

	ui.addTextInstructions("\nPress 'M' to interrupt gaurd shoot when he dont see you!!! (artificial)\n");
	if (io->keys['M'].becomesPressed()) {
		artificialInterrupt();
	}

	turnTo(posPlayer);
	if (squaredDistY(myPos, posPlayer) * 2 > dist) { //Angulo de 30 grados
														//Si pitch muy alto me alejo
		goForward(-SPEED_WALK);
	}
	if (!playerVisible()) {
		logic_manager->throwEvent(logic_manager->OnInterruptHit, "");
		return OK;
	}
	else {
		ChangePose(pose_shoot_route);
		shootToPlayer();
		return STAY;
	}
}

int bt_guard::actionShootWall() {
	PROFILE_FUNCTION("guard: shootwall");
	if (!myParent.isValid()) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	turnTo(posPlayer);
	if (playerVisible()) {
		return OK;
	}
	else {
		shootToPlayer();
		if (timerShootingWall < 0) {
			return OK;
		}
		else {
			if (timerShootingWall > -1)
				timerShootingWall -= getDeltaTime();
			return STAY;
		}
	}
}

int bt_guard::actionSearch() {
	PROFILE_FUNCTION("guard: search");
	if (!myParent.isValid()) return false;
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, noisePoint);

	//Player Visible?
	if (playerVisible()) {
		setCurrent(NULL);
	}
	// If we heared a noise, we go to the point and look around
	else if (noiseHeard) {
		//Noise Point Reached ?
		if (distance < DIST_SQ_REACH_PNT) {
			noiseHeard = false;
			return OK;
		}
		else {
			getPath(myPos, noisePoint, "sala1");

			goTo(noisePoint);
			return STAY;
		}
	}
	// If player was lost, we simply look around
	else
		return OK;
}
int bt_guard::actionLookAround() {
	PROFILE_FUNCTION("guard: lookaround");
	if (!myParent.isValid()) return false;
	//Player Visible?
	if (playerVisible()) {
		setCurrent(NULL);
	}
	//Turn arround
	else if (deltaYawLookingArround < 2 * M_PI) {
		ChangePose(pose_idle_route);
		float yaw, pitch;
		getTransform()->getAngles(&yaw, &pitch);

		float deltaYaw = SPEED_ROT * getDeltaTime();
		deltaYawLookingArround += deltaYaw;
		yaw += deltaYaw;
		getTransform()->setAngles(yaw, pitch);
		return STAY;
	}
	else {
		deltaYawLookingArround = 0;
		return OK;
	}
}

int bt_guard::actionSeekWpt() {
	PROFILE_FUNCTION("guard: actionseekwpt");
	if (!myParent.isValid()) return 0;
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = keyPoints[curkpt].pos;
	//Player Visible?
	if (playerVisible()) {
		setCurrent(NULL);
	}
	//Go to waypoint
	else if (keyPoints[curkpt].type == Seek) {
		//reach waypoint?
		if (squaredDistXZ(myPos, dest) < DIST_SQ_REACH_PNT) {
			curkpt = (curkpt + 1) % keyPoints.size();
			return OK;
		}
		else {
			getPath(myPos, dest, "sala1");
			ChangePose(pose_run_route);
			goTo(dest);
			return STAY;
		}
	}
	//Look to waypoint
	else if (keyPoints[curkpt].type == Look) {
		ChangePose(pose_idle_route);
		//Look to waypoint
		if (turnTo(dest)) {
			curkpt = (curkpt + 1) % keyPoints.size();
			return OK;
		}
		else {
			return STAY;
		}
	}

	return OK;
}

int bt_guard::actionNextWpt() {
	PROFILE_FUNCTION("guard: actionnextwpt");
	if (!myParent.isValid()) return false;
	VEC3 dest = keyPoints[curkpt].pos;

	//Player Visible?
	if (playerVisible()) {
		setCurrent(NULL);
	}
	//Look to waypoint
	else if (turnTo(dest)) {
		return OK;
	}
	else {
		return STAY;
	}
}

int bt_guard::actionWaitWpt() {
	//PROFILE_FUNCTION("guard: actionwaitwpt");
	if (!myParent.isValid()) return false;
	ChangePose(pose_idle_route);

	//player visible?
	if (playerVisible()) {
		setCurrent(NULL);
	}
	else if (timeWaiting > keyPoints[curkpt].time) {
		timeWaiting = 0;
		return OK;
	}
	else {
		timeWaiting += getDeltaTime();
		return STAY;
	}
}

/**************
* Mensajes
**************/
void bt_guard::noise(const TMsgNoise& msg) {
	PROFILE_FUNCTION("guard: noise");
	if (outJurisdiction(msg.source)) return;
	if (playerVisible()) return;
	if (stunned) return;
	if (canHear(msg.source, msg.intensity)) {
		resetTimers();
		noisePoint = msg.source;
		noiseHeard = true;
	}
}

void bt_guard::onMagneticBomb(const TMsgMagneticBomb & msg)
{
	PROFILE_FUNCTION("guard: onmagneticbomb");
	VEC3 myPos = getTransform()->getPosition();
	float d = squaredDist(msg.pos, myPos);

	if (d < msg.r) {
		reduceStats();
		t_reduceStats = t_reduceStats_max;
	}
}

void bt_guard::onStaticBomb(const TMsgStaticBomb& msg) {
	PROFILE_FUNCTION("guard: onstaticbomb");
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	if (squaredDist(msg.pos, posPlayer) < msg.r * msg.r) {
		resetTimers();
		stunned = true;
		setCurrent(NULL);
	}
}

/**************
 * Auxiliares
 **************/

 // -- Go To -- //
bool bt_guard::canHear(VEC3 position, float intensity) {
	return (squaredDistXZ(getTransform()->getPosition(), position) < DIST_SQ_SOUND_DETECTION);
}

// -- Go To -- //
void bt_guard::goTo(const VEC3& dest) {
	PROFILE_FUNCTION("guard: go to");
	if (!SBB::readBool("sala1")) {
		return;
	}
	VEC3 target = dest;
	VEC3 npcPos = getTransform()->getPosition();
	while (totalPathWpt > 0 && currPathWpt < totalPathWpt && fabsf(squaredDistXZ(pathWpts[currPathWpt], npcPos)) < 0.5f) {
		++currPathWpt;
	}
	if (currPathWpt < totalPathWpt) {
		target = pathWpts[currPathWpt];
	}
	VEC3 npcFront = getTransform()->getFront();
	if (needsSteering(npcPos + npcFront, getTransform(), SPEED_WALK, myParent, "sala1")) {
		goForward(SPEED_WALK);
	}
	else if (!getTransform()->isHalfConeVision(target, deg2rad(5.0f))) {
		turnTo(target);
	}
	else {
		float distToWPT = squaredDistXZ(target, getTransform()->getPosition());
		if (fabsf(distToWPT) > 0.5f && currPathWpt < totalPathWpt || fabsf(distToWPT) > 6.0f) {
			goForward(SPEED_WALK);
		}
	}
}

// -- Go Forward -- //
void bt_guard::goForward(float stepForward) {
	PROFILE_FUNCTION("guard: go forward");
	VEC3 myPos = getTransform()->getPosition();
	float dt = getDeltaTime();
	getCC()->AddMovement(getTransform()->getFront() * stepForward*dt);
}

// -- Turn To -- //
bool bt_guard::turnTo(VEC3 dest) {
	PROFILE_FUNCTION("guard: turn to");
	if (!myParent.isValid()) return false;
	VEC3 myPos = getTransform()->getPosition();
	float yaw, pitch;
	getTransform()->getAngles(&yaw, &pitch);

	float deltaAngle = SPEED_ROT * getDeltaTime();
	float deltaYaw = getTransform()->getDeltaYawToAimTo(dest);

	if (deltaYaw > 0) {
		if (deltaAngle < deltaYaw) yaw += deltaAngle;
		else yaw += deltaYaw;
	}
	else {
		if (deltaAngle < abs(deltaYaw)) yaw -= deltaAngle;
		else yaw += deltaYaw;
	}

	if (!getTransform()->isHalfConeVision(dest, deg2rad(deltaAngle))) {
		bool inLeft = getTransform()->isInLeft(dest);
		if (inLeft) {
			yaw += deltaAngle;
		}
		else {
			yaw -= deltaAngle;
		}
		getTransform()->setAngles(yaw, pitch);
	}

	//Ha acabado el giro?
	return abs(deltaYaw) < deltaAngle;
}

// -- Player Visible? -- //
bool bt_guard::playerVisible() {
	if (!myParent.isValid()) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	if (SBB::readBool("possMode") && squaredDistXZ(myPos, posPlayer) > 25.f) {
		return false;
	}
	if (squaredDistY(posPlayer, myPos) < squaredDistXZ(posPlayer, myPos) * 2) { //Pitch < 30
		if (getTransform()->isHalfConeVision(posPlayer, CONE_VISION)) { //Cono vision
			if (squaredDistXZ(myPos, posPlayer) < DIST_SQ_PLAYER_DETECTION) { //Distancia
				if (inJurisdiction(posPlayer)) { //Jurisdiccion
					float distanceJur = squaredDistXZ(posPlayer, jurCenter);
					float distRay;
					if (SBB::readBool("possMode")) {
						// Estas poseyendo, estas cerca y dentro del cono de vision, no hace falta raycast
						return true;
					}
					else {
						//TODO RAYCAST PLAYER
						PxRaycastBuffer hit;
						bool ret = rayCastToPlayer(1, distRay, hit);
						if (ret) { //No bloquea vision
							CHandle h = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
							if (h.hasTag("player")) { //player?
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool bt_guard::rayCastToPlayer(int types, float& distRay, PxRaycastBuffer& hit) {
	VEC3 myPos = getTransform()->getPosition();
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 origin = myPos + VEC3(0, PLAYER_CENTER_Y, 0);
	VEC3 direction = tPlayer->getPosition() - myPos;
	direction.Normalize();
	float dist = DIST_RAYSHOT;
	//rcQuery.types = types;
	CEntity *e = myParent;
	TCompCharacterController *cc = e->get<TCompCharacterController>();
	Debug->DrawLine(origin + VEC3(0, 0.5f, 0), getTransform()->getFront(), 10.0f);
	bool ret = g_PhysxManager->raycast(origin + direction*cc->GetRadius(), direction, dist, hit);

	if (ret)
		distRay = hit.getAnyHit(0).distance;

	return ret;
}

void bt_guard::shootToPlayer() {
	//If cant shoot returns
	if (noShoot) return;

	//Values
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);

	bool damage = false;
	float distRay;
	if (SBB::readBool("possMode")) {
		damage = true;
		distRay = realDist(myPos, posPlayer);
	}
	else {
		//RayCast to player //TODO RAYCAST
		PxRaycastBuffer hit;
		bool ret = rayCastToPlayer(1, distRay, hit);
		if (ret) {
			CHandle h = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (h.hasTag("player")) {
				damage = true;
			}
		}
	}

	//Do damage
	if (damage && !sendMsgDmg) {
		CEntity* ePlayer = getPlayer();
		sendMsgDmg = !sendMsgDmg;
		TMsgDamage dmg;
		dmg.modif = DAMAGE_LASER;
		ePlayer->sendMsg(dmg);
	}
	else if (!damage && sendMsgDmg) {
		CEntity* ePlayer = getPlayer();
		sendMsgDmg = !sendMsgDmg;
		TMsgStopDamage dmg;
		ePlayer->sendMsg(dmg);
	}

	//Render Debug
	for (int i = 0; i < 8; i++) {
		float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		Debug->DrawLine(myPos + VEC3(r1 - 0.5f, 1 + r2 - 0.5f, 0), posPlayer - myPos, distRay, RED);
	}
}

// -- Jurisdiction -- //
bool bt_guard::inJurisdiction(VEC3 posPlayer) {
	float distanceJur = squaredDistXZ(jurCenter, posPlayer);
	return distanceJur < jurRadiusSq;
}

bool bt_guard::outJurisdiction(VEC3 posPlayer) {
	float distanceJur = squaredDistXZ(jurCenter, posPlayer);
	return distanceJur > jurRadiusSq + DIST_SQ_SHOT_AREA_ENTER;
}

// -- Reset Timers-- //
void bt_guard::resetTimers() {
	timeWaiting = 0;
	deltaYawLookingArround = 0;
	____TIMER_RESET_(timerShootingWall);
	____TIMER_RESET_(timerStunt);
}

/**************
* Load, Render, and RenderInMenu
**************/
#define KPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "kpt%d_%s", index, nameSufix);

bool bt_guard::load(MKeyValue& atts) {
	dbg("load de AI_GUARD\n");
	int n = atts.getInt("kpt_size", 0);
	keyPoints.resize(n);
	for (unsigned int i = 0; i < n; i++) {
		KPT_ATR_NAME(atrType, "type", i);
		KPT_ATR_NAME(atrPos, "pos", i);
		KPT_ATR_NAME(atrWait, "wait", i);
		keyPoints[i] = KeyPoint(
			kptTypes[atts.getString(atrType, "seek")]
			, atts.getPoint(atrPos)
			, atts.getFloat(atrWait, 0.0f)
			);
	}
	noShoot = atts.getBool("noShoot", false);

	//Jurisdiction
	jurCenter = atts.getPoint("jurisdiction");
	jurRadiusSq = atts.getFloat("jurRadius", 1000.0f);
	if (jurRadiusSq < FLT_MAX) jurRadiusSq *= jurRadiusSq;

	return true;
}

void bt_guard::render() {
}

void bt_guard::renderInMenu() {
	ImGui::SliderFloat("Speed Walk", &SPEED_WALK, 0, 1);
	ImGui::SliderFloat("Speed Rot (rad/s)", &SPEED_ROT, 0, 2 * (float)M_PI);
	ImGui::SliderFloat("Cone Vision 1/2 (rads)", &CONE_VISION, 0, (float)M_PI);
	ImGui::SliderFloat("Distance Reach", &DIST_SQ_REACH_PNT, 0, 1);
	ImGui::SliderFloat("Detection Area", &DIST_SQ_PLAYER_DETECTION, 0, 500);
	ImGui::SliderFloat("Shot Area Enter", &DIST_SQ_SHOT_AREA_ENTER, 0, 500);
	ImGui::SliderFloat("Shot Area Leave", &DIST_SQ_SHOT_AREA_LEAVE, 0, 500);
	ImGui::SliderFloat("Lost Player Distance", &DIST_SQ_PLAYER_LOST, 0, 500);
	ImGui::SliderFloat("Laser Shot Reach", &DIST_RAYSHOT, DIST_SQ_SHOT_AREA_ENTER, DIST_SQ_SHOT_AREA_LEAVE * 2);
	ImGui::SliderFloat("Laser Damage", &DAMAGE_LASER, 0, 10);
	ImGui::SliderFloat("Time Shooting Wall before leave", &_timerShootingWall, 0, 15);
}

/**************/
//FROM SCIENTIST
/**************/

void bt_guard::reduceStats()
{
	noShoot = true;
	DIST_SQ_REACH_PNT = DIST_SQ_REACH_PNT_INI / reduce_factor;
	DIST_SQ_SHOT_AREA_ENTER = DIST_SQ_SHOT_AREA_ENTER_INI / reduce_factor;
	DIST_SQ_SHOT_AREA_LEAVE = DIST_SQ_SHOT_AREA_LEAVE_INI / reduce_factor;
	DIST_RAYSHOT = DIST_RAYSHOT_INI / reduce_factor;
	DIST_SQ_PLAYER_DETECTION = DIST_SQ_PLAYER_DETECTION_INI / reduce_factor;
	SPEED_WALK = SPEED_WALK_INI / reduce_factor;
	CONE_VISION = CONE_VISION_INI / reduce_factor;
	SPEED_ROT = SPEED_ROT_INI / reduce_factor;
	DAMAGE_LASER = DAMAGE_LASER_INI / reduce_factor;
}

void bt_guard::resetStats()
{
	DIST_SQ_REACH_PNT = DIST_SQ_REACH_PNT_INI;
	DIST_SQ_SHOT_AREA_ENTER = DIST_SQ_SHOT_AREA_ENTER_INI;
	DIST_SQ_SHOT_AREA_LEAVE = DIST_SQ_SHOT_AREA_LEAVE_INI;
	DIST_RAYSHOT = DIST_RAYSHOT_INI;
	DIST_SQ_PLAYER_DETECTION = DIST_SQ_PLAYER_DETECTION_INI;
	SPEED_WALK = SPEED_WALK_INI;
	CONE_VISION = CONE_VISION_INI;
	SPEED_ROT = SPEED_ROT_INI;
	DAMAGE_LASER = DAMAGE_LASER_INI;
}

//Cambio de malla
void bt_guard::ChangePose(string new_pose_route) {
	PROFILE_FUNCTION("guard bt: change pose");
	if (last_pose != new_pose_route) {
		mesh->unregisterFromRender();
		MKeyValue atts_mesh;
		atts_mesh["name"] = new_pose_route;
		mesh->load(atts_mesh);
		mesh->registerToRender();
		last_pose = new_pose_route;
	}
}

//TODO: remove
void bt_guard::artificialInterrupt()
{
	TCompTransform *t = getTransform();
	float yaw, pitch;
	t->getAngles(&yaw, &pitch);
	t->setAngles(-yaw, pitch);
	setCurrent(NULL);
}