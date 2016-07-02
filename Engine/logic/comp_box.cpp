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
	if (size.x < 1.5f) type_box = eTypeBox::SMALL;
	else type_box = eTypeBox::MEDIUM;
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

bool TCompBox::getGrabPoints(
	TCompTransform* t_actor
	, VEC3& left
	, VEC3& right
	, VEC3& front_dir
	, VEC3& pos_grab
	, float offset_separation
	, bool recalc)
{
	GET_MY(t, TCompTransform);
	VEC3 pos = t->getPosition();
	VEC3 actor_pos = t_actor->getPosition();
	VEC3 actor_up = t_actor->getUp();

	// Previous calc variables
	VEC3 left_actor, right_actor;
	float size_side, size_front;

	//6 direction (3 * 2)
	VEC3 directions[3];
	float sizes[3] = { size.x, size.y, size.z };
	directions[0] = t->getLeft(); //x
	directions[1] = t->getUp(); //y
	directions[2] = t->getFront(); //z

	//Get the best direction
	if (recalc) {
		max_dot_index = -1;
		max_dot = 0;
		VEC3 actor_dir = VEC3(pos.x - actor_pos.x, 0, pos.z - actor_pos.z);
		actor_dir.Normalize();
		for (int i = 0; i < 3; i++) {
			float dot = actor_dir.Dot(directions[i]);
			if (abs(dot) > abs(max_dot)) {
				max_dot = dot;
				max_dot_index = i;
			}
		}
	}
	front_dir = directions[max_dot_index];
	if (max_dot < 0) front_dir *= -1;

	//Calc position
	//TODO: left and right must be calc using t_actor->getUp() [cross] front_dir
	//Casuistica actual fallara cuando la cara superior no esté mirando hacia arriba

	left_actor = actor_up.Cross(front_dir);
	left_actor.Normalize();
	right_actor = -left_actor;

	float dot_4 = left_actor.Dot(directions[(max_dot_index + 4) % 3]);
	float dot_5 = left_actor.Dot(directions[(max_dot_index + 5) % 3]);
	int left_index = abs(dot_4) > abs(dot_5) ? (max_dot_index + 4) % 3 : (max_dot_index + 5) % 3;

	size_side = sizes[left_index];
	size_front = sizes[max_dot_index];

	//SMALL BOX
	if (type_box == eTypeBox::SMALL) {
		left = left_actor * (size_side / 2 + offset_separation);
		right = right_actor * (size_side / 2 + offset_separation);
		//left -= front_dir * size_front / 4;
		//right -= front_dir * size_front / 4;
	}
	//BIG BOX
	else {
		left = left_actor * (size_side / 4);
		left.y -= size.y;
		left -= front_dir * size_front / 2.f;

		right = right_actor * (size_side / 4);
		right.y -= size.y;
		right -= front_dir * size_front / 2.f;
	}

	left += pos;
	right += pos;
	pos_grab = pos - front_dir * (size_front + 0.1f);

	return true;
}