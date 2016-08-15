#include "mcv_platform.h"
#include "throw_bomb.h"
#include "components\comp_msgs.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "components\comp_physics.h"
#include "skeleton/comp_bone_tracker.h"

#include "components\entity_tags.h"
#include "app_modules/logic_manager/logic_manager.h"

map<string, statehandler> CThrowBomb::statemap = {};

bool CThrowBomb::getUpdateInfo() {
	transform = compBaseEntity->get<TCompTransform>();
	if (!transform) return false;
	physics = compBaseEntity->get<TCompPhysics>();
	if (!physics) return false;
	rd = physics->getActor()->isRigidDynamic();
	if (!rd) return false;

	return true;
}

bool CThrowBomb::load(MKeyValue & atts)
{
	lmax = atts.getFloat("lmax", 3.0f);
	hmax = atts.getFloat("hmax", 1.f);
	speed = atts.getFloat("speed", 1.f);
	t_explode = atts.getFloat("timer", 2.5f);
	rad_squared = powf(atts.getFloat("radius", 2.f), 2);
	return true;
}

bool CThrowBomb::ImpactWhenBorn() {
	//PxQueryFilterData filterData;
	//filterData.data.word0 = 384;
	//PxRaycastBuffer hit;
	//VEC3 posThrower = transform->getPosition() + VEC3_UP* 4.f;
	//VEC3 posBorn = transform->getPosition()
	//	+ front_offset * transform->getFront()
	//	+ height_offset * VEC3_UP;
	//VEC3 rayDir = posBorn - posThrower;
	//float rayLong = rayDir.Length();
	//rayDir.Normalize();
	//Debug->DrawLine(posThrower, rayDir, rayLong, VEC3(1,0,0), 10.f);
	//impact = g_PhysxManager->raycast(posThrower, rayDir, rayLong, hit, filterData);
	//if (impact) onImpact(TMsgActivate());
	//return impact;
	return false;
}

void CThrowBomb::onCreate(const TMsgEntityCreated& msg) {
	if (statemap.empty()) {
		//Specific Bomb nodes
		AddState("born", (statehandler)&CThrowBomb::Born);
		AddState("idle", (statehandler)&CThrowBomb::Idle);
		AddState("throwing", (statehandler)&CThrowBomb::Throwing);
		AddState("throwed", (statehandler)&CThrowBomb::Throwed);
		AddState("impacted", (statehandler)&CThrowBomb::Impacted);
		AddState("explode", (statehandler)&CThrowBomb::Explode);
		AddState("dead", (statehandler)&CThrowBomb::Dead);
	}
	____TIMER_REDEFINE_(t_explode, 2.5f);
	ChangeState("born");
}

void CThrowBomb::update(float elapsed)
{
	Recalc();
}

void CThrowBomb::Born()
{
	nextState = false;
	rd->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	ChangeState("idle");
}

void CThrowBomb::Idle()
{
	GET_MY(phys, TCompPhysics);
	auto fd = phys->getFilterData();
	fd.word1 = 0;
	checkNextState("throwing");
}

void CThrowBomb::Throwing()
{
	//Nothing to do (bone tracker move this)
	return;
}

void CThrowBomb::Throwed() {
	GET_MY(phys, TCompPhysics);
	auto fd = phys->getFilterData();
	fd.word1 = PXM_NO_PLAYER;

	if (checkNextState("impacted")) {
		rd->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
	}
	else {
		throwMovement();
	}
	countDown();
}

void CThrowBomb::Impacted() {
	countDown();
}

void CThrowBomb::Explode()
{
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnExplode, "throw_bomb", ClHandle(this).getOwner());
	SendMsg();
	ChangeState("dead");
	ClHandle(this).getOwner().destroy();
}

void CThrowBomb::Dead()
{
	return; //Nothing to do
}

void CThrowBomb::initThrow() {
	lcurrent = hcurrent = 0;

	initial_pos = transform->getPosition();
	rd->setGlobalPose(PxTransform(
		PhysxConversion::Vec3ToPxVec3(transform->getPosition()),
		PhysxConversion::CQuaternionToPxQuat(transform->getRotation())
		));
}

void CThrowBomb::throwMovement() {
	PxTransform tmx;
	lcurrent = lcurrent + speed * getDeltaTime();
	//static int i = 0;
	//physics->setKinematic(i++ % 5 != 0);

	if (lcurrent < lmax) {
		hcurrent = sinf(lcurrent * M_PI / lmax);
	}
	else {
		hcurrent = (lmax - lcurrent) * 1.5f;
	}
	dbg("Lcur = %f, Hcur = %f\n", lcurrent, hcurrent);
	tmx = PxTransform(
		PhysxConversion::Vec3ToPxVec3(initial_pos + lcurrent * dir_throw + hcurrent * VEC3_UP),
		PhysxConversion::CQuaternionToPxQuat(transform->getRotation()));
	//rd->setKinematicTarget(tmx);
	rd->setGlobalPose(tmx);
}

bool CThrowBomb::countDown() {
	____TIMER_CHECK_DO_(t_explode);
	ChangeState("explode");
	return true;
	____TIMER_CHECK_DONE_(t_explode);
	return false;
}

void CThrowBomb::SendMsg()
{
	TMsgStaticBomb msg;
	VEC3 org = transform->getPosition();
	msg.pos = org;
	msg.r = rad_squared;

	VHandles ets = tags_manager.getHandlesByTag(getID("AI"));
	for (CEntity *e : ets) {
		e->sendMsg(msg);
	}
}

void CThrowBomb::onNextState(const TMsgActivate& msg) {
	nextState = true;
}

void CThrowBomb::onThrow(const TMsgThrow& msg) {
	dir_throw = msg.dir;
	initThrow();
	GETH_MY(TCompBoneTracker).destroy();
	ChangeState("throwed");
}

bool CThrowBomb::checkNextState(string new_st)
{
	bool changed = false;
	if (nextState) {
		ChangeState(new_st);
		changed = true;
	}
	nextState = false;
	return changed;
}