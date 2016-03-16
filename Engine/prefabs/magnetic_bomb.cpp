#include "mcv_platform.h"
#include "magnetic_bomb.h"
#include "components\comp_msgs.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "components\entity_tags.h"
#include "physics\physics.h"

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

	ray_cast_query floor_query = ray_cast_query(curr_position, VEC3(0, -1, 0), 15.0f, COL_TAG_SOLID);
	ray_cast_result res = Physics::calcRayCast(floor_query);
	VEC3 ground = res.positionCollision;
	float d = simpleDist(curr_position, ground);

	UpdatePosition();
	float altura = 2.0f;
	if (d >= altura / 2) {
		ChangeState("going_down");
	}
}

void CMagneticBomb::GoingDown() {
	SetMyEntity();
	TCompTransform *mtx = myEntity->get<TCompTransform>();

	UpdatePosition();
	VEC3 curr_position = mtx->getPosition();

	ray_cast_query floor_query = ray_cast_query(curr_position, VEC3(0, -1, 0), 15.0f, COL_TAG_SOLID);
	ray_cast_result res = Physics::calcRayCast(floor_query);
	VEC3 ground = res.positionCollision;
	float d = simpleDist(curr_position, ground);

	if (d < 0.2) {
		curr_position.y = res.positionCollision.y+0.2;
		mtx->setPosition(curr_position);

		ChangeState("countDown");
	}

}

void CMagneticBomb::UpdatePosition() {
	SetMyEntity();
	TCompTransform *mtx = myEntity->get<TCompTransform>();

	VEC3 curr_position = mtx->getPosition();

	ray_cast_query floor_query = ray_cast_query(curr_position, VEC3(0, -1, 0), 15.0f, COL_TAG_SOLID);
	ray_cast_result res = Physics::calcRayCast(floor_query);
	VEC3 ground = res.positionCollision;
	float d = simpleDist(curr_position, ground);


	float dist = 1.0f;
	float altura = 2.0f;
	float speed = 5.0;

	curr_position.z += mtx->getFront().z*speed * getDeltaTime();
	curr_position.x += mtx->getFront().x*speed * getDeltaTime();

	x_local += speed*getDeltaTime();
	curr_position.y = (curr_position.y - d) + altura*sinf(dist*(x_local / (float)M_PI));


	mtx->setPosition(curr_position);
}

void CMagneticBomb::toExplode() {
	ChangeState("explode");
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
		destroy();
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
