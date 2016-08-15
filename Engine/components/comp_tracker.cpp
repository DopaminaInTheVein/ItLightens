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
	assert(size > 1);
	assert(size <= MAX_TRACK_POINTS);

	//Positions & Orientations
	char nameAttr[20];
	for (int i = 0; i < size; i++) {
		//Position
		sprintf(nameAttr, "point%d", i);
		positions[i] = atts.getPoint(nameAttr);

		//Orientation
		//sprintf(nameAttr, "tangent%d", i); // Quaternions!?
		//orientations[i] = atts.getPoint(nameAttr);
	}

	return true;
}
bool TCompTracker::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("long", longitude);
	atts.put("size", size);

	//Positions & Orientations
	char nameAttr[20];
	for (int i = 0; i < size; i++) {
		//Position
		sprintf(nameAttr, "point%d", i);
		atts.put(nameAttr, positions[i]);

		//Orientation
		//sprintf(nameAttr, "tangent%d", i); // Quaternions!?
		//orientations[i] = atts.getPoint(nameAttr);
	}
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

	ClHandle follower = msg.follower;
	if (follower.isValid()) {
		HandleTrack ht;
		ht.handle = follower;
		ht.speed = msg.speed;

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

		ht.normalTime = (float)nearestIndex / (size - 1);
		ht.normalSpeed = ht.speed / longitude;
		followers.push_back(ht);
	}
}

void TCompTracker::update(float elapsed) {
	for (auto follower_it = followers.begin(); follower_it != followers.end(); ) {
		if (follower_it->handle.isValid()) {
			updateTrackMovement(*follower_it);
			if (follower_it->arrived()) {
				follower_it = unfollow(*follower_it);
			}
			else follower_it++;
		}
	}
}

void TCompTracker::updateTrackMovement(HandleTrack& ht) {
	CEntity* e = ht.handle;
	//TCompPhysics * physic_comp = e->get<TCompPhysics>();
	TCompTransform* transform = e->get<TCompTransform>();
	if (transform) { //&& physic_comp) {
		// Prev position
		VEC3 prevPos = evaluatePos(ht);

		// Next Position
		ht.normalTime += ht.normalSpeed * getDeltaTime();
		clamp_me(ht.normalTime, 0.f, 1.f);
		VEC3 nextPos = evaluatePos(ht);
		transform->setPosition(transform->getPosition() + nextPos - prevPos);
		GET_COMP(phys, ht.handle, TCompPhysics);
		if (phys) {
			phys->setPosition(transform->getPosition(), transform->getRotation());
		}
		//DeltaPos
		//VEC3 deltaPos = nextPos - prevPos;
		//deltaPos.Normalize();
		//float deltaTime = getDeltaTime();
		//PxRigidDynamic * rd = physic_comp->getRigidActor()->isRigidDynamic();
		//PxVec3 force = Vec3ToPxVec3(deltaPos * mSpeed);
		//rd->setLinearVelocity(force);
	}
}

std::vector<HandleTrack>::iterator TCompTracker::unfollow(HandleTrack ht)
{
	return followers.erase(
		std::remove(
			followers.begin(),
			followers.end(),
			ht
			),
		followers.end()
		);
}

VEC3 TCompTracker::evaluatePos(HandleTrack ht) {
	float indexPrev = ht.normalTime * ((float)size - 1);
	float weightNext = indexPrev - (int)(indexPrev);
	float weightPrev = 1 - weightNext;
	return positions[(int)indexPrev] * weightPrev + positions[(int)indexPrev + 1] * weightNext;
}

void TCompTracker::renderInMenu() {
}