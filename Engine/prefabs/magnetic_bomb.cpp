#include "mcv_platform.h"
#include "magnetic_bomb.h"
#include "components\comp_msgs.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "components\entity_tags.h"

void CMagneticBomb::update(float elapsed)
{
	Recalc();
}

void CMagneticBomb::Init() {
	auto om = getHandleManager<CMagneticBomb>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	AddState("going_up", (statehandler)&CMagneticBomb::GoingUp);
	AddState("countDown", (statehandler)&CMagneticBomb::CountDown);
	AddState("going_down", (statehandler)&CMagneticBomb::GoingDown);
	AddState("explode", (statehandler)&CMagneticBomb::Explode);

	ChangeState("going_up");
}

void CMagneticBomb::CountDown() {
	t_waiting += getDeltaTime();
	if (t_waiting >= t_explode) {
		ChangeState("explode");
	}
}

void CMagneticBomb::GoingUp() {
	SetMyEntity();
	TCompTransform *mtx = myEntity->get<TCompTransform>();

	VEC3 curr_position = mtx->getPosition();

	UpdatePosition();
	float altura = 2.0f;
	if (curr_position.y >= altura / 2) {

		ChangeState("going_down");
	}
}

void CMagneticBomb::GoingDown() {
	SetMyEntity();
	TCompTransform *mtx = myEntity->get<TCompTransform>();

	VEC3 curr_position = mtx->getPosition();

	UpdatePosition();

	if (curr_position.y <= 0) {
		curr_position.y = 0.0f;
		mtx->setPosition(curr_position);

		ChangeState("countDown");
	}

}

void CMagneticBomb::UpdatePosition() {
	SetMyEntity();
	TCompTransform *mtx = myEntity->get<TCompTransform>();

	VEC3 curr_position = mtx->getPosition();

	float dist = 1.0f;
	float altura = 2.0f;
	float speed = 5.0;

	curr_position.z += mtx->getFront().z*speed * getDeltaTime();
	curr_position.x += mtx->getFront().x*speed * getDeltaTime();

	x_local += speed*getDeltaTime();
	curr_position.y = altura*sinf(dist*(x_local / (float)M_PI));


	mtx->setPosition(curr_position);
}

void CMagneticBomb::Explode()
{
	dbg("MAGNETIC BOMB -> I am going to explode\n");
	SendMsg();
	//TODO: animation
	dbg("MAGNETIC BOMB -> exploded\n");
	destroy();
}

void CMagneticBomb::SendMsg()
{

	TMsgMagneticBomb msg;
	float rad = 10.0f;
	CEntity *p_e = myParent;
	TCompTransform *mtx = p_e->get<TCompTransform>();
	VEC3 org = mtx->getPosition();

	msg.pos = org;
	msg.r = rad;

	//TODO: for each NPC, tag NPC
	VEntities ets = tags_manager.getHandlesPointerByTag(getID("AI_guard"));
	for (CEntity *e : ets) {
		e->sendMsg(msg);
	}
}

// Sets the entity
void CMagneticBomb::SetMyEntity() {
	myEntity = myParent;
}
