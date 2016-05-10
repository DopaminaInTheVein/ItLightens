#include "mcv_platform.h"
#include "comp_drone.h"

#include "comp_transform.h"
#include "comp_physics.h"
#include "handle\handle.h"
#include "entity.h"

void TCompDrone::onCreate(const TMsgEntityCreated &)
{
	CHandle h = CHandle(this).getOwner();
	CEntity *e = h;
	if (e) {
		//Init Waypoints (and add the last point the initial position)
		TCompTransform *t = e->get<TCompTransform>();
		wpts[wpts.size() - 1] = t->getPosition();
		waitTimes[waitTimes.size()-1] = 0;
		curWpt = 0;

		// Set Kinematic
		TCompPhysics *p = e->get<TCompPhysics>();
		p->setKinematic(true);
		
		final_pos = t->getPosition();
	}
	else h.destroy();
}

bool TCompDrone::SetMyBasicComponents()
{
	CHandle h = CHandle(this).getOwner();
	CEntity *e = h;
	if (!e) return false;
	transform = e->get<TCompTransform>();
	physics = e->get<TCompPhysics>();
	return true;
}

void TCompDrone::update(float elapsed)
{
	if (!SetMyBasicComponents()) return;
	if (timeToWait > 0) {
		//Waiting
		timeToWait -= elapsed;
	} else {
		if (simpleDist(wpts[curWpt], transform->getPosition()) < mEpsilon) {
			//Arrived
			curWpt = (curWpt + 1) % wpts.size();
			timeToWait = waitTimes[curWpt];
		} else {
			//Move to next
			moveToNext(elapsed);
		}
	}
}

void TCompDrone::moveToNext(float elapsed) {

	PxRigidDynamic *rd = physics->getActor()->isRigidDynamic();
	if (rd) {
		//PxTransform tmx = rd->getGlobalPose();
		//VEC3 curr_pos = PhysxConversion::PxVec3ToVec3(tmx.p);
		VEC3 direction = wpts[curWpt] - final_pos;
		direction.Normalize();
		final_pos = final_pos + direction * speed * elapsed;
		transform->setPosition(final_pos);
		//PxVec3 pxTarget = PhysxConversion::Vec3ToPxVec3(target);
		//rd->setKinematicTarget(PxTransform(pxTarget, tmx.q));
	}
}


// Loading the wpts
#define WPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d_%s", index, nameSufix);

bool TCompDrone::load(MKeyValue & atts)
{
	int n = atts.getInt("wpts_size", 0);
	wpts.resize(n+1);
	waitTimes.resize(n + 1);
	for (int i = 0; i < n; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		WPT_ATR_NAME(atrWait, "wait", i);
		wpts[i] = atts.getPoint(atrPos);
		waitTimes[i] = 0.5f; // MS2! atts.getFloat(atrWait, 0);
	}
	return true;
}

void TCompDrone::fixedUpdate(float elapsed) {
	PxRigidDynamic *rd = physics->getActor()->isRigidDynamic();
	if (!physics->isKinematic()) return;
	if (rd) {
		PxTransform tmx = rd->getGlobalPose();
		VEC3 target = final_pos;
		PxVec3 pxTarget = PhysxConversion::Vec3ToPxVec3(target);
		rd->setKinematicTarget(PxTransform(pxTarget, tmx.q));
	}
}
