#include "mcv_platform.h"
#include <windows.h>
#include "bt_guard.h"
#include "components/entity_tags.h"
#include "utils/XMLParser.h"
#include "utils/utils.h"
#include "logic/sbb.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "ui/ui_interface.h"

//Render shoot
#include "render/fx/GuardShots.h"

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_GUARD(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_GUARD_P(state) SET_ANIM_STATE_P(animController, state)

//float bt_guard::SHOT_OFFSET = 1.f;

map<string, bt_guard::KptType> bt_guard::kptTypes = {
  {"seek", KptType::Seek}
  , {"look", KptType::Look}
};

map<string, btnode *> bt_guard::tree = {};
map<string, btaction> bt_guard::actions = {};
map<string, btcondition> bt_guard::conditions = {};
map<string, btevent> bt_guard::events = {};
btnode* bt_guard::root = nullptr;
int bt_guard::guards_chasing = 0;

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
	thePlayer = CPlayerBase::handle_player;
	CEntity* player = thePlayer;
	if (!player) {
		dbg("GUARD CAUTION: PLAYER NOT FOUND!\n");
	}
	return player;
}

void bt_guard::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_guard")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields = readIniAtrData(file_ini, "bt_guard");

			readNpcIni(fields);

			assignValueToVar(PLAYER_DETECTION_RADIUS, fields);
			assignValueToVar(DIST_SQ_SHOT_AREA_ENTER, fields);
			assignValueToVar(DIST_SQ_SHOT_AREA_LEAVE, fields);
			assignValueToVar(DIST_RAYSHOT, fields);
			assignValueToVar(DIST_SQ_PLAYER_DETECTION, fields);
			assignValueToVar(DIST_SQ_PLAYER_LOST, fields);
			assignValueToVar(SHOOT_PREP_TIME, fields);
			assignValueToVar(MIN_SQ_DIST_TO_PLAYER, fields);
			assignValueToVar(CONE_VISION, fields);
			CONE_VISION = deg2rad(CONE_VISION);
			assignValueToVar(DAMAGE_LASER, fields);
			assignValueToVar(MAX_REACTION_TIME, fields);
			assignValueToVar(MAX_SEARCH_DISTANCE, fields);
			assignValueToVar(LOOK_AROUND_TIME, fields);
			assignValueToVar(GUARD_ALERT_TIME, fields);
			assignValueToVar(GUARD_ALERT_RADIUS, fields);
			assignValueToVar(reduce_factor, fields);
			assignValueToVar(t_reduceStats_max, fields);
			assignValueToVar(t_reduceStats, fields);
		}
	}
}

void bt_guard::onDifficultyChanged(const TMsgDifficultyChanged&)
{
	readIniFileAttr();
}

/**************
* Init
**************/
void bt_guard::Init()
{
	initParent();
	// read main attributes from file
	readIniFileAttr();

	//Handles
	myHandle = CHandle(this);
	myParent = myHandle.getOwner();
	//animController.init(myParent);
	thePlayer = tags_manager.getFirstHavingTag(getID("player"));

	if (tree.empty()) {
		// insert all states in the map
		createRoot("guard", PRIORITY, NULL, NULL);
		// stuck management
		addNpcStates("guard");
		// formation toggle
		addChild("guard", "formationsequence", SEQUENCE, (btcondition)&bt_guard::checkFormation, NULL);
		addChild("formationsequence", "gotoformation", ACTION, NULL, (btaction)&bt_guard::actionGoToFormation);
		addChild("formationsequence", "turntoformation", ACTION, NULL, (btaction)&bt_guard::actionTurnToFormation);
		addChild("formationsequence", "waitinformation", ACTION, NULL, (btaction)&bt_guard::actionWaitInFormation);
		// stunned state
		addChild("guard", "stunned", ACTION, (btcondition)&bt_guard::playerStunned, (btaction)&bt_guard::actionStunned);
		// attack states
		addChild("guard", "attack_decorator", DECORATOR, (btcondition)&bt_guard::playerDetected, (btaction)&bt_guard::actionReact);
		addChild("attack_decorator", "attack", PRIORITY, NULL, NULL);
		addChild("attack", "chase", ACTION, (btcondition)&bt_guard::playerOutOfReach, (btaction)&bt_guard::actionChase);
		addChild("attack", "absorbsequence", SEQUENCE, NULL, NULL);
		addChild("absorbsequence", "preparetoabsorb", ACTION, NULL, (btaction)&bt_guard::actionPrepareToAbsorb);
		addChild("absorbsequence", "absorb", ACTION, NULL, (btaction)&bt_guard::actionAbsorb);
		addChild("absorbsequence", "shootwall", ACTION, NULL, (btaction)&bt_guard::actionShootWall);
		// alert states
		addChild("guard", "alertdetected", SEQUENCE, (btcondition)&bt_guard::guardAlerted, NULL);
		addChild("alertdetected", "search", ACTION, NULL, (btaction)&bt_guard::actionSearch);
		addChild("alertdetected", "movearound", ACTION, NULL, (btaction)&bt_guard::actionMoveAround);
		addChild("alertdetected", "lookaround", ACTION, NULL, (btaction)&bt_guard::actionLookAround);
		// patrol states
		addChild("guard", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "nextWpt", ACTION, NULL, (btaction)&bt_guard::actionNextWpt);
		addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_guard::actionSeekWpt);
		addChild("patrol", "waitwpt", ACTION, NULL, (btaction)&bt_guard::actionWaitWpt);
	}

	curkpt = 0;
	SET_ANIM_GUARD(AST_IDLE);

	//Other info
	____TIMER_REDEFINE_(timerShootingWall, 1);
	____TIMER_REDEFINE_(timerStunt, 15);
	timeWaiting = 0;
	deltaYawLookingArround = 0;
	stunned = false;
	formation_toggle = false;
}

