#include "mcv_platform.h"
#include <windows.h>
#include "ai_guard.h"
#include "components/entity_tags.h"
#include "utils/XMLParser.h"
#include "physics/physics.h"
#include "logic/sbb.h"
#include "app_modules\io\io.h"
#include "ui\ui_interface.h"

map<string, ai_guard::KptType> ai_guard::kptTypes = {
	  {"seek", KptType::Seek}
	, {"look", KptType::Look}
};

TCompTransform * ai_guard::getTransform() {
	CEntity * e = myParent;
	TCompTransform * t = e->get<TCompTransform>();
	return t;
}

CEntity* ai_guard::getPlayer() {
	VHandles targets = tags_manager.getHandlesByTag(getID("target"));
	thePlayer = targets[targets.size() - 1];
	CEntity* player = thePlayer;
	return player;
}

/**************
* Init
**************/
void ai_guard::Init()
{
	//Handles
	myHandle = CHandle(this);
	myParent = myHandle.getOwner();
	thePlayer = tags_manager.getFirstHavingTag(getID("target"));

	// insert all states in the map
	AddState(ST_SELECT_ACTION, (statehandler)&ai_guard::SelectActionState);
	AddState(ST_NEXT_ACTION, (statehandler)&ai_guard::NextActionState);
	AddState(ST_SEEK_POINT, (statehandler)&ai_guard::SeekPointState);
	AddState(ST_WAIT_NEXT, (statehandler)&ai_guard::WaitNextState);
	AddState(ST_CHASE, (statehandler)&ai_guard::ChaseState);
	AddState(ST_LOOK_POINT, (statehandler)&ai_guard::LookPointState);
	AddState(ST_SHOOT, (statehandler)&ai_guard::ShootState);
	AddState(ST_SOUND_DETECTED, (statehandler)&ai_guard::SoundDetectedState);
	AddState(ST_LOOK_ARROUND, (statehandler)&ai_guard::LookArroundState);
	AddState(ST_SHOOTING_WALL, (statehandler)&ai_guard::ShootingWallState);

	// reset the state
	ChangeState(ST_SELECT_ACTION);
	curkpt = 0;

	//Other info
	____TIMER_REDEFINE_(timerShootingWall, 3);
	____TIMER_REDEFINE_(timerStunt, 3);
	timeWaiting = 0;
	deltaYawLookingArround = 0;
	stunned = false;

	//Mallas
	pose_run = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_shoot = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	CEntity* myEntity = myParent;
	pose_idle = myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	actual_render = pose_idle;

	pose_idle.setOwner(myEntity);
	pose_run.setOwner(myEntity);
	pose_shoot.setOwner(myEntity);

	TCompRenderStaticMesh *mesh;

	mesh = pose_shoot;
	mesh->static_mesh = Resources.get("static_meshes/guard_shoot.static_mesh")->as<CStaticMesh>();

	mesh = pose_run;
	mesh->static_mesh = Resources.get("static_meshes/guard_run.static_mesh")->as<CStaticMesh>();

	actual_render->registerToRender();
}

/**************
* Select Action
**************/
void ai_guard::SelectActionState() {
	ChangePose(pose_idle);
	switch (keyPoints[curkpt].type)
	{
	case Seek:
		ChangeState(ST_SEEK_POINT);
		break;
	case Look:
		ChangeState(ST_LOOK_POINT);
		break;
	}
}

/**************
* NextAction State
**************/
void ai_guard::NextActionState() {
	curkpt = (curkpt + 1) % keyPoints.size();
	ChangeState(ST_SELECT_ACTION);
}

/**************
* SeekPoint State
**************/
void ai_guard::SeekPointState() {
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = keyPoints[curkpt].pos;

	//player visible?
	if (playerVisible()) {
		ChangeState(ST_CHASE);
	}

	//reach waypoint?
	else if (squaredDistXZ(myPos, dest) < DIST_SQ_REACH_PNT) {
		ChangeState(ST_WAIT_NEXT);
	}

	//Go to waypoint
	else {
		ChangePose(pose_run);
		goTo(dest);
	}
}

/**************
* Look Point State
**************/
void ai_guard::LookPointState() {
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = keyPoints[curkpt].pos;

	//player visible?
	if (playerVisible()) {
		ChangeState(ST_CHASE);
	}

	//Look to waypoint
	else if (turnTo(dest)) {
		ChangeState(ST_WAIT_NEXT);
	}
}

