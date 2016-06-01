
#include "mcv_platform.h"

#include "comp_tracker.h"
#include "comp_physics.h"
#include "comp_transform.h"
#include "entity.h"
#include "entity_parser.h"
#include "imgui/imgui.h"

void TCompTracker::onCreate(const TMsgEntityCreated &)
{

}

bool TCompTracker::load(MKeyValue& atts) {
	// Size & Longitude
	longitude = atts.getFloat("long", -1);
	assert(longitude > 0);
	size = atts.getInt("size", -1);
	assert(size > 0);
	assert(size <= MAX_TRACK_POINTS);

	//Positions & Orientations
	char nameAttr[20];
	for (int i = 0; i < size; i++) {
		//Position
		sprintf(nameAttr, "point%d", i);
		positions[i] = atts.getPoint(nameAttr);
		
		//Orientation
		sprintf(nameAttr, "tangent%d", i); // Quaternions!?
		orientations[i] = atts.getPoint(nameAttr);
	}

	//Normal Speed
	normalSpeed = mSpeed / longitude;

	return true;
}

void TCompTracker::setFollower(const TMsgFollow &msg) {

	//dbg("---------- Tracker -----------------------------------\n");
	//dbg("Longitude = %f\n", longitude);
	//for (int i = 0; i < size; i++) {
	//	dbg("point%d: (%f, %f, %f)\n", i, positions[i].x, positions[i].y, positions[i].z);
	//	dbg("tangent%d: (%f, %f, %f)\n", i, orientations[i].x, orientations[i].y, orientations[i].z);
	//}
	//dbg("---------- /Tracker -----------------------------------\n");

	CHandle follower = msg.follower;
	if (follower.isValid()) {
		HandleTrack ht;
		ht.handle = follower;
		
		//Busqueda index del punto mas cercano a la spline
		CEntity* eFollower = follower;
		TCompTransform* transform = eFollower->get<TCompTransform>();
		int nearestIndex = 0;
		float distNearest = FLT_MAX;
		VEC3 pos = transform->getPosition();
		for (int i = 0; i < size; i++) {
			float dist = realDist(pos, positions[i]);
			if (dist < distNearest) {
				distNearest = dist;
				nearestIndex = i;
			}
		}

		ht.normalTime = (float) nearestIndex / size;

		followers.push_back(ht);
		TCompPhysics * physic_comp = eFollower->get<TCompPhysics>();
		PxRigidDynamic * rd = physic_comp->getRigidActor()->isRigidDynamic();
		if (rd) rd->setMassSpaceInertiaTensor(PxVec3(0.f, 1.f, 0.f));
	}
}

void TCompTracker::update(float elapsed) {
	for (HandleTrack follower : followers) {
		if (follower.handle.isValid()) {
			updateTrackMovement(follower);
		}
	}
}

void TCompTracker::updateTrackMovement(HandleTrack ht) {
	CEntity* e = ht.handle;
	TCompPhysics * physic_comp = e->get<TCompPhysics>();
	TCompTransform* transform = e->get<TCompTransform>();
	if (physic_comp && transform) {
		// Prev position
		VEC3 prevPos = evaluatePos(ht);

		// Next Position
		ht.normalTime += clamp(normalSpeed * getDeltaTime(), 0, 1);
		VEC3 nextPos = evaluatePos(ht);

		//DeltaPos
		VEC3 deltaPos = nextPos - prevPos;
		deltaPos.Normalize();
		float deltaTime = getDeltaTime();
		PxRigidDynamic * rd = physic_comp->getRigidActor()->isRigidDynamic();
		PxVec3 force = Vec3ToPxVec3(deltaPos * mSpeed);
		rd->setLinearVelocity(force);
	}
}

VEC3 TCompTracker::evaluatePos(HandleTrack ht) {
	float indexPrev = ht.normalTime * (float)size;
	float weightPrev = indexPrev - (int)(indexPrev);
	float weightNext = 1 - weightPrev;
	return positions[(int)indexPrev] * weightPrev + positions[(int)indexPrev + 1] * weightNext;
}

void TCompTracker::renderInMenu() {
	ImGui::DragFloat3("Speed:", &mSpeed);
}