//conditions
bool bt_guard::playerStunned() {
	PROFILE_FUNCTION("guard: player stunned");
	if (stunned == true) {
		logic_manager->throwEvent(logic_manager->OnStunned, MY_NAME);
		logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
		decreaseChaseCounter();
		return true;
	}
	return false;
}

bool bt_guard::playerNear() {
	PROFILE_FUNCTION("guard: player near");
	CEntity* ePlayer = getPlayer();
	if (!ePlayer) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();

	float distance = squaredDistXZ(myPos, posPlayer);
	if (distance < MIN_SQ_DIST_TO_PLAYER) {
		return true;
	}
	else {
		return false;
	}
}

bool bt_guard::playerDetected() {
	PROFILE_FUNCTION("guard: player detected");
	// if the player is visible
	if (playerVisible() || boxMovingDetected()) {
		TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
		VEC3 posPlayer = tPlayer->getPosition();
		VEC3 myPos = getTransform()->getPosition();
		float distance = squaredDistXZ(myPos, posPlayer);

		// we send a new alert with our position and the player position
		guard_alert alert;
		alert.guard_position = myPos;
		alert.alert_position = posPlayer;
		alert.timer = GUARD_ALERT_TIME;

		CEntity* entity = myHandle.getOwner();
		string name = entity->getName() + string("_player_detected");

		SBB::postGuardAlert(name, alert);

		logic_manager->throwEvent(logic_manager->OnDetected, std::to_string(distance), CHandle(this).getOwner());

		return true;
	}
	else {
		return false;
	}
}

bool bt_guard::playerOutOfReach() {
	PROFILE_FUNCTION("guard: player out of reach");
	CEntity * ePlayer = getPlayer();
	if (!ePlayer)
		return false;
	if (!playerVisible()) {
		decreaseChaseCounter();
		return true;
	}

	//Calc out of reach
	bool res;
	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);
	res = (distance > DIST_SQ_SHOT_AREA_ENTER);

	//Update animation
	if (res) SET_ANIM_GUARD(AST_MOVE);
	else SET_ANIM_GUARD(AST_SHOOT);

	//Return calc
	return res;
}

bool bt_guard::guardAlerted() {
	PROFILE_FUNCTION("guard: guardalert");

	VEC3 myPos = getTransform()->getPosition();
	CEntity* ePlayer = getPlayer();
	if (!ePlayer) return false;

	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();

	// If he player is out of jurisdiction, we forget about him
	if (outJurisdiction(posPlayer)) {
		return false;
	}

	// if playerLost is active (our own alert), we need to search for him
	if (playerLost) {
		// Get the position of the player where we will search
		TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
		VEC3 posPlayer = tPlayer->getPosition();
		player_last_seen_point = posPlayer;
		// We send a new alert with our position and the player position to the rest of the guards
		guard_alert alert;
		alert.guard_position = myPos;
		alert.timer = GUARD_ALERT_TIME;
		CEntity* entity = myHandle.getOwner();
		alert.alert_position = player_last_seen_point;
		string name = entity->getName() + string("_player_lost");
		SBB::postGuardAlert(name, alert);
		return true;
	}
	// check alerts from other guards
	else {
		std::map<string, guard_alert> guard_alerts = SBB::sbbGuardAlerts;

		if (!guard_alerts.empty()) {
			for (std::map<string, guard_alert>::iterator alert_it = guard_alerts.begin(); alert_it != guard_alerts.end(); alert_it++) {
				VEC3 guard_alert_position = alert_it->second.guard_position;

				// the guard will be alerted if he is near enough
				if (simpleDist(myPos, guard_alert_position) < GUARD_ALERT_RADIUS) {
					VEC3 alert_point = alert_it->second.alert_position;
					if ((alert_it->first.find(string("_player_lost")) != string::npos) ||
						(alert_it->first.find(string("_player_detected")) != string::npos)) {
						playerLost = true;
						player_last_seen_point = alert_point;
						return true;
					}
				}
			}
		}
	}

	return false;
}

//toggle conditions
bool bt_guard::checkFormation() {
	return formation_toggle;
}

int bt_guard::actionStunned() {
	PROFILE_FUNCTION("guard: actionstunned");
	if (!myParent.isValid()) return false;
	lookAtFront();
	stuck = false;
	stuck_time = 0.f;
	if (!stunt_recover) return STAY;
	if (timerStunt < 0) {
		stunned = false;
		logic_manager->throwEvent(logic_manager->OnStunnedEnd, "");
		return OK;
	}
	else {
		if (timerStunt > -1)
			timerStunt -= getDeltaTime();
		return STAY;
	}
}

