#include "mcv_platform.h"
#include <windows.h>
#include "bt_guard.h"
#include "components/entity_tags.h"
#include "utils/XMLParser.h"
#include "utils/utils.h"
#include "logic/sbb.h"
#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "ui/ui_interface.h"

//Render shoot
#include "render/fx/GuardShots.h"

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_GUARD(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_GUARD_P(state) SET_ANIM_STATE_P(animController, state)

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
	thePlayer = tags_manager.getFirstHavingTag("player");
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

			assignValueToVar(DIST_REACH_PNT, fields);
			assignValueToVar(PLAYER_DETECTION_RADIUS, fields);
			assignValueToVar(DIST_SQ_SHOT_AREA_ENTER, fields);
			assignValueToVar(DIST_SQ_SHOT_AREA_LEAVE, fields);
			assignValueToVar(DIST_RAYSHOT, fields);
			assignValueToVar(DIST_SQ_PLAYER_DETECTION, fields);
			assignValueToVar(DIST_SQ_PLAYER_LOST, fields);
			assignValueToVar(SPEED_WALK, fields);
			assignValueToVar(SHOOT_PREP_TIME, fields);
			assignValueToVar(MIN_SQ_DIST_TO_PLAYER, fields);
			assignValueToVar(CONE_VISION, fields);
			CONE_VISION = deg2rad(CONE_VISION);
			assignValueToVar(SPEED_ROT, fields);
			SPEED_ROT = deg2rad(SPEED_ROT);
			assignValueToVar(DAMAGE_LASER, fields);
			assignValueToVar(MAX_REACTION_TIME, fields);
			assignValueToVar(MAX_BOX_REMOVAL_TIME, fields);
			assignValueToVar(BOX_REMOVAL_ANIM_TIME, fields);
			assignValueToVar(MAX_SEARCH_DISTANCE, fields);
			assignValueToVar(LOOK_AROUND_TIME, fields);
			assignValueToVar(GUARD_ALERT_TIME, fields);
			assignValueToVar(GUARD_ALERT_RADIUS, fields);
			assignValueToVar(RANDOM_POINT_MAX_DISTANCE, fields);
			assignValueToVar(reduce_factor, fields);
			assignValueToVar(t_reduceStats_max, fields);
			assignValueToVar(t_reduceStats, fields);
			assignValueToVar(MAX_STUCK_TIME, fields);
			SHOT_OFFSET = VEC4(0, 1.5f, 0.5f, 1);
		}
	}
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
		addChild("guard", "stuck", ACTION, (btcondition)&bt_guard::guardStuck, (btaction)&bt_guard::actionUnstuck);
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
		addChild("absorbsequence", "removebox", ACTION, NULL, (btaction)&bt_guard::actionRemoveBox);
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
bool bt_guard::guardStuck() {
	PROFILE_FUNCTION("guard: guard stuck");
	return stuck;
}

