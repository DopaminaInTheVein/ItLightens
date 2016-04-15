#include "mcv_platform.h"

#include "comp_tracker.h"
#include "comp_physics.h"
#include "comp_transform.h"
#include "entity.h"
#include "entity_parser.h"

void TCompTracker::onCreate(const TMsgEntityCreated &)
{
	//Pruebas
	CEntity* eMe = CHandle(this).getOwner();
	TMsgFollow msg;
	msg.follower = IdEntities::findById(1);
	eMe->sendMsg(msg);
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
		followers.push_back(ht);
		
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
	}
}

void TCompTracker::update(float elapsed) {
	for (HandleTrack follower : followers) {
		//if (follower.isValid()) {
		//	CEntity* e = follower;
		//	TCompPhysics * physic_comp = e->get<TCompPhysics>();
		//	TCompTransform* transform = e->get<TCompTransform>();
		//	if (physic_comp) updateTrackMovement(physic_comp, transform);
		//}
	}
}

void TCompTracker::updateTrackMovement(TCompPhysics* physic_comp, TCompTransform* transform) {

}