int bt_guard::actionStepBack() {
	PROFILE_FUNCTION("guard: actionstepback");
	SET_ANIM_GUARD(AST_MOVE);
	goForward(-2.f*SPEED_WALK);
	turnToPlayer();

	if (playerNear()) return STAY;
	else return OK;
}

void bt_guard::updateLookAt()
{
	if (looking_player) {
		lookAtPlayer();
	}
}

void bt_guard::lookAtPlayer()
{
	CEntity * ePlayer = getPlayer();
	if (ePlayer) {
		GET_MY(look_at, TCompSkeletonLookAt);
		TCompCharacterController * ccPlayer = ePlayer->get<TCompCharacterController>();
		if (look_at && ccPlayer) {
			looking_player = true;
			look_at->setTarget(ccPlayer->GetPosition());
		}
	}
}

void bt_guard::lookAtFront()
{
	GET_MY(look_at, TCompSkeletonLookAt);
	if (look_at) {
		if (looking_player) {
			looking_player = false;
			look_at->setTarget(VEC3());
		}
	}
}

int bt_guard::actionReact() {
	PROFILE_FUNCTION("guard: actionreact");
	if (!myParent.isValid()) return false;
	//lookAtPlayer();
	looking_player = true;

	if (!player_detected_start) {
		// starting the reaction time decorator
		player_detected_start = true;
		reaction_time = (rand() % (int)MAX_REACTION_TIME);
	}

	// stay in this state until the reaction time is over
	if (reaction_time < 0.f) {
		player_detected_start = false;
		increaseChaseCounter();
		return OK;
	}
	else {
		if (reaction_time > -1)
			reaction_time -= getDeltaTime();
		stuck_time = 0.f;
		return STAY;
	}
}

int bt_guard::actionChase() {
	PROFILE_FUNCTION("guard: chase");
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return STAY;
	if (!myParent.isValid()) return STAY;

	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);
	//player lost?
	if (distance > DIST_SQ_PLAYER_LOST) {
		playerLost = true;
		player_last_seen_point = posPlayer;
		SET_ANIM_GUARD(AST_IDLE);
		decreaseChaseCounter();
		return KO;
	}
	//player near?
	else if (distance < DIST_SQ_SHOT_AREA_ENTER) {
		SET_ANIM_GUARD(AST_PREP_SHOOT);
		if (!playerVisible()) {
			//if the distance is short but we dont see the player anymore, search for him
			playerLost = true;
			player_last_seen_point = posPlayer;
			SET_ANIM_GUARD(AST_IDLE);
			decreaseChaseCounter();
			return KO;
		}
		else {
			//if we see the player, shot him!
			return OK;
		}
	}
	// chase the player
	else {
		getPath(myPos, posPlayer);
		SET_ANIM_GUARD(AST_MOVE);
		goTo(posPlayer);
		return STAY;
	}
}

int bt_guard::actionPrepareToAbsorb() {
	PROFILE_FUNCTION("guard: prepare to absorb");
	if (!myParent.isValid()) return false;
	logic_manager->throwEvent(logic_manager->OnGuardMovingStop, "");
	shoot_preparation_time += getDeltaTime();
	dbg("PREPARING TO SHOOT!\n");
	if (shoot_preparation_time > SHOOT_PREP_TIME) {
		shoot_preparation_time = 0.f;
		return OK;
	}
	else {
		stuck_time = 0.f;
		return STAY;
	}
}

int bt_guard::actionAbsorb() {
	PROFILE_FUNCTION("guard: absorb");
	if (!myParent.isValid()) return false;
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return STAY;
	stuck_time = 0.f;

	// if player too near, move backwards
	if (playerNear()) {
		dbg("SHOOTING BACKWARDS!\n");
		SET_ANIM_GUARD(AST_SHOOT_BACK);
		goForward(-0.75f*SPEED_WALK);
		turnToPlayer();
		shootToPlayer();
		shooting_backwards = true;
		return STAY;
	}

	shooting_backwards = false;
	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float dist = squaredDistXZ(posPlayer, myPos);

#ifndef NDEBUG
	ui.addTextInstructions("\nPress 'M' to interrupt gaurd shoot when he dont see you!!! (artificial)\n");
	if (controller->interruptGuardShotButtonPressed()) {
		artificialInterrupt();
	}
#endif // !NDEBUG

	turnTo(posPlayer);
	if (squaredDistY(myPos, posPlayer) * 2.0f > dist) { //Angulo de 30 grados
		//Si pitch muy alto me alejo
		goForward(-SPEED_WALK);
	}
	// if we see the payer, shoot at him
	if (playerVisible()) {
		SET_ANIM_GUARD(AST_SHOOT);
		turnToPlayer();
		shootToPlayer();
		return STAY;
	}
	// if we don't see the player anymore and we were shooting
	else if (shooting) {
		shooting = false;
		// throw interrupt hit event
		logic_manager->throwEvent(logic_manager->OnInterruptHit, "");
		// stop damaging the player
		CEntity* ePlayer = getPlayer();
		sendMsgDmg = !sendMsgDmg;
		TMsgDamageSpecific dmg;
		CEntity* entity = myHandle.getOwner();
		dmg.source = entity->getName();
		dmg.type = Damage::ABSORB;
		dmg.actived = false;
		ePlayer->sendMsg(dmg);
		// if the player went out of reach, we don't shoot the wall
		if (dist > DIST_SQ_PLAYER_DETECTION) {
			logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
			playerLost = true;
			player_last_seen_point = posPlayer;
			SET_ANIM_GUARD(AST_IDLE);
			decreaseChaseCounter();
			return KO;
		}
		else {
			return OK;
		}
	}

	logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
	decreaseChaseCounter();
	return KO;
}

