#include "mcv_platform.h"
#include "npc.h"

#include "recast/navmesh.h"
#include "recast/navmesh_query.h"
#include "recast/DebugUtils/Include/DebugDraw.h"

void npc::readNpcIni(std::map<std::string, float>& fields)
{
	assignValueToVar(DIST_REACH_PNT, fields);
	assignValueToVar(SPEED_WALK, fields);
	assignValueToVar(SPEED_ROT, fields);
	SPEED_ROT = deg2rad(SPEED_ROT);
	assignValueToVar(MAX_STUCK_TIME, fields);
	assignValueToVar(UNSTUCK_DISTANCE, fields);
}

void npc::addNpcStates(std::string parent)
{
	addChild(parent, "stucksequence", SEQUENCE, (btcondition)&npc::npcStuck, NULL);
	addChild("stucksequence", "unstuckturn", ACTION, NULL, (btaction)&npc::actionUnstuckTurn);
	addChild("stucksequence", "unstuckmove", ACTION, NULL, (btaction)&npc::actionUnstuckMove);
}

bool npc::getPath(const VEC3& startPoint, const VEC3& endPoint) {
	PROFILE_FUNCTION("bt Get Path");
	CNavmesh nav = SBB::readNavmesh();
	CNavmeshQuery query(&nav);

	//No permite const!
	VEC3 startPointValue = startPoint;
	VEC3 endPointValue = endPoint;
	//TODO: Arreglar esto. Dar punto mas cercano!
	if (abs(startPointValue.y - endPointValue.y) < 5.f) {
		endPointValue.y = startPointValue.y;
	}
	query.updatePosIni(startPointValue);
	query.updatePosEnd(endPointValue);
	//Debug->DrawLine(startPointValue, endPointValue);
	query.findPath(query.p1, query.p2);
	const float * path = query.getVertexSmoothPath();
	pathWpts.clear();
	totalPathWpt = query.getNumVertexSmoothPath();
	if (totalPathWpt > 0) {
		for (int i = 0; i < totalPathWpt * 3; i = i + 3) {
			pathWpts.push_back(VEC3(path[i], path[i + 1], path[i + 2]));
		}
	}
	currPathWpt = 0;
	if (totalPathWpt < 1)
		return false;

	for (int i = 0; i < (pathWpts.size() - 1); i++) {
		//Debug->DrawLine(pathWpts[i], pathWpts[i + 1], VEC3(1, 0, 1));
	}

	currPathWpt = 0;

	return true;
}

CEntity* npc::frontCollisionIA(const VEC3 & npcPos, CHandle ownHandle) {
	CEntity * ownHandleE = ownHandle;
	TCompTransform * candidateOwn = ownHandleE->get<TCompTransform>();
	VEC3 npcFront = candidateOwn->getFront();
	npcFront.Normalize();
	TTagID tagIDia = getID("AI");
	vector<CHandle> colCandidates = tags_manager.getHandlesByTag(tagIDia);
	for (CHandle candidateH : colCandidates) {
		if (candidateH != ownHandle) {
			CEntity * candidateE = candidateH;
			TCompTransform * candidateT = candidateE->get<TCompTransform>();
			VEC3 candidateFront = candidateT->getFront();
			candidateFront.Normalize();
			VEC3 directionDiference = candidateFront - npcFront;
			bool sameDir = (directionDiference.x + directionDiference.y + directionDiference.z) < 0.1;
			if (realDist(npcPos, candidateT->getPosition()) < 1.5f && !sameDir && !candidateT->rotating) {
				candidateOwn->rotating = true;
				return candidateE;
			}
		}
	}
	candidateOwn->rotating = false;
	return nullptr;
}

CEntity* npc::frontCollisionBOX(const TCompTransform * transform, CEntity *  molePursuingBoxi) {
	TTagID tagIDbox = getID("box");
	VEC3 npcPos = transform->getPosition();
	vector<CHandle> colCandidates = tags_manager.getHandlesByTag(tagIDbox);//SBB::readHandlesVector("collisionables");
	for (CHandle candidateH : colCandidates) {
		if (!candidateH.isValid()) continue;
		CEntity * candidateE = candidateH;
		TCompTransform * candidateT = candidateE->get<TCompTransform>();
		VEC3 colPos = candidateT->getPosition();
		if ((molePursuingBoxi == nullptr || molePursuingBoxi != candidateE) && realDist(npcPos, colPos) < 1.5f) {
			return candidateE;
		}
	}
	return nullptr;
}
bool npc::avoidBoxByLeft(CEntity * candidateE, const TCompTransform * transform) {
	VEC3 npcPos = transform->getPosition();
	VEC3 dir = transform->getLeft();
	dir.Normalize();
	PxRaycastBuffer hit;
	bool ret = g_PhysxManager->raycast(npcPos, dir, 10.f, hit);
	if (ret) {
		return true;
	}
	return false;
}
bool npc::needsSteering(VEC3 npcPos, TCompTransform * transform, float rotation_speed, CHandle myHandle, CEntity *  molePursuingBoxi) {
	float yaw, pitch, delta_yaw = 0.25f;
	transform->getAngles(&yaw, &pitch);
	CEntity * collisionBOX = frontCollisionBOX(transform, molePursuingBoxi);
	if (frontCollisionIA(npcPos, myHandle) != nullptr) {
		transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
		return true;
	}
	else if (collisionBOX != nullptr && avoidBoxByLeft(collisionBOX, transform)) {
		transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
		return true;
	}
	else if (collisionBOX != nullptr) {
		delta_yaw = -0.25f;
		transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
		return true;
	}
	return false;
}