bool bt_guard::playerStunned() {
	PROFILE_FUNCTION("guard: player stunned");
	if (stunned == true) {
		logic_manager->throwEvent(logic_manager->OnStunned, "");
		logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
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
	if ((playerVisible() || boxMovingDetected()) && !player_detected) {
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

		logic_manager->throwEvent(logic_manager->OnDetected, std::to_string(distance) + " " + std::to_string(myPos.x) + " " + std::to_string(myPos.y) + " " + std::to_string(myPos.z));
		return true;
	}
	else {
		return false;
	}
}

bool bt_guard::playerOutOfReach() {
	PROFILE_FUNCTION("guard: player out of reach");
	CEntity * ePlayer = getPlayer();
	if (!playerVisible())
		return true;
	//Calc out of reach
	bool res;
	if (!ePlayer) {
		res = true;
	}
	else {
		TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
		VEC3 posPlayer = tPlayer->getPosition();
		VEC3 myPos = getTransform()->getPosition();
		float distance = squaredDistXZ(myPos, posPlayer);
		res = (distance > DIST_SQ_SHOT_AREA_ENTER);
	}

	//Update animation
	if (res) SET_ANIM_GUARD(AST_MOVE)
	else SET_ANIM_GUARD(AST_SHOOT)

		//Return calc
		return res;
}

bool bt_guard::guardAlerted() {
	PROFILE_FUNCTION("guard: guardalert");

	VEC3 myPos = getTransform()->getPosition();

	// we send a new alert with our position and the player position
	guard_alert alert;
	alert.guard_position = myPos;
	alert.timer = GUARD_ALERT_TIME;
	CEntity* entity = myHandle.getOwner();

	if (playerLost) {
		CEntity* ePlayer = getPlayer();
		if (ePlayer) {
			TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
			VEC3 posPlayer = tPlayer->getPosition();
			player_last_seen_point = posPlayer;
			// send an alert for the nearest guards with the player last position
			alert.alert_position = player_last_seen_point;
			string name = entity->getName() + string("_player_lost");
			SBB::postGuardAlert(name, alert);
		}
		return true;
	}
	else if (noiseHeard) {
		// send an alert for the nearest guards with the noise point
		alert.alert_position = noisePoint;
		string name = entity->getName() + string("_noise_heard");
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

					// depending on the alert type, we make a different decission
					if (alert_it->first.find(string("_player_lost")) != string::npos) {
						playerLost = true;
						player_last_seen_point = alert_point;
						return true;
					}
					else if (alert_it->first.find(string("_noise_heard")) != string::npos) {
						noiseHeard = true;
						noisePoint = alert_point;
						return true;
					}
					else if (alert_it->first.find(string("_player_detected")) != string::npos) {
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

//actions
int bt_guard::actionUnstuck() {
	PROFILE_FUNCTION("guard: actionunstuck");
	if (!myParent.isValid()) return false;
	setCurrent(NULL);
	stuck_time = 0.f;
	stuck = false;
}

int bt_guard::actionStunned() {
	PROFILE_FUNCTION("guard: actionstunned");
	if (!myParent.isValid()) return false;
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

int bt_guard::actionReact() {
	PROFILE_FUNCTION("guard: actionreact");
	if (!myParent.isValid()) return false;

	if (!player_detected_start) {
		// starting the reaction time decorator
		player_detected_start = true;
		reaction_time = (rand() % (int)MAX_REACTION_TIME);
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
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return STAY;
	if (!myParent.isValid()) return STAY;
	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);
	//player lost?
	if (distance > DIST_SQ_PLAYER_LOST || outJurisdiction(posPlayer)) {
		playerLost = true;
		player_last_seen_point = posPlayer;
		SET_ANIM_GUARD(AST_IDLE);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return OK;
	}
	//player near?
	else if (distance < DIST_SQ_SHOT_AREA_ENTER) {
		SET_ANIM_GUARD(AST_PREP_SHOOT);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return OK;
	}
	else {
		/*if (!isPathObtained) {
			isPathObtainedAccessible = */getPath(myPos, posPlayer);
			/*isPathObtained = true;
		}

		if (!isPathObtainedAccessible) {
			isPathObtainedAccessible = false;
			isPathObtained = false;
			return OK;
		}
		else {*/
			SET_ANIM_GUARD(AST_MOVE);
			goTo(posPlayer);
			return STAY;
		//}
	}
}

int bt_guard::actionPrepareToAbsorb() {
	PROFILE_FUNCTION("guard: prepare to absorb");
	if (!myParent.isValid()) return false;
	shoot_preparation_time += getDeltaTime();
	dbg("PREPARING TO SHOOT!\n");
	if (shoot_preparation_time > SHOOT_PREP_TIME) {
		shoot_preparation_time = 0.f;
		// calling OnGuardAttackEvent
		logic_manager->throwEvent(logic_manager->OnGuardAttack, "");
		return OK;
	}
	else {
		return STAY;
	}
}

int bt_guard::actionAbsorb() {
	PROFILE_FUNCTION("guard: absorb");
	if (!myParent.isValid()) return false;
	if (playerNear() && playerVisible()) {
		dbg("SHOOTING BACKWARDS!\n");
		SET_ANIM_GUARD(AST_SHOOT_BACK);
		goForward(-0.9f*SPEED_WALK);
		turnToPlayer();
		shootToPlayer();
		return STAY;
	}
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) {
		return STAY;
	}
	TCompTransform* tPlayer = ePlayer->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float dist = squaredDistXZ(posPlayer, getTransform()->getPosition());

	ui.addTextInstructions("\nPress 'M' to interrupt gaurd shoot when he dont see you!!! (artificial)\n");
	if (io->keys['M'].becomesPressed()) {
		artificialInterrupt();
	}

	turnTo(posPlayer);
	if (squaredDistY(myPos, posPlayer) * 2.0f > dist) { //Angulo de 30 grados
				  //Si pitch muy alto me alejo
		goForward(-SPEED_WALK);
	}
	// if we don't see the player anymore
	if (!playerVisible() && shooting) {
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
		if (squaredDistXZ(myPos, posPlayer) > DIST_SQ_PLAYER_DETECTION || !inJurisdiction(posPlayer)) {
			logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
			return KO;
		}
		else {
			return OK;
		}
	}
	else if (playerVisible()) {
		SET_ANIM_GUARD(AST_SHOOT);
		shootToPlayer();
		return STAY;
	}

	logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
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

	if (playerVisible() || boxMovingDetected()) {
		logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
		return KO;
	}
	else {
		//if a box can be removed, we remove it (next state)
		if (shootToPlayer()) {
			removeBox(box_to_remove);
			logic_manager->throwEvent(logic_manager->OnGuardRemoveBox, "");
			logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
			return OK;
		}
		else {
			if (timerShootingWall < 0) {
				playerLost = true;
				player_last_seen_point = posPlayer;
				logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
				return KO;
			}
			else {
				if (timerShootingWall > -1)
					timerShootingWall -= getDeltaTime();
				return STAY;
			}
		}
	}
}

int bt_guard::actionRemoveBox() {
	PROFILE_FUNCTION("guard: removebox");
	if (!myParent.isValid()) return false;
	// wait for the remove box animation to end
	if (removing_box_animation_time > BOX_REMOVAL_ANIM_TIME) {
		removing_box_animation_time = 0.f;
		remove_box_ready = true;
		return OK;
	}
	else {
		removing_box_animation_time += getDeltaTime();
		return STAY;
	}
}

int bt_guard::actionSearch() {
	PROFILE_FUNCTION("guard: search");
	if (!myParent.isValid()) return STAY;
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return STAY;
	VEC3 myPos = getTransform()->getPosition();

	//Player Visible?
	if (playerVisible() || boxMovingDetected()) {
		setCurrent(NULL);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return KO;
	}
	else if (playerLost) {
		float distance = simpleDistXZ(myPos, player_last_seen_point);
		/*if (!isPathObtained) {
			isPathObtainedAccessible = */getPath(myPos, player_last_seen_point);
			/*isPathObtained = true;
		}*/
		SET_ANIM_GUARD(AST_MOVE);
		goTo(player_last_seen_point);
		//Noise Point Reached ?
		if (distance < DIST_REACH_PNT) {
			playerLost = false;
			looking_around_time = LOOK_AROUND_TIME;

			TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
			VEC3 playerPos = tPlayer->getPosition();

			VEC3 dir = playerPos - myPos;
			dir.Normalize();

			search_player_point = playerPos + 1.0f * dir;
			/*isPathObtainedAccessible = false;
			isPathObtained = false;*/
			return OK;
		}
		else {
			return STAY;
		}
	}
	// If we heared a noise, we go to the point and look around
	else if (noiseHeard) {
		float distance = simpleDistXZ(myPos, noisePoint);
		/*if (!isPathObtained) {
			isPathObtainedAccessible = */getPath(myPos, noisePoint);
			/*isPathObtained = true;
		}*/
		SET_ANIM_GUARD(AST_MOVE);
		goTo(noisePoint);
		//Noise Point Reached ?
		if (distance < DIST_REACH_PNT) {
			noiseHeard = false;
			looking_around_time = LOOK_AROUND_TIME;

			VEC3 dir = noisePoint - myPos;
			dir.Normalize();

			search_player_point = noisePoint + 1.0f * dir;
			Debug->DrawLine(myPos, search_player_point);
			/*isPathObtainedAccessible = false;
			isPathObtained = false;*/
			return OK;
		}
		else {
			return STAY;
		}
	}
	// If player was lost, we simply move and look around
	else {
		looking_around_time = LOOK_AROUND_TIME;
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return OK;
	}
}

int bt_guard::actionMoveAround() {
	PROFILE_FUNCTION("guard: movearound");
	if (!myParent.isValid()) return false;

	//Player Visible?
	if (playerVisible() || boxMovingDetected()) {
		setCurrent(NULL);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return KO;
	}

	VEC3 myPos = getTransform()->getPosition();

	float distance_to_point = simpleDistXZ(myPos, search_player_point);

	// if the player is too far, we just look around
	if (distance_to_point > MAX_SEARCH_DISTANCE) {
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return OK;
	}

	if (distance_to_point > DIST_REACH_PNT) {
		/*if (!isPathObtained) {
			isPathObtainedAccessible = */getPath(myPos, search_player_point);
			/*isPathObtained = true;
		}*/

		/*if (!isPathObtainedAccessible) {
			isPathObtainedAccessible = false;
			isPathObtained = false;
			return OK;
		}
		else {*/
			SET_ANIM_GUARD(AST_MOVE);
			goTo(search_player_point);
			return STAY;
		//}
	}
	/*isPathObtainedAccessible = false;
	isPathObtained = false;*/
	return OK;
}

int bt_guard::actionLookAround() {
	PROFILE_FUNCTION("guard: lookaround");
	if (!myParent.isValid()) return false;
	//Player Visible?
	if (playerVisible() || boxMovingDetected()) {
		setCurrent(NULL);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return KO;
	}
	// Turn arround
	else if (deltaYawLookingArround < 2 * M_PI && looking_around_time > 0.f) {
		SET_ANIM_GUARD(AST_TURN);
		float yaw, pitch;
		getTransform()->getAngles(&yaw, &pitch);

		float deltaYaw = SPEED_ROT * getDeltaTime();
		deltaYawLookingArround += deltaYaw;
		yaw += deltaYaw;
		getTransform()->setAngles(yaw, pitch);

		looking_around_time -= getDeltaTime();

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
	if (playerVisible() || boxMovingDetected()) {
		setCurrent(NULL);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return KO;
	}
	//Go to waypoint
	else if (keyPoints[curkpt].type == Seek) {
		//reach waypoint?
		if (simpleDistXZ(myPos, dest) < DIST_REACH_PNT) {
			curkpt = (curkpt + 1) % keyPoints.size();
			/*isPathObtainedAccessible = false;
			isPathObtained = false;*/
			return OK;
		}
		else {
			/*if (!isPathObtained) {
				isPathObtainedAccessible = */getPath(myPos, dest);
				/*isPathObtained = true;
			}*/
			/*if (!isPathObtainedAccessible) {
				isPathObtainedAccessible = false;
				isPathObtained = false;
				return OK;
			}
			else {*/
				SET_ANIM_GUARD(AST_MOVE);
				goTo(dest);
				return STAY;
			//}
		}
	}
	//Look to waypoint
	else if (keyPoints[curkpt].type == Look) {
		//Look to waypoint
		if (turnTo(dest)) {
			curkpt = (curkpt + 1) % keyPoints.size();
			/*isPathObtainedAccessible = false;
			isPathObtained = false;*/
			return OK;
		}
		else {
			return STAY;
		}
	}
	/*isPathObtainedAccessible = false;
	isPathObtained = false;*/
	return OK;
}

int bt_guard::actionNextWpt() {
	PROFILE_FUNCTION("guard: actionnextwpt");
	if (!myParent.isValid()) return false;
	if (keyPoints.size() == 0) return false;
	SET_ANIM_GUARD(AST_TURN);
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = keyPoints[curkpt].pos;
	//Player Visible?
	if (playerVisible() || boxMovingDetected()) {
		setCurrent(NULL);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
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

	//player visible?
	if (playerVisible() || boxMovingDetected()) {
		setCurrent(NULL);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
		return KO;
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

// toggle actions
int bt_guard::actionGoToFormation() {
	PROFILE_FUNCTION("guard: gotoformation");
	if (!myParent.isValid()) return false;

	VEC3 myPos = getTransform()->getPosition();

	float distance_to_point = simpleDistXZ(myPos, formation_point);

	// if we didn't reach the point
	if (distance_to_point > DIST_REACH_PNT) {
		/*if (!isPathObtained) {
			isPathObtainedAccessible = */getPath(myPos, formation_point);
			/*isPathObtained = true;
		}*/
		SET_ANIM_GUARD(AST_MOVE);
		goTo(formation_point);
		return STAY;
	}
	/*isPathObtainedAccessible = false;
	isPathObtained = false;*/

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
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/
	}
}

void bt_guard::onMagneticBomb(const TMsgMagneticBomb & msg)
{
	PROFILE_FUNCTION("guard: onmagneticbomb");
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 myPos = getTransform()->getPosition();
	if (inSquaredRangeXZ_Y(msg.pos, myPos, msg.r, 5.f)) {
		resetTimers();
		stunned = true;
		SET_ANIM_GUARD(AST_STUNNED);
		checkStopDamage();
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/

		setCurrent(NULL);
	}
	//VEC3 myPos = getTransform()->getPosition();
	//float d = squaredDist(msg.pos, myPos);

	//if (d < msg.r) {
	//  reduceStats();
	//  t_reduceStats = t_reduceStats_max;
	//}
}

void bt_guard::onStaticBomb(const TMsgStaticBomb& msg) {
	PROFILE_FUNCTION("guard: onstaticbomb");
	VEC3 myPos = getTransform()->getPosition();
	if (squaredDist(msg.pos, myPos) < msg.r * msg.r) {
		resetTimers();
		stunned = true;
		SET_ANIM_GUARD(AST_STUNNED);
		checkStopDamage();
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/

		setCurrent(NULL);
	}
}

void bt_guard::onOverCharged(const TMsgOverCharge& msg) {
	PROFILE_FUNCTION("guard: onovercharge");
	CEntity * entity = myHandle.getOwner();
	string guard_name = entity->getName();

	if (msg.guard_name == guard_name) {
		logic_manager->throwEvent(logic_manager->OnGuardOvercharged, "");
		stunned = true;
		____TIMER_RESET_(timerStunt);
		/*isPathObtainedAccessible = false;
		isPathObtained = false;*/

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
	/*isPathObtainedAccessible = false;
	isPathObtained = false;*/
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

// -- Go To -- //
void bt_guard::goTo(const VEC3& dest) {
	PROFILE_FUNCTION("guard: go to");
	if (!SBB::readBool("navmesh")) {
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
	/*else if (totalPathWpt == 0) {
	  TCompTransform * player_transform = getPlayer()->get<TCompTransform>();
	  VEC3 posPlayer = player_transform->getPosition();
	  VEC3 myPos = getTransform()->getPosition();

	  VEC3 dir = getTransform()->getPosition() - myPos;
	  dir.Normalize();

	  std::vector<VEC3> candidates;
	  bool trobat = false;

	  target = posPlayer + 2.0f * dir;
	  candidates.push_back(target);
	  target = posPlayer - 2.0f * dir;
	  candidates.push_back(target);

	  dir = player_transform->getLeft();
	  target = posPlayer + 2.0f * dir;
	  candidates.push_back(target);
	  target = posPlayer - 2.0f * dir;
	  candidates.push_back(target);

	  for (auto cand : candidates) {
		bool accesible = getPath(myPos, cand);
		if (!accesible)
		  continue;
		else {
		  target = cand;
		  trobat = true;
		  break;
		}
	  }

	  if (!trobat) {
	  isPathObtainedAccessible = false;
	  isPathObtained = false;
	  setCurrent(NULL);
		return;
	  }
	}*/

	if (needsSteering(npcPos, getTransform(), SPEED_WALK, myParent)) {
		goForward(SPEED_WALK);
	}
	else if (!getTransform()->isHalfConeVision(target, deg2rad(5.0f))) {
		turnTo(target);
	}
	else {
		float distToWPT = simpleDistXZ(target, getTransform()->getPosition());
		if (fabsf(distToWPT) > 0.1f && currPathWpt < totalPathWpt || fabsf(distToWPT) > 0.2f) {
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
	float angle_epsilon = deg2rad(5);

	if (abs(deltaYaw) < angle_epsilon || abs(deltaYaw) > deg2rad(355))
		return true;

	if (deltaYaw > 0) {
		if (deltaAngle < deltaYaw) yaw += deltaAngle;
		else yaw += deltaYaw;
	}
	else {
		if (deltaAngle < abs(deltaYaw)) yaw -= deltaAngle;
		else yaw += deltaYaw;
	}

	if (!getTransform()->isHalfConeVision(dest, deg2rad(5.0f))) {
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
	return abs(deltaYaw) < angle_epsilon || abs(deltaYaw) > deg2rad(355);
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

//THIS IS NOT USED!
//VEC3 bt_guard::generateRandomPoint() {
//	PROFILE_FUNCTION("guard: generate random point");
//	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
//	VEC3 myPos = tPlayer->getPosition();
//
//	// generate random increments for x and z coords
//	float x_diff = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_POINT_MAX_DISTANCE));
//	float z_diff = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_POINT_MAX_DISTANCE));
//
//	// randomly decide x sign
//	if (rand() % 10 < 5) {
//		myPos.x += x_diff;
//	}
//	else {
//		myPos.x -= x_diff;
//	}
//
//	// randomly decide z sign
//	if (rand() % 10 < 5) {
//		myPos.z += z_diff;
//	}
//	else {
//		myPos.z -= z_diff;
//	}
//
//	return myPos;
//}

bool bt_guard::playerTooNear() {
	if (!myParent.isValid()) return false;
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();

	return simpleDist(posPlayer, myPos) < PLAYER_DETECTION_RADIUS;
}

// -- Player Visible? -- //
bool bt_guard::playerVisible() {
	if (!myParent.isValid()) return false;
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	if (SBB::readBool("possMode") && squaredDistXZ(myPos, posPlayer) > 25.f) {
		return false;
	}

	if (simpleDist(posPlayer, myPos) < PLAYER_DETECTION_RADIUS) {
		float distRay;
		PxRaycastBuffer hit;
		bool ret = rayCastToPlayer(1, distRay, hit);
		if (ret) { //No bloquea vision
			CHandle h = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (h.hasTag("player")) { //player?
				return true;
			}
		}
	}

	float distancia_vertical = squaredDistY(posPlayer, myPos);

	if (distancia_vertical < squaredDistXZ(posPlayer, myPos) * 0.5f) { //Pitch < 30
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

bool bt_guard::boxMovingDetected() {
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
	Debug->DrawLine(origin + direction*(cc->GetRadius() + 0.05f), getTransform()->getFront(), 10.0f);
	bool ret = g_PhysxManager->raycast(origin + direction*(cc->GetRadius() + 0.05f), direction, dist, hit);

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

bool bt_guard::shootToPlayer() {
	//If cant shoot returns
	if (noShoot) return false;
	CEntity* ePlayer = getPlayer();
	if (!ePlayer) return false;

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
		if (ret) {
			CHandle h = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (h.hasTag("player")) {
				damage = true;
				raijin = h.hasTag("raijin");
			}
			else if (h.hasTag("box")) {
				CEntity* box = h;
				TCompBox* box_component = box->get<TCompBox>();
				if (box_component && box_component->isRemovable()) {
					// if remove box is ready, reset the timer and remove the box
					if (remove_box_ready) {
						remove_box_time = MAX_BOX_REMOVAL_TIME;
						remove_box_ready = false;
						box_to_remove = h;
						return true;
					}
					// if not, just update the timer
					else {
						remove_box_time -= getDeltaTime();
						if (remove_box_time <= 0.f)
							remove_box_ready = true;
					}
				}
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

	//Render Shot
	drawShot(distRay);

	return false;
}

void bt_guard::drawShot(float distRay) {
	PROFILE_FUNCTION("guard bt: draw shot");
	CEntity * ePlayer = getPlayer();
	if (!ePlayer) return;
	// Centro del personaje
	TCompCharacterController * cc = ePlayer->get<TCompCharacterController>();
	TCompTransform *t = ePlayer->get<TCompTransform>();
	assert(cc || fatal("Player doesn't have character controller!"));
	VEC3 posPlayer = cc->GetPosition();

	// Origin and rayshot
	VEC4 originShot4;
	VEC4::Transform(SHOT_OFFSET, getTransform()->asMatrix(), originShot4);
	VEC3 originShot = VEC3(originShot4.x, originShot4.y, originShot4.z);
	originShot += VEC3(0.0f, -0.32f, 0.0f);
	originShot += 0.15f*getTransform()->getLeft();
	originShot += 0.15f*getTransform()->getFront();
	VEC3 destShot = posPlayer; //algun offset?
	VEC3 rayShot = destShot - originShot;

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
	/*float distanceJur = squaredDistXZ(jurCenter, posPlayer);
	return distanceJur > jurRadiusSq + DIST_SQ_SHOT_AREA_ENTER;*/
	return false;
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
	ImGui::SliderFloat3("Offset Starting Shot", &SHOT_OFFSET.x, 0.f, 2.f);
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

//Cambio de malla
//void bt_guard::ChangePose(string new_pose_route) {
//	PROFILE_FUNCTION("guard bt: change pose");
//	if (last_pose != new_pose_route) {
//		mesh->unregisterFromRender();
//		MKeyValue atts_mesh;
//		atts_mesh["name"] = new_pose_route;
//		mesh->load(atts_mesh);
//		mesh->registerToRender();
//		last_pose = new_pose_route;
//	}
//}

//TODO: remove
void bt_guard::artificialInterrupt()
{
	TCompTransform *t = getTransform();
	float yaw, pitch;
	t->getAngles(&yaw, &pitch);
	t->setAngles(-yaw, pitch);
	setCurrent(NULL);
	/*isPathObtainedAccessible = false;
	isPathObtained = false;*/
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
		/*if (!isPathObtained) {
			isPathObtained = true;*/
			getPath(getTransform()->getPosition(), dest);
		//}
		goTo(dest);
	}

	SET_ANIM_GUARD(AST_IDLE);
	forced_move = false;
}