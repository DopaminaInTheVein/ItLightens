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
	size = atts.getPoint("size");
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

#ifndef NDEBUG
void TCompBox::render() {
	//VEC3 up, front, left;
	//VEC3 upf, frontf, leftf;

	//GET_MY(t, TCompTransform);
	//VEC3 pos = t->getPosition();
	//up = (t->getUp() * size.y / 2);
	//up += pos;
	//upf = pos + t->getUp() * size;

	//front = (t->getFront() * size.z / 2);
	//front += pos;
	//frontf = pos + t->getFront() * size;

	//left = (t->getLeft() * size.z / 2);
	//left += pos;
	//leftf = pos + t->getLeft() * size;

	//Debug->DrawLine(up, upf);
	//Debug->DrawLine(left, leftf, VEC3(0, 1, 0));
	//Debug->DrawLine(front, frontf, VEC3(0,0,1));
}
#endif

bool TCompBox::getGrabPoints(const VEC3& actor_pos, VEC3& left, VEC3& right) {
	GET_MY(t, TCompTransform);
	VEC3 pos = t->getPosition();
	// Four directions
	VEC3 directions[4];
	float sizes[4];
	directions[0] = t->getFront();
	directions[1] = -t->getLeft();
	directions[2] = -t->getFront();
	directions[3] = t->getLeft();
	sizes[0] = sizes[2] = size.z;
	sizes[1] = sizes[3] = size.x;


	//Get the best direction
	float max_dot = FLT_MIN;
	int max_dot_index = -1;
	VEC3 actor_dir = pos - actor_pos;
	actor_dir.Normalize();	
	for (int i = 0; i < 4; i++) {
		float dot = actor_dir.Dot(directions[i]);
		if (dot > max_dot) {
			max_dot = dot;
			max_dot_index = i;
		}
	}

	//Calc position
	VEC3 left_actor = directions[(max_dot_index + 3) % 4];
	left = left_actor * sizes[max_dot_index] / 2;
	right = (left_actor * -sizes[max_dot_index] / 2);
	left += pos;
	right += pos;
	return true;
}