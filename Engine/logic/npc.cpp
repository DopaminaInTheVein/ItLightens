#include "mcv_platform.h"
#include "npc.h"

#include "recast/navmesh.h"
#include "recast/navmesh_query.h"
#include "recast/DebugUtils/Include/DebugDraw.h"

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
	Debug->DrawLine(startPointValue, endPointValue);
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
		Debug->DrawLine(pathWpts[i], pathWpts[i + 1], VEC3(1, 0, 1));
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