int bt_guard::actionShootWall() {
	PROFILE_FUNCTION("guard: shootwall");
	if (!myParent.isValid()) return false;
	CEntity* ePlayer = getPlayer();
	if (!ePlayer) return STAY;

	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();

	turnTo(posPlayer);

	//If the player is visible, we stop shooting the wall
	if (playerVisible() || boxMovingDetected()) {
		logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
		decreaseChaseCounter();
		return OK;
	}
	else {
		//We shoot in the player direction, and stop if the timer is finished
		turnToPlayer();
		shootToPlayer();
		if (timerShootingWall < 0) {
			playerLost = true;
			player_last_seen_point = posPlayer;
			logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
			____TIMER_REDEFINE_(timerShootingWall, 1);
			decreaseChaseCounter();
			return OK;
		}
		else {
			if (timerShootingWall > -1)
				timerShootingWall -= getDeltaTime();
			stuck_time = 0.f;
			return STAY;
		}
	}
}

int bt_guard::actionSearch() {
	PROFILE_FUNCTION("guard: search");
	if (!myParent.isValid()) return STAY;
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return STAY;

	lookAtFront();
	looking_around_time -= getDeltaTime();
	VEC3 myPos = getTransform()->getPosition();

	// player/box visible or search time ended
	if (playerVisible() || boxMovingDetected() || looking_around_time < 0.f) {
		looking_around_time = LOOK_AROUND_TIME;
		return KO;
	}
	else if (playerLost) {
		float distance = simpleDistXZ(myPos, player_last_seen_point);
		// go to player last seen point
		getPath(myPos, player_last_seen_point);
		SET_ANIM_GUARD(AST_MOVE);
		goTo(player_last_seen_point);
		if (distance < DIST_REACH_PNT) {
			playerLost = false;

			TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
			VEC3 playerPos = tPlayer->getPosition();

			VEC3 dir = playerPos - myPos;
			dir.Normalize();

			search_player_point = playerPos + 1.0f * dir;
			return OK;
		}
		else {
			return STAY;
		}
	}
	// If player was lost, we simply move and look around
	else {
		return OK;
	}
}

int bt_guard::actionMoveAround() {
	PROFILE_FUNCTION("guard: movearound");
	if (!myParent.isValid()) return false;
	looking_around_time -= getDeltaTime();
	// player/box visible or search time ended
	if (playerVisible() || boxMovingDetected() || looking_around_time <= 0.f) {
		looking_around_time = LOOK_AROUND_TIME;
		return KO;
	}

	VEC3 myPos = getTransform()->getPosition();
	float distance_to_point = simpleDistXZ(myPos, search_player_point);

	// if the player is too far, we just look around
	if (distance_to_point > MAX_SEARCH_DISTANCE) {
		return OK;
	}

	if (distance_to_point > DIST_REACH_PNT) {
		getPath(myPos, search_player_point);
		SET_ANIM_GUARD(AST_MOVE);
		goTo(search_player_point);
		return STAY;
	}

	return OK;
}

int bt_guard::actionLookAround() {
	PROFILE_FUNCTION("guard: lookaround");
	if (!myParent.isValid()) return false;
	looking_around_time -= getDeltaTime();
	//Player Visible?
	if (playerVisible() || boxMovingDetected() || looking_around_time <= 0.f) {
		looking_around_time = LOOK_AROUND_TIME;
		return KO;
	}
	// Turn arround
	else if (deltaYawLookingArround < 2 * M_PI) {
		SET_ANIM_GUARD(AST_TURN);
		float yaw, pitch;
		getTransform()->getAngles(&yaw, &pitch);

		float deltaYaw = SPEED_ROT * getDeltaTime();
		deltaYawLookingArround += deltaYaw;
		yaw += deltaYaw;
		getTransform()->setAngles(yaw, pitch);
		looking_around_time -= getDeltaTime();
		stuck_time = 0.f;
		return STAY;
	}
	else {
		deltaYawLookingArround = 0;
		looking_around_time = LOOK_AROUND_TIME;
		return OK;
	}
}