/**************
* WaitNext State
**************/
void ai_guard::WaitNextState() {
	ChangePose(pose_idle);

	//player visible?
	if (playerVisible()) {
		ChangeState(ST_CHASE);
	}

	if (timeWaiting > keyPoints[curkpt].time) {
		timeWaiting = 0;
		ChangeState(ST_NEXT_ACTION);
	}
	else {
		timeWaiting += getDeltaTime();
	}
}

/**************
* Chase State
**************/
void ai_guard::ChaseState()
{
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, posPlayer);
	float distanceJur = squaredDistXZ(jurCenter, posPlayer);
	bool playerLost = distance > DIST_SQ_PLAYER_LOST || outJurisdiction(posPlayer);
	//bool playerLost = distance > DIST_SQ_PLAYER_LOST || distanceJur > jurRadiusSq + DIST_SQ_SHOT_AREA_ENTER;
	//player lost?
	if (playerLost) {
		ChangeState(ST_NEXT_ACTION);
	}

	//player near?
	else if (distance < DIST_SQ_SHOT_AREA_ENTER) {
		ChangeState(ST_SHOOT);
	}

	else {
		ChangePose(pose_run);
		goTo(posPlayer);
	}
}

/**************
* ShootState
**************/
void ai_guard::ShootState() {
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float dist = squaredDistXZ(posPlayer, getTransform()->getPosition());

	//Fuera de tiro
	if (dist > DIST_SQ_SHOT_AREA_LEAVE) {
		ChangeState(ST_CHASE);
	}
	else {
		turnTo(posPlayer);
		if (squaredDistY(myPos, posPlayer) * 2 > dist) { //Angulo de 30 grados
			//Si pitch muy alto me alejo
			goForward(-SPEED_WALK * getDeltaTime());
		}
		if (!playerVisible()) {
			ChangeState(ST_SHOOTING_WALL);
		}
		else {
			ChangePose(pose_shoot);
			shootToPlayer();
		}
	}

	ui.addTextInstructions("\nPress 'M' to interrupt gaurd shoot when he dont see you!!! (artificial)\n");
	if (io->keys['M'].becomesPressed()) {
		artificialInterrupt();
	}
}

/**************
* Shooting Wall
**************/
void ai_guard::ShootingWallState() {
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	turnTo(posPlayer);
	if (playerVisible()) {
		ChangeState(ST_SHOOT);
	}
	else {
		shootToPlayer();
		____TIMER_CHECK_DO_(timerShootingWall);
		ChangeState(ST_SELECT_ACTION);
		____TIMER_CHECK_DONE_(timerShootingWall);
	}
}

/**************
* Sound Detected
**************/
void ai_guard::SoundDetectedState() {
	VEC3 myPos = getTransform()->getPosition();
	float distance = squaredDistXZ(myPos, noisePoint);

	//Player Visible?
	if (playerVisible()) {
		ChangeState(ST_CHASE);
	}

	//Noise Point Reached ?
	else if (distance < DIST_SQ_REACH_PNT) {
		ChangeState(ST_LOOK_ARROUND);
	}

	else goTo(noisePoint);
}

/**************
* LookArround
**************/
void ai_guard::LookArroundState() {
	//Player Visible?
	if (playerVisible()) {
		ChangeState(ST_CHASE);
	}

	//Turn arround
	else if (deltaYawLookingArround < 2 * M_PI) {
		ChangePose(pose_idle);
		float yaw, pitch;
		getTransform()->getAngles(&yaw, &pitch);

		float deltaYaw = SPEED_ROT * getDeltaTime();
		deltaYawLookingArround += deltaYaw;
		yaw += deltaYaw;
		getTransform()->setAngles(yaw, pitch);
	}
	else {
		deltaYawLookingArround = 0;
		ChangeState(ST_SELECT_ACTION);
	}
}

void ai_guard::StuntState() {
	____TIMER_CHECK_DO_(timerStunt);
	ChangeState(ST_SELECT_ACTION);
	stunned = false;
	____TIMER_CHECK_DONE_(timerStunt);
}

/**************
* Mensajes
**************/
void ai_guard::noise(const TMsgNoise& msg) {
	if (outJurisdiction(msg.source)) return;
	if (playerVisible()) return;
	if (stunned) return;
	if (canHear(msg.source, msg.intensity)) {
		resetTimers();
		noisePoint = msg.source;
		ChangeState(ST_SOUND_DETECTED);
	}
}

void ai_guard::onMagneticBomb(const TMsgMagneticBomb & msg)
{
	VEC3 myPos = getTransform()->getPosition();
	float d = squaredDist(msg.pos, myPos);

	if (d < msg.r) {
		reduceStats();
		t_reduceStats = t_reduceStats_max;
	}
}

