#include "mcv_platform.h"
#include "comp_box.h"

#include "logic/sbb.h"
#include "components/entity.h"
#include "components/comp_transform.h"
#include "components/comp_physics.h"

VHandles TCompBox::all_boxes;

void TCompBox::init() {
	if (carePosition) {
		GET_MY(t, TCompTransform);
		if (!t) return;
		originPoint = t->getPosition();
	}
	all_boxes.push_back(mParent);
}

void TCompBox::update(float elapsed) {
	if (carePosition && !added) {
		if (!mParent.isValid()) return;
		CEntity *e = mParent;
		GET_MY(t, TCompTransform);
		if (!t) return;
		float d = simpleDist(t->getPosition(), originPoint);
		if (d > dist_separation) {
			ImTooFar();
		}
	}
	stuntNpcs();
}

void TCompBox::stuntNpcs() {
	GET_MY(pc, TCompPhysics);
	if (!pc)return;
	auto rd = pc->getActor()->isRigidDynamic();
	if (!rd) return;
	VEC3 speed = PhysxConversion::PxVec3ToVec3(rd->isRigidDynamic()->getLinearVelocity());
	if (speed.LengthSquared() > 10.f) {
		GET_MY(tMe, TCompTransform);
		if (!tMe) return;
		VHandles npcs = tags_manager.getHandlesByTag(getID("AI"));
		for (CHandle npc : npcs) {
			if (npc.isValid()) {
				GET_COMP(tNpc, npc, TCompTransform);
				if (!tNpc) return;
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
						npc.sendMsg(msg);
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

bool TCompBox::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("size", size);
	return true;
}

VEC3 TCompBox::GetLeavePoint() const { return originPoint; }
bool TCompBox::isRemovable() { return removable; }
void TCompBox::UnGrab()
{
	GET_MY(box_p, TCompPhysics);
	GET_MY(box_t, TCompTransform);
	box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);
	box_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false);
	box_p->setGravity(true);
	box_p->setKinematic(false);
}
void TCompBox::onUnLeaveBox(const TMsgLeaveBox& msg) {
	added = !added;
	VHandles hs;
	hs = SBB::readHandlesVector("wptsBoxes");
	hs.erase(std::remove(hs.begin(), hs.end(), mParent), hs.end());
	SBB::postHandlesVector("wptsBoxes", hs);
	GET_MY(p, TCompPhysics);
	GET_MY(t, TCompTransform);
	if (t) originPoint = t->getPosition();
	if (p) p->setKinematic(false);
}

TCompBox::~TCompBox() {
	removeFromVector(all_boxes, CHandle(this).getOwner());
}

bool TCompBox::getGrabPoints(
	TCompTransform* t_actor
	, VEC3& left
	, VEC3& right
	, VEC3& front_dir
	, VEC3& pos_grab
	, VEC3& normal_left
	, VEC3& normal_right
	, float offset_separation
	, bool recalc)
{
	GET_MY(t, TCompTransform);
	VEC3 pos = t->getPosition();
	VEC3 actor_pos = t_actor->getPosition();
	VEC3 actor_up = t_actor->getUp();

	// Previous calc variables
	VEC3 left_actor, right_actor, up_actor;
	float size_side, size_front, size_up;

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

	//Left and right vector of the box from actor perspective
	left_actor = actor_up.Cross(front_dir);
	left_actor.Normalize();
	right_actor = -left_actor;
	up_actor = front_dir.Cross(left_actor);
	up_actor.Normalize();

	// Sizes side, front and top from actor perspective
	int cand_1 = (max_dot_index + 4) % 3;
	int cand_2 = (max_dot_index + 5) % 3;
	float dot_1 = left_actor.Dot(directions[(max_dot_index + 4) % 3]);
	float dot_2 = left_actor.Dot(directions[(max_dot_index + 5) % 3]);
	int left_index, up_index;
	if (abs(dot_1) > abs(dot_2)) {
		left_index = cand_1;
		up_index = cand_2;
	}
	else {
		left_index = cand_2;
		up_index = cand_1;
	}
	size_side = sizes[left_index];
	size_up = sizes[up_index];
	size_front = sizes[max_dot_index];

	//SMALL BOX (GRAB)
	if (type_box == eTypeBox::SMALL) {
		left = left_actor * (size_side / 2 + offset_separation);
		right = right_actor * (size_side / 2 + offset_separation);
		normal_left = left_actor;
		normal_right = right_actor;
		pos_grab = pos - front_dir * (size_front / 2.f + 0.4f);
	}
	//MEDIUM BOX (PUSH)
	else if (type_box == eTypeBox::MEDIUM) {
		left = left_actor * (size_side / 4);
		left -= up_actor * size_up / 2.f;
		left -= front_dir * size_front / 2.f;
		right = right_actor * (size_side / 4);
		right -= up_actor * size_up / 2.f;
		right -= front_dir * size_front / 2.f;
		normal_left = normal_right = -up_actor;
		pos_grab = pos - front_dir * (size_front / 2.f + push_box_distance);
	}

	left += pos;
	right += pos;

	Debug->DrawLine(pos_grab, left, VEC3(0.f, 1.f, 0.f));
	Debug->DrawLine(pos_grab, right, VEC3(1.f, 1.f, 0.5f));
	Debug->DrawLine(left, normal_left, 0.2f, VEC3(0.f, 1.f, 0.f));
	Debug->DrawLine(right, normal_right, 0.2f, VEC3(1.f, 1.f, 0.f));

	return true;
}

bool TCompBox::getPushPoints(
	TCompTransform* t_actor
	, VEC3& left
	, VEC3& right
	, VEC3& front_dir
	, VEC3& pos_grab
	, VEC3& normal_left
	, VEC3& normal_right
	, float offset_separation
	, bool recalc)
{
	GET_MY(t, TCompTransform);
	VEC3 pos = t->getPosition();
	VEC3 actor_pos = t_actor->getPosition();
	VEC3 actor_up = t_actor->getUp();

	// Previous calc variables
	VEC3 left_actor, right_actor, up_actor;
	float size_side, size_front, size_up;

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

	//Left and right vector of the box from actor perspective
	left_actor = actor_up.Cross(front_dir);
	left_actor.Normalize();
	right_actor = -left_actor;
	up_actor = front_dir.Cross(left_actor);
	up_actor.Normalize();

	// Sizes side, front and top from actor perspective
	int cand_1 = (max_dot_index + 4) % 3;
	int cand_2 = (max_dot_index + 5) % 3;
	float dot_1 = left_actor.Dot(directions[(max_dot_index + 4) % 3]);
	float dot_2 = left_actor.Dot(directions[(max_dot_index + 5) % 3]);
	int left_index, up_index;
	if (abs(dot_1) > abs(dot_2)) {
		left_index = cand_1;
		up_index = cand_2;
	}
	else {
		left_index = cand_2;
		up_index = cand_1;
	}
	size_side = sizes[left_index];
	size_up = sizes[up_index];
	size_front = sizes[max_dot_index];

	// we only push the box if its MEDIUM
	if (type_box == eTypeBox::MEDIUM) {
		left = left_actor * (size_side / 4);
		left += up_actor * size_up / 2.5f;
		left -= front_dir * size_front / 2.f;
		right = right_actor * (size_side / 4);
		right += up_actor * size_up / 2.5f;
		right -= front_dir * size_front / 2.f;
		normal_left = normal_right = +up_actor;
		pos_grab = pos - front_dir * (size_front / 2.f + 1.f);
	}
	else {
		return false;
	}

	left += pos;
	right += pos;

	Debug->DrawLine(pos_grab, left, VEC3(0.f, 1.f, 0.f));
	Debug->DrawLine(pos_grab, right, VEC3(1.f, 1.f, 0.5f));
	Debug->DrawLine(left, normal_left, 0.2f, VEC3(0.f, 1.f, 0.f));
	Debug->DrawLine(right, normal_right, 0.2f, VEC3(1.f, 1.f, 0.f));

	return true;
}