int bt_guard::actionSeekWpt() {
	PROFILE_FUNCTION("guard: actionseekwpt");
	if (!myParent.isValid()) return 0;
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = keyPoints[curkpt].pos;
	patrolling = true;
	//Player Visible?
	if (playerVisible() || boxMovingDetected()) {
		patrolling = false;
		return KO;
	}
	//Go to waypoint
	else if (keyPoints[curkpt].type == Seek) {
		//reach waypoint?
		if (simpleDistXZ(myPos, dest) < DIST_REACH_PNT) {
			curkpt = (curkpt + 1) % keyPoints.size();
			return OK;
		}
		else {
			getPath(myPos, dest);
			SET_ANIM_GUARD(AST_MOVE);
			goTo(dest);
			return STAY;
		}
	}
	//Look to waypoint
	else if (keyPoints[curkpt].type == Look) {
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
	lookAtFront();
	if (keyPoints.size() == 0) return false;
	SET_ANIM_GUARD(AST_TURN);
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = keyPoints[curkpt].pos;
	//Player Visible?
	if (playerVisible() || boxMovingDetected()) {
		patrolling = false;
		return KO;
	}
	//If we are already there, we continue
	if (simpleDistXZ(myPos, dest) < DIST_REACH_PNT)
		return OK;
	//Look to waypoint
	if (turnTo(dest)) {
		return OK;
	}
	else {
		return STAY;
	}
}

int bt_guard::actionWaitWpt() {
	//PROFILE_FUNCTION("guard: actionwaitwpt");
	if (!myParent.isValid()) return false;
	SET_ANIM_GUARD(AST_IDLE);
	stuck = false;
	stuck_time = 0.f;
	//player visible?
	if (playerVisible() || boxMovingDetected()) {
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		patrolling = false;
		return KO;
	}
	else if (timeWaiting > keyPoints[curkpt].time) {
		timeWaiting = 0;
		patrolling = false;
		return OK;
	}
	else {
		timeWaiting += getDeltaTime();
		return STAY;
	}
}

// toggle actions
int bt_guard::actionGoToFormation() {
	PROFILE_FUNCTION("guard: gotoformation");
	if (!myParent.isValid()) return false;

	VEC3 myPos = getTransform()->getPosition();

	float distance_to_point = simpleDistXZ(myPos, formation_point);

	// if we didn't reach the point
	if (distance_to_point > DIST_REACH_PNT) {
		getPath(myPos, formation_point);
		SET_ANIM_GUARD(AST_MOVE);
		goTo(formation_point);
		return STAY;
	}
	return OK;
}

int bt_guard::actionTurnToFormation() {
	PROFILE_FUNCTION("guard: turntoformation");
	if (!myParent.isValid()) return false;

	VEC3 dest = formation_dir;

	if (turnTo(dest)) {
		SET_ANIM_GUARD(AST_IDLE);
		return OK;
	}
	else {
		SET_ANIM_GUARD(AST_TURN);
		return STAY;
	}
}

int bt_guard::actionWaitInFormation() {
	PROFILE_FUNCTION("guard: waitinformation");
	if (!myParent.isValid()) return false;

	VEC3 dest = formation_dir;

	if (!formation_toggle) {
		return OK;
	}
	else {
		return STAY;
	}
}

/**************
* Mensajes
**************/
void bt_guard::noise(const TMsgNoise& msg) {
	PROFILE_FUNCTION("guard: noise");
	if (outJurisdiction(msg.source)) return;
	if (playerVisible() || boxMovingDetected()) return;
	if (stunned) return;
	if (canHear(msg.source, msg.intensity)) {
		resetTimers();
		noisePoint = msg.source;
		noiseHeard = true;
		setCurrent(NULL);
	}
}

void bt_guard::onStaticBomb(const TMsgStaticBomb& msg) {
	PROFILE_FUNCTION("guard: onstaticbomb");
	VEC3 myPos = getTransform()->getPosition();
	if (squaredDist(msg.pos, myPos) < msg.r * msg.r) {
		resetTimers();
		stunned = true;
		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnStunned, MY_NAME);
		SET_ANIM_GUARD(AST_STUNNED);
		checkStopDamage();
		setCurrent(NULL);
	}
}

void bt_guard::onOverCharged(const TMsgOverCharge& msg) {
	PROFILE_FUNCTION("guard: onovercharge");
	CEntity * entity = myHandle.getOwner();
	string guard_name = entity->getName();

	if (std::find(msg.guard_names.begin(), msg.guard_names.end(), guard_name) != msg.guard_names.end()) {
		logic_manager->throwEvent(logic_manager->OnGuardOvercharged, "");
		stunned = true;
		____TIMER_RESET_(timerStunt);
		setCurrent(NULL);
		SET_ANIM_GUARD(AST_STUNNED);
		//End Damage Message
		checkStopDamage();
	}
}

void bt_guard::checkStopDamage() {
	if (sendMsgDmg) {
		CEntity * ePlayer = getPlayer();
		if (ePlayer) {
			//End Damage Message
			sendMsgDmg = shooting = false;

			TMsgDamageSpecific dmg;
			dmg.source = compBaseEntity->getName();
			dmg.type = Damage::ABSORB;
			dmg.actived = false;

			ePlayer->sendMsg(dmg);
		}
	}
}