void ai_guard::onStaticBomb(const TMsgStaticBomb& msg) {
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	if (squaredDist(msg.pos, posPlayer) < msg.r * msg.r) {
		resetTimers();
		stunned = true;
		ChangeState("ST_STUNT");
	}
}

/**************
 * Auxiliares
 **************/

 // -- Go To -- //
bool ai_guard::canHear(VEC3 position, float intensity) {
	return (squaredDistXZ(getTransform()->getPosition(), position) > DIST_SQ_SOUND_DETECTION);
}

// -- Go To -- //
void ai_guard::goTo(const VEC3& dest) {
	//avanzar
	goForward(SPEED_WALK * getDeltaTime());

	//girar
	turnTo(dest);
}

// -- Go Forward -- //
void ai_guard::goForward(float stepForward) {
	VEC3 myPos = getTransform()->getPosition();
	getTransform()->setPosition(myPos + getTransform()->getFront() * stepForward);
}

// -- Turn To -- //
bool ai_guard::turnTo(VEC3 dest) {
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
bool ai_guard::playerVisible() {
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
					ray_cast_query rcQuery;
					float distRay;
					if (SBB::readBool("possMode")) {
						// Estas poseyendo, estas cerca y dentro del cono de vision, no hace falta raycast
						return true;
					}
					else {
						CHandle collider = rayCastToPlayer(COL_TAG_PLAYER | COL_TAG_SOLID, distRay);
						if (collider.isValid()) { //No bloquea vision
							if (collider == thePlayer) {
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

CHandle ai_guard::rayCastToPlayer(int types, float& distRay) {
	ray_cast_query rcQuery;
	VEC3 myPos = getTransform()->getPosition();
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	rcQuery.position = myPos + VEC3(0, PLAYER_CENTER_Y, 0);
	rcQuery.direction = tPlayer->getPosition() - myPos;
	rcQuery.maxDistance = DIST_RAYSHOT;
	rcQuery.types = types;
	ray_cast_result res = Physics::calcRayCast(rcQuery);
	distRay = realDist(res.positionCollision, getTransform()->getPosition());
	return res.firstCollider;
}

void ai_guard::shootToPlayer() {
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
		//RayCast
		CHandle collider = rayCastToPlayer(COL_TAG_PLAYER | COL_TAG_SOLID, distRay);
		if (collider == CHandle(getPlayer())) {
			damage = true;
		}
	}

	//Do damage
	if (damage) {
		CEntity* ePlayer = getPlayer();
		TMsgDamage dmg;
		dmg.source = getTransform()->getPosition();
		dmg.sender = myParent;
		dmg.points = DAMAGE_LASER * getDeltaTime();
		dmg.dmgType = LASER;
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
bool ai_guard::inJurisdiction(VEC3 posPlayer) {
	float distanceJur = squaredDistXZ(jurCenter, posPlayer);
	return distanceJur < jurRadiusSq;
}

bool ai_guard::outJurisdiction(VEC3 posPlayer) {
	float distanceJur = squaredDistXZ(jurCenter, posPlayer);
	return distanceJur > jurRadiusSq + DIST_SQ_SHOT_AREA_ENTER;
}

// -- Reset Timers-- //
void ai_guard::resetTimers() {
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

bool ai_guard::load(MKeyValue& atts) {
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
			, atts.getInt(atrWait, 0)
			);
	}
	noShoot = atts.getBool("noShoot", false);

	//Jurisdiction
	jurCenter = atts.getPoint("jurisdiction");
	jurRadiusSq = atts.getFloat("jurRadius", 1000.0f);
	if (jurRadiusSq < FLT_MAX) jurRadiusSq *= jurRadiusSq;

	return true;
}

void ai_guard::render() {
}

void ai_guard::renderInMenu() {
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

void ai_guard::reduceStats()
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

void ai_guard::resetStats()
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
void ai_guard::ChangePose(CHandle new_pos_h)
{
	TCompRenderStaticMesh *new_pose = new_pos_h;
	if (new_pose == actual_render) return;
	actual_render->unregisterFromRender();
	actual_render = new_pose;
	actual_render->registerToRender();
}

//TODO: remove
void ai_guard::artificialInterrupt()
{
	TCompTransform *t = getTransform();
	float yaw, pitch;
	t->getAngles(&yaw, &pitch);
	t->setAngles(-yaw, pitch);
	ChangeState(ST_WAIT_NEXT);
}