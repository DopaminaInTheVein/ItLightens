#include "mcv_platform.h"
#include "comp_box.h"

#include "logic/sbb.h"
#include "components/entity.h"
#include "components/comp_transform.h"
#include "components/comp_physics.h"

VHandles TCompBox::all_boxes;

void TCompBox::init() {
	mParent = CHandle(this).getOwner();
	if (carePosition) {
		CEntity *e = mParent;
		TCompTransform *t = e->get<TCompTransform>();
		originPoint = t->getPosition();
	}
	if (mParent.isValid()) all_boxes.push_back(mParent);
}

void TCompBox::update(float elapsed) {
	if (carePosition && !added) {
		if (!mParent.isValid()) return;
		CEntity *e = mParent;
		TCompTransform *t = e->get<TCompTransform>();
		float d = simpleDist(t->getPosition(), originPoint);
		if (d > dist_separation) {
			ImTooFar();
		}
	}
	stuntNpcs();
}

void TCompBox::stuntNpcs() {
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

void TCompBox::ImTooFar() {
	VHandles hs;
	hs = SBB::readHandlesVector("wptsBoxes");
	hs.push_back(mParent);
	SBB::postHandlesVector("wptsBoxes", hs);
	added = !added;
}

bool TCompBox::load(MKeyValue& atts) {
	carePosition = atts.getBool("fixed", false);
	removable = atts.getBool("removable", false);
	return true;
}

VEC3 TCompBox::GetLeavePoint() const { return originPoint; }
bool TCompBox::isRemovable() { return removable; }

void TCompBox::onUnLeaveBox(const TMsgLeaveBox& msg) {
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

TCompBox::~TCompBox() {
	removeFromVector(all_boxes, CHandle(this).getOwner());
}