void bt_guard::onBoxHit(const TMsgBoxHit& msg) {
	PROFILE_FUNCTION("guard: onboxhit");
	CEntity * entity = myHandle.getOwner();

	logic_manager->throwEvent(logic_manager->OnGuardBoxHit, "");
	stunned = true;
	____TIMER_RESET_(timerStunt);
	setCurrent(NULL);
	SET_ANIM_GUARD(AST_STUNNED_BOX);

	//If was shooting...
	if (shooting) {
		TMsgDamageSpecific dmg;
		dmg.source = entity->getName();
		dmg.type = Damage::ABSORB;
		dmg.actived = false;
		CEntity * ePlayer = getPlayer();
		if (ePlayer) ePlayer->sendMsg(dmg);
		else fatal("Cannot send end damage message to player!");
		//End Damage Message
		sendMsgDmg = shooting = false;
	}
}

/**************
 * Auxiliares
 **************/

 // -- Go To -- //
bool bt_guard::canHear(VEC3 position, float intensity) {
	return (realDist(getTransform()->getPosition(), position) < DIST_SQ_SOUND_DETECTION);
}

bool bt_guard::turnToPlayer()
{
	CEntity* ePlayer = getPlayer();
	if (ePlayer) {
		GET_COMP(tPlayer, CHandle(ePlayer), TCompTransform);
		return turnTo(tPlayer->getPosition());
	}
	return true;
}

