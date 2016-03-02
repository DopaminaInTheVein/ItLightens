#include "mcv_platform.h"
#include <windows.h>
#include "ai_guard.h"
#include "components/entity_tags.h"
#include "utils/XMLParser.h"
#include "physics/physics.h"
#include "logic/sbb.h"

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
	CEntity* player = thePlayer;
	return player;;
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
	CHandle prueba = tags_manager.getFirstHavingTag(getID("target"));
	if (prueba == thePlayer) dbg("Son iguales!");

	// insert all states in the map
	AddState(ST_NEXT_ACTION, (statehandler)&ai_guard::NextActionState);
	AddState(ST_SEEK_POINT, (statehandler)&ai_guard::SeekPointState);
	AddState(ST_WAIT_NEXT, (statehandler)&ai_guard::WaitNextState);
	AddState(ST_CHASE, (statehandler)&ai_guard::ChaseState);
	AddState(ST_LOOK_POINT, (statehandler)&ai_guard::LookPointState);
	AddState(ST_SHOOT, (statehandler)&ai_guard::ShootState);
	AddState(ST_SOUND_DETECTED, (statehandler)&ai_guard::SoundDetectedState);
	AddState(ST_LOOK_ARROUND, (statehandler)&ai_guard::LookArroundState);

	// reset the state
	ChangeState(ST_NEXT_ACTION);
	curkpt = -1; //Para que el primero en acceder sea el índice 0

	//Other info
	timeWaiting = 0;
	deltaYawLookingArround = 0;
}

/**************
* NextAction State
**************/
void ai_guard::NextActionState() {
	curkpt = (curkpt + 1) % keyPoints.size();
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

	if (!playerVisible()) {
		ChangeState(ST_NEXT_ACTION);
	}
	//player lost?
	//if (distance > DIST_SQ_PLAYER_LOST) {
	//	ChangeState(ST_NEXT_ACTION);
	//}

	//player near?
	else if (distance < DIST_SQ_SHOT_AREA_ENTER) {
		ChangeState(ST_SHOOT);
	}

	else goTo(posPlayer);
}

/**************
* ShootState
**************/
void ai_guard::ShootState() {
	if (!playerVisible()) {
		ChangeState(ST_NEXT_ACTION);
		return;
	}

	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	float dist = squaredDistXZ(posPlayer, getTransform()->getPosition());

	//RayCast
	float distRay;
	CHandle collider = rayCastToFront(COL_TAG_PLAYER | COL_TAG_OBJECT, distRay);
	if (collider == thePlayer) {
		CEntity* ePlayer = thePlayer;
		TMsgDamage dmg;
		dmg.source = getTransform()->getPosition();
		dmg.sender = myParent;
		dmg.points = DAMAGE_LASER * getDeltaTime();
		dmg.dmgType = DMGTYPE::LASER;
		ePlayer->sendMsg(dmg);
	}

	//Render Debug
	for (int i = 0; i < 8; i++) {
		float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		Debug->DrawLine(myPos + VEC3(r1, 1, r2), getTransform()->getFront(), distRay, RED);
	}

	//Fuera de tiro?
	if (dist > DIST_SQ_SHOT_AREA_LEAVE) ChangeState(ST_CHASE);
	else turnTo(posPlayer);
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
		float yaw, pitch;
		getTransform()->getAngles(&yaw, &pitch);

		float deltaYaw = SPEED_ROT * getDeltaTime();
		deltaYawLookingArround += deltaYaw;
		yaw += deltaYaw;
		getTransform()->setAngles(yaw, pitch);
	}
	else {
		deltaYawLookingArround = 0;
		ChangeState(ST_NEXT_ACTION);
	}
}

/**************
* Mensajes
**************/
void ai_guard::noise(const TMsgNoise& msg) {
	if (!playerVisible()) {
		resetTimes();
		noisePoint = msg.source;
		ChangeState(ST_SOUND_DETECTED);
	}
}

/**************
 * Auxiliares
 **************/
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
	if (SBB::readBool("possMode")) return false;
	TCompTransform* tPlayer = getPlayer()->get<TCompTransform>();
	VEC3 posPlayer = tPlayer->getPosition();
	VEC3 myPos = getTransform()->getPosition();
	if (getTransform()->isHalfConeVision(posPlayer, CONE_VISION)) {
		if (squaredDistXZ(myPos, posPlayer) < DIST_SQ_PLAYER_DETECTION) {
			return true;
		}
	}
	ray_cast_query rcQuery;
	float distRay;
	CHandle collider = rayCastToFront(COL_TAG_OBJECT, distRay);
	if (!collider.isValid()) { //No bloquea vision
		____TIMER_CHECK_DO_(timerDebug);
		dbg("ai_guard: Detecto al player\n");
		____TIMER_CHECK_DONE_(timerDebug);
		return true;
	}
	return false;
}

CHandle ai_guard::rayCastToFront(char types, float& distRay) {
	ray_cast_query rcQuery;
	rcQuery.position = getTransform()->getPosition();
	rcQuery.direction = getTransform()->getFront();
	rcQuery.maxDistance = DIST_RAYSHOT;
	rcQuery.types = types;
	ray_cast_result res = Physics::calcRayCast(rcQuery);
	distRay = realDist(res.positionCollision, getTransform()->getPosition());
	return res.firstCollider;
}

// -- Reset Times-- //
void ai_guard::resetTimes() {
	timeWaiting = 0;
	deltaYawLookingArround = 0;
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
	return true;
}

void ai_guard::render() {
}

void ai_guard::renderInMenu() {
	ImGui::SliderFloat("Speed Walk", &SPEED_WALK, 0, 1);
	ImGui::SliderFloat("Speed Rot (rad/s)", &SPEED_ROT, 0, 2 * M_PI);
	ImGui::SliderFloat("Cone Vision 1/2 (rads)", &CONE_VISION, 0, 180);
	ImGui::SliderFloat("Distance Reach", &DIST_SQ_REACH_PNT, 0, 1);
	ImGui::SliderFloat("Detection Area", &DIST_SQ_PLAYER_DETECTION, 0, 500);
	ImGui::SliderFloat("Shot Area Enter", &DIST_SQ_SHOT_AREA_ENTER, 0, 500);
	ImGui::SliderFloat("Shot Area Leave", &DIST_SQ_SHOT_AREA_LEAVE, 0, 500);
	ImGui::SliderFloat("Lost Player Distance", &DIST_SQ_PLAYER_LOST, 0, 500);
	ImGui::SliderFloat("Laser Shot Reach", &DIST_RAYSHOT, DIST_SQ_SHOT_AREA_ENTER, DIST_SQ_SHOT_AREA_LEAVE * 2);
	ImGui::SliderFloat("Laser Damage", &DAMAGE_LASER, 0, 10);
}