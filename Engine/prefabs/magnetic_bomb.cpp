#include "mcv_platform.h"
#include "magnetic_bomb.h"
#include "components\comp_msgs.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "components\entity_tags.h"

map<string, statehandler> CMagneticBomb::statemap = {};

void CMagneticBomb::update(float elapsed)
{
	CountDown();
}

void CMagneticBomb::Init() {
}

void CMagneticBomb::CountDown() {
	t_waiting += getDeltaTime();
	if (t_waiting >= t_explode) {
		Explode();
	}
}

void CMagneticBomb::UpdatePosition() {
	SetMyEntity();
	TCompTransform *mtx = myEntity->get<TCompTransform>();

	VEC3 curr_position = mtx->getPosition();

	//TODO  REVISION BOMB DYNAMIC PHYSX

	float dist = 1.0f;
	float altura = 2.0f;
	float speed = 5.0;

	mtx->setPosition(curr_position);
}

void CMagneticBomb::Explode()
{
	myHandle = CHandle(this);
	myParent = myHandle.getOwner();
	dbg("MAGNETIC BOMB -> I am going to explode\n");
	if (myHandle.isValid() && myParent.isValid()) {
		SendMsg();
		//TODO: animation
		dbg("MAGNETIC BOMB -> exploded\n");
		myParent.destroy();
	}
}

void CMagneticBomb::SendMsg()
{
	TMsgStaticBomb msg;
	float rad = 10.0f;
	CEntity *p_e = myParent;
	TCompTransform *mtx = p_e->get<TCompTransform>();
	VEC3 org = mtx->getPosition();

	msg.pos = org;
	msg.r = rad;

	VHandles ets = tags_manager.getHandlesByTag(getID("AI"));
	for (CEntity *e : ets) {
		e->sendMsg(msg);
	}
}

// Sets the entity
void CMagneticBomb::SetMyEntity() {
	myEntity = myParent;
}