// -- Player Visible? -- //
bool bt_guard::playerVisible(bool check_raycast) {
	if (!myParent.isValid()) return false;
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return false;

	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float dist_sq = squaredDistXZ(myPos, posPlayer);

	if (SBB::readBool("possMode") && dist_sq > 25.f) {
		return false;
	}

	bool res = false;

	// deteccion por radio minimo
	if (simpleDist(posPlayer, myPos) < PLAYER_DETECTION_RADIUS) {
		res = true;
	}
	// deteccion por vision
	else {
		float distancia_vertical = squaredDistY(posPlayer, myPos);

		if (distancia_vertical < dist_sq * 0.5f) { //Pitch < 30
			if (getTransform()->isHalfConeVision(posPlayer, CONE_VISION)) { //Cono vision
				if (dist_sq < DIST_SQ_PLAYER_DETECTION) { //Distancia
					if (inJurisdiction(posPlayer)) { //Jurisdiccion
						float distRay;
						if (SBB::readBool("possMode")) {
							// Estas poseyendo, estas cerca y dentro del cono de vision, no hace falta raycast
							res = true;
						}
						else {
							//Raycast
							if (!check_raycast) {
								res = true;
							}
							else {
								PxRaycastBuffer hit;
								bool ret = rayCastToPlayer(1, distRay, hit);
								if (ret) { //No bloquea vision
									CHandle h = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
									dest_shoot = PhysxConversion::PxVec3ToVec3(hit.getAnyHit(0).position);
									if (h == CPlayerBase::handle_player || dist_sq < squaredDistXZ(myPos, dest_shoot)) {
										res = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// if we see the player, it's not lost
	if (res)
		playerLost = false;
	return res;
}

bool bt_guard::boxMovingDetected() {
	if (!myParent.isValid()) return false;
	// moving boxes detection
	float distRay;
	PxRaycastBuffer hit;
	bool ret = rayCastToFront(1, distRay, hit);
	if (ret) { //No bloquea vision
		CHandle h = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
		if (h.hasTag("box")) { //box?
			CEntity* box = h;
			TCompPolarized* pol_component = box->get<TCompPolarized>();
			if (pol_component && pol_component->moving) {
				return true;
			}
		}
	}
	return false;
}

bool bt_guard::rayCastToPlayer(int types, float& distRay, PxRaycastBuffer& hit) {
	CEntity* ePlayer = getPlayer();
	if (!ePlayer) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	return rayCastToTransform(types, distRay, hit, tPlayer);
}

bool bt_guard::rayCastToTransform(int types, float& distRay, PxRaycastBuffer& hit, TCompTransform* transform) {
	VEC3 myPos = getTransform()->getPosition();
	VEC3 origin = myPos + VEC3(0, PLAYER_CENTER_Y, 0);
	VEC3 direction = transform->getPosition() - myPos;
	direction.Normalize();
	float dist = DIST_RAYSHOT;
	//rcQuery.types = types;
	CEntity *e = myParent;
	TCompCharacterController *cc = e->get<TCompCharacterController>();
	Debug->DrawLine(origin + direction*(cc->GetRadius() + 0.075f), getTransform()->getFront(), 10.0f);
	bool ret = g_PhysxManager->raycast(origin + direction*(cc->GetRadius() + 0.075f), direction, dist, hit);

	if (ret)
		distRay = hit.getAnyHit(0).distance;

	return ret;
}

bool bt_guard::rayCastToFront(int types, float& distRay, PxRaycastBuffer& hit) {
	VEC3 myPos = getTransform()->getPosition();
	VEC3 origin = myPos + VEC3(0, PLAYER_CENTER_Y, 0);
	VEC3 direction = getTransform()->getFront();
	direction.Normalize();
	float dist = DIST_RAYSHOT;
	//rcQuery.types = types;
	CEntity *e = myParent;
	TCompCharacterController *cc = e->get<TCompCharacterController>();
	Debug->DrawLine(origin + direction*(cc->GetRadius() + 0.1f), getTransform()->getFront(), 10.0f);
	bool ret = g_PhysxManager->raycast(origin + direction*(cc->GetRadius() + 0.1f), direction, dist, hit);

	if (ret)
		distRay = hit.getAnyHit(0).distance;

	return ret;
}

void bt_guard::shootToPlayer() {
	//If cant shoot returns
	if (noShoot) return;
	CEntity* ePlayer = getPlayer();
	if (!ePlayer) return;

	//Values
	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);

	bool damage = false;
	bool raijin = false;
	float distRay;
	if (SBB::readBool("possMode")) {
		damage = true;
		distRay = realDist(myPos, posPlayer);
	}
	else {
		//RayCast to player
		PxRaycastBuffer hit;
		bool ret = rayCastToPlayer(1, distRay, hit);
		dest_shoot = PhysxConversion::PxVec3ToVec3(hit.getAnyHit(0).position);
		if (ret) {
			CHandle h = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (h == CPlayerBase::handle_player) {
				damage = true;
				raijin = h.hasTag("raijin");
			}
		}
	}

	CEntity *entity = myHandle.getOwner();

	//Do damage
	if (raijin) {
		if (damage && !sendMsgDmg && !shooting
			|| !damage && sendMsgDmg && shooting
			) {
			shooting = sendMsgDmg = true;

			TMsgDamageSpecific dmg;
			dmg.source = entity->getName();
			dmg.type = Damage::ABSORB;
			dmg.actived = damage;
			ePlayer->sendMsg(dmg);
		}
	}
	else {
		TMsgUnpossesDamage msgUnpossess;
		ePlayer->sendMsg(msgUnpossess);
	}

	//Render Debug
	//for (int i = 0; i < 8; i++) {
	//	float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	Debug->DrawLine(myPos + VEC3(r1 - 0.5f, 1 + r2 - 0.5f, 0), posPlayer - myPos, distRay, RED);
	//}

	if (shooting_backwards || distance < squaredDistXZ(myPos, dest_shoot)) {
		dest_shoot = posPlayer;
		dest_shoot.y += 0.5f;
	}

	//Render Shot
	drawShot(dest_shoot);

	// calling OnGuardAttackEvent
	logic_manager->throwEvent(logic_manager->OnGuardAttack, "", CHandle(this).getOwner());
}

void bt_guard::drawShot(VEC3 dest) {
	PROFILE_FUNCTION("guard bt: draw shot");
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return;
	// Centro del personaje
	TCompCharacterController * cc = ePlayer->get<TCompCharacterController>();
	TCompTransform *t = ePlayer->get<TCompTransform>();
	assert(cc || fatal("Player doesn't have character controller!"));
	VEC3 posPlayer = cc->GetPosition();

	// Origin and rayshot
	//VEC4 originShot4;
	//VEC4::Transform(SHOT_OFFSET, getTransform()->asMatrix(), originShot4);
	//VEC3 originShot = VEC3(originShot4.x, originShot4.y, originShot4.z);
	GET_MY(skel, TCompSkeleton);
	VEC3 hand = skel->getBonePos(KEYBONE_RHAND);
	VEC3 arm = skel->getBonePos(KEYBONE_RARM);
	//VEC3 originShot = hand + (hand - arm) * SHOT_OFFSET;
	VEC3 originShot = hand + hand - arm;
	VEC3 destShot = dest; //algun offset?

	// Add Render Instruction
	ShootManager::shootLaser(originShot, destShot);
}

void bt_guard::removeBox(CHandle box_handle) {
	CEntity* box = box_handle;
	TCompPhysics* box_physx = box->get<TCompPhysics>();
	int lateral_force = rand() % 2500;
	box_physx->AddForce(VEC3(lateral_force, rand() % 100, 2500 - lateral_force));
}

// -- Jurisdiction -- //
bool bt_guard::inJurisdiction(VEC3 posPlayer) {
	/*float distanceJur = squaredDistXZ(jurCenter, posPlayer);
	return distanceJur < jurRadiusSq;*/
	return true;
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
	for (int i = 0; i < n; i++) {
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

	//Formation
	formation_point = atts.getPoint("formation_point");
	formation_dir = atts.getPoint("formation_dir");

	//Stunt_recover
	stunt_recover = atts.getBool("stunt_recover", true);
	return true;
}

std::string bt_guard::getKpTypeStr(bt_guard::KptType type)
{
	if (type == KptType::Seek) return "seek";
	else return "look";
}

bool bt_guard::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("kpt_size", (int)keyPoints.size());
	int i = 0;
	for (auto kp : keyPoints) {
		KPT_ATR_NAME(atrType, "type", i);
		KPT_ATR_NAME(atrPos, "pos", i);
		KPT_ATR_NAME(atrWait, "wait", i);
		atts.put(atrType, getKpTypeStr(kp.type));
		atts.put(atrPos, kp.pos);
		atts.put(atrWait, kp.time);
		i++;
	}
	atts.put("jurisdiction", jurCenter);
	atts.put("jurRadius", sqrtf(jurRadiusSq));
	atts.put("formation_point", formation_point);
	atts.put("formation_dir", formation_dir);

	save_bt(os, atts);
	return true;
}

void bt_guard::render() {
}

void bt_guard::renderInMenu() {
	ImGui::SliderFloat("Speed Walk", &SPEED_WALK, 0, 1);
	ImGui::SliderFloat("Speed Rot (rad/s)", &SPEED_ROT, 0, 2 * (float)M_PI);
	ImGui::SliderFloat("Cone Vision 1/2 (rads)", &CONE_VISION, 0, (float)M_PI);
	ImGui::SliderFloat("Distance Reach", &DIST_REACH_PNT, 0, 1);
	ImGui::SliderFloat("Detection Area", &DIST_SQ_PLAYER_DETECTION, 0, 500);
	ImGui::SliderFloat("Shot Area Enter", &DIST_SQ_SHOT_AREA_ENTER, 0, 500);
	ImGui::SliderFloat("Shot Area Leave", &DIST_SQ_SHOT_AREA_LEAVE, 0, 500);
	ImGui::SliderFloat("Lost Player Distance", &DIST_SQ_PLAYER_LOST, 0, 500);
	ImGui::SliderFloat("Laser Shot Reach", &DIST_RAYSHOT, DIST_SQ_SHOT_AREA_ENTER, DIST_SQ_SHOT_AREA_LEAVE * 2);
	ImGui::SliderFloat("Laser Damage", &DAMAGE_LASER, 0, 10);
	ImGui::SliderFloat("Time Shooting Wall before leave", &_timerShootingWall, 0, 15);
	ImGui::Separator();
	//ImGui::DragFloat("Offset Starting Shot", &SHOT_OFFSET);
	if (bt::current) ImGui::Text("NODE: %s", bt::current->getName().c_str());
	else ImGui::Text("NODE: %s", "???\n");
	ImGui::Text("Next patrol: %d, Type: %s, Pos: (%f,%f,%f), Wait: %f"
		, curkpt
		, keyPoints[curkpt].type == KptType::Seek ? "Seek" : "Look"
		, VEC3_VALUES(keyPoints[curkpt].pos)
		, keyPoints[curkpt].time);
	//ImGui::Text("Esto tira? %d", curkpt);
}

/**************/
//FROM SCIENTIST
/**************/

void bt_guard::reduceStats()
{
	noShoot = true;
	DIST_REACH_PNT = DIST_SQ_REACH_PNT_INI / reduce_factor;
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
	DIST_REACH_PNT = DIST_SQ_REACH_PNT_INI;
	DIST_SQ_SHOT_AREA_ENTER = DIST_SQ_SHOT_AREA_ENTER_INI;
	DIST_SQ_SHOT_AREA_LEAVE = DIST_SQ_SHOT_AREA_LEAVE_INI;
	DIST_RAYSHOT = DIST_RAYSHOT_INI;
	DIST_SQ_PLAYER_DETECTION = DIST_SQ_PLAYER_DETECTION_INI;
	SPEED_WALK = SPEED_WALK_INI;
	CONE_VISION = CONE_VISION_INI;
	SPEED_ROT = SPEED_ROT_INI;
	DAMAGE_LASER = DAMAGE_LASER_INI;
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

// function that will be used from LUA
void bt_guard::goToPoint(VEC3 dest) {
	PROFILE_FUNCTION("guard: go to point");
	if (!myParent.isValid()) return;

	SET_ANIM_GUARD(AST_MOVE);
	forced_move = true;

	// if we didn't reach the point
	// Will it really work? It will work like teleport....
	while (simpleDistXZ(getTransform()->getPosition(), dest) > DIST_REACH_PNT) {
		getPath(getTransform()->getPosition(), dest);
		goTo(dest);
	}

	SET_ANIM_GUARD(AST_IDLE);
	forced_move = false;
}

bool bt_guard::isPatrolling() {
	return patrolling;
}

bool bt_guard::isInFirstSeekPoint()
{
	bool res = false;
	for (auto kp : keyPoints) {
		if (kp.type == KptType::Seek) {
			GET_MY(tmx, TCompTransform);
			res = inSquaredRangeXZ_Y(kp.pos, tmx->getPosition(), 0.5f, 5.f);
			break;
		}
	}
	return res;
}

void bt_guard::increaseChaseCounter() {
	//if im the first guard that is chasing, start the chase music
	if (!chasing) {
		chasing = true;
		guards_chasing++;
		if (guards_chasing == 1)
			logic_manager->throwEvent(logic_manager->OnGuardChase, "0.2");
	}
}
void bt_guard::decreaseChaseCounter() {
	//if im the last guard that is chasing, stop the chase music
	if (chasing) {
		chasing = false;
		guards_chasing--;
		if (guards_chasing == 0)
			logic_manager->throwEvent(logic_manager->OnGuardChaseEnd, "0.2");
	}
}

void bt_guard::changeCommonState(std::string state)
{
	//if (state == AST_IDLE)
	SET_ANIM_GUARD(state);
}

void bt_guard::onGetWhoAmI(TMsgGetWhoAmI& msg)
{
	msg.who = PLAYER_TYPE::GUARD;
	msg.who_string = "Guard";
	if (msg.action_flag)
		step_counter = (step_counter + 1) % 4;
}