void npc::updateStuck()
{
	float distance = simpleDistXZ(last_position, getTransform()->getPosition());
	if (distance <= 0.75f*getDeltaTime()*SPEED_WALK) {
		stuck_time += getDeltaTime();
		if (stuck_time > MAX_STUCK_TIME && !stuck) {
			stuck = true;
			path_found = false;
			action_when_stuck = current;
			setCurrent(NULL);
		}
	}
	else {
		stuck_time = 0.f;
	}

	last_position = getTransform()->getPosition();
}

bool npc::npcStuck() {
	PROFILE_FUNCTION("npc: guard stuck");
	return stuck;
}

//actions
int npc::actionUnstuckTurn() {
	PROFILE_FUNCTION("npc: actionunstuckturn");
	// turn to get unstuck
	//SET_ANIM_GUARD(AST_IDLE);
	changeCommonState(AST_IDLE);
	if (!reoriented) {
		VEC3 left = getTransform()->getLeft();
		VEC3 front = getTransform()->getFront();
		switch (direction) {
		case 0: {
			unstuck_target = left*UNSTUCK_DISTANCE + getTransform()->getPosition();
			direction = 2;
			break;
		}
		case 1: {
			unstuck_target = -left*UNSTUCK_DISTANCE + getTransform()->getPosition();
			direction = 1;
			break;
		}
		case 2: {
			unstuck_target = -front*UNSTUCK_DISTANCE + getTransform()->getPosition();
			direction = 0;
			break;
		}
		default: {
			unstuck_target = left*UNSTUCK_DISTANCE + getTransform()->getPosition();
			direction = 0;
			break;
		}
		}
		reoriented = true;
	}
	if (!turnTo(unstuck_target))
		return STAY;
	else {
		reoriented = false;
		stuck_time = 0.f;
		stuck = false;
		return OK;
	}
}

// -- Turn To -- //
bool npc::turnTo(VEC3 dest, bool wide) {
	//static int test_giro = 0;
	//dbg("Estoy girando! (%d)\n", (++test_giro) % 100);

	PROFILE_FUNCTION("npc: turn to");
	float angle = 5.f;
	if (wide)
		angle = 30.f;
	float angle_epsilon = deg2rad(angle);

	VEC3 myPos = getTransform()->getPosition();

	//float dbg_yawBefore = yaw;

	// Cuanto necesito girar?
	float delta_yaw = getTransform()->getDeltaYawToAimTo(dest);
	bool done = turnYaw(delta_yaw, angle_epsilon);

	return done;
}

bool npc::turnYaw(float delta_yaw, float angle_epsilon)
{
	float yaw, pitch;
	getTransform()->getAngles(&yaw, &pitch);

	// Necesito girar menos que epsilon? --> Termino giro!
	if (abs(delta_yaw) < angle_epsilon) {
		//dbg("No es necesario girar. Devuelvo true. (deltayaw = %f", deltaYaw);
		return true;
	}

	// Ajusto deltayaw al maximo que puede girar el mequetrefe
	float maxDeltaYaw = SPEED_ROT * getDeltaTime();
	float delta_yaw_clamp = clampAbs(delta_yaw, maxDeltaYaw);
	yaw += delta_yaw_clamp;
	getTransform()->setAngles(yaw, pitch);

	//Ha acabado el giro?
	bool done = abs(delta_yaw) < angle_epsilon + 0.05;
	return done;
}

bool npc::turnToYaw(float target_yaw)
{
	TCompTransform * t = getTransform();
	float yaw = t->getYaw();

	//Normalize angles
	if (yaw < 0.f) yaw += 2 * M_PI;
	if (target_yaw < 0.f) target_yaw += 2 * M_PI;

	float delta_yaw = target_yaw - yaw;
	if (delta_yaw > M_PI) delta_yaw -= 2 * M_PI;
	else if (delta_yaw < -M_PI) delta_yaw += 2 * M_PI;
	return turnYaw(delta_yaw);
}

int npc::actionUnstuckMove() {
	PROFILE_FUNCTION("npc: actionunstuckmove");
	// move to get unstuck
	VEC3 myPos = getTransform()->getPosition();
	float dist = simpleDistXZ(myPos, unstuck_target);
	if (dist > DIST_REACH_PNT) {
		getPath(myPos, unstuck_target);
		changeCommonState(AST_MOVE);
		//SET_ANIM_GUARD(AST_MOVE);
		goTo(unstuck_target);
		return STAY;
	}
	else {
		setCurrent(action_when_stuck);
		return OK;
	}
}

// -- Go To -- //
void npc::goTo(const VEC3& dest) {
	PROFILE_FUNCTION("guard: go to");
	if (!SBB::readBool("navmesh")) {
		return;
	}
	VEC3 target = dest;
	VEC3 npcPos = getTransform()->getPosition();
	float walk_amount = SPEED_WALK * getDeltaTime();
	bool target_found = totalPathWpt <= 0 || currPathWpt >= totalPathWpt;
	while (!target_found) {
		if (fabsf(squaredDistXZ(pathWpts[currPathWpt], npcPos)) >= walk_amount) {
			target_found = true;
		}
		else {
			if (currPathWpt < totalPathWpt - 1) {
				currPathWpt++;
			}
			else target_found = true;
		}
	}

	if (currPathWpt < totalPathWpt) {
		target = pathWpts[currPathWpt];
	}

	target.y = npcPos.y;
	if (needsSteering(npcPos, getTransform(), SPEED_WALK, getParent())) {
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
void npc::goForward(float stepForward) {
	//static int test_forward = 0;
	//dbg("Estoy avanzando! (%d)\n", (++test_forward) % 100);

	PROFILE_FUNCTION("guard: go forward");
	VEC3 myPos = getTransform()->getPosition();
	float dt = getDeltaTime();
	getCC()->AddMovement(getTransform()->getFront() * stepForward*dt);
}