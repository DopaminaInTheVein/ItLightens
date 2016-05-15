#ifndef INC_BOX_COMPONENT_H_
#define	INC_BOX_COMPONENT_H_

#include "comp_base.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_physics.h"

//----------------------------
#include "logic\sbb.h"

struct TCompBox : public TCompBase {
	bool moving						= false;
	bool carePosition				= true;
	bool removable					= true;
	bool added						= false;

	VEC3 originPoint;
	float dist_separation	= 5.0f;

	CHandle mParent;

	

	void init() {
		mParent = CHandle(this).getOwner();
		if (carePosition) {
			CEntity *e = mParent;
			TCompTransform *t = e->get<TCompTransform>();
			originPoint = t->getPosition();
		}
	}

	void update(float elapsed) {
		if (carePosition && !added) {
			if (!mParent.isValid()) return;
			CEntity *e = mParent;
			TCompTransform *t = e->get<TCompTransform>();
			float d = simpleDist(t->getPosition(),originPoint);
			if (d > dist_separation) {
				ImTooFar();
			}

		}
		stuntNpcs();
	}

	void stuntNpcs() {
		CEntity * eMe = CHandle(this).getOwner();
		assert(eMe);
		TCompPhysics * pc = eMe->get<TCompPhysics>();
		assert(pc);
		auto rd = pc->getActor()->isRigidDynamic();
		if (!rd) return;
		VEC3 speed = PhysxConversion::PxVec3ToVec3(rd->isRigidDynamic()->getLinearVelocity());
		if (speed.LengthSquared() > 10.f) {
			TCompTransform * tMe = eMe->get<TCompTransform>();
			assert(tMe);
			VHandles npcs = tags_manager.getHandlesByTag(getID("AI"));
			for (CHandle npc : npcs) {
				if (npc.isValid()) {
					CEntity * eNpc = npc;
					TCompTransform * tNpc = eNpc->get<TCompTransform>();
					assert(tNpc);
					float dist = simpleDistXZ(tMe->getPosition(), tNpc->getPosition());
					if (dist < 1.5f) {
						//Check direction
						bool testDirection = false;
						if (abs(speed.x) > abs(speed.z)) {
							testDirection = sameSign(speed.x, tNpc->getPosition().x - tMe->getPosition().x);
						}
						else {
							testDirection = sameSign(speed.z, tNpc->getPosition().z - tMe->getPosition().z);
						}
						if (testDirection) {
							TMsgBoxHit	msg;
							eNpc->sendMsg(msg);
						}
					}
				}
			}
		}
	}

	void ImTooFar() {
		VHandles hs;
		hs = SBB::readHandlesVector("wptsBoxes");
		hs.push_back(mParent);
		SBB::postHandlesVector("wptsBoxes", hs);
		added = !added;
	}

	bool load(MKeyValue& atts) {
		carePosition = atts.getBool("fixed", false);
		removable = atts.getBool("removable", false);
		return true;
	}

	VEC3 GetLeavePoint() const { return originPoint; }
	bool isRemovable() { return removable;  }

	void onUnLeaveBox(const TMsgLeaveBox& msg) {
		added = !added;
		VHandles hs;
		hs = SBB::readHandlesVector("wptsBoxes");
		hs.erase(std::remove(hs.begin(), hs.end(), mParent), hs.end());
		SBB::postHandlesVector("wptsBoxes", hs);
		CEntity *e = mParent;
		if (!e) return;
		TCompPhysics *p = e->get<TCompPhysics>();
		TCompTransform *t = e->get<TCompTransform>();
		originPoint = t->getPosition();
		p->setKinematic(false);
	}

};

#endif