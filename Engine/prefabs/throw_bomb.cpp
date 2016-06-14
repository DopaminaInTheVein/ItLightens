#include "mcv_platform.h"
#include "throw_bomb.h"
#include "components\comp_msgs.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "components\comp_physics.h"

#include "components\entity_tags.h"

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
	front_offset = atts.getFloat("front", 0.3f);
	height_offset = atts.getFloat("height", 1.f);
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

void CThrowBomb::Init(float lmax, float hmax) {
	//getUpdateInfoBase(CHandle(this).getOwner());
	//if (ImpactWhenBorn()) return;

	//this->lmax = lmax;
	//this->hmax = hmax;
	//lcurrent = hcurrent = 0;
	//transform->setPosition(
	//	transform->getPosition()
	//	+ front_offset * transform->getFront()
	//	+ height_offset * VEC3_UP);
	//initial_pos = transform->getPosition();
	//final_pos = initial_pos + transform->getFront() * lmax;
	//rd->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//rd->setGlobalPose(PxTransform(
	//	PhysxConversion::Vec3ToPxVec3(transform->getPosition()),
	//	PhysxConversion::CQuaternionToPxQuat(transform->getRotation())
	//));


	/*rd->setKinematicTarget(PxTransform(
		PhysxConversion::Vec3ToPxVec3(transform->getPosition()),
		PhysxConversion::CQuaternionToPxQuat(transform->getRotation())
	));*/
}

void CThrowBomb::update(float elapsed)
{
	//if (!impact) {
	//	UpdatePosition();
	//}
	//CountDown();
}

void CThrowBomb::UpdatePosition() {
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
	tmx = PxTransform(
		PhysxConversion::Vec3ToPxVec3(initial_pos + lcurrent * transform->getFront() + hcurrent * VEC3_UP),
		PhysxConversion::CQuaternionToPxQuat(transform->getRotation()));
	//rd->setKinematicTarget(tmx);
	rd->setGlobalPose(tmx);
}

void CThrowBomb::CountDown() {
	if (exploded) return;
	t_waiting += getDeltaTime();
	if (t_waiting >= t_explode) {
		Explode();
	}
}

void CThrowBomb::Explode()
{
	dbg("THROW BOMB -> I am going to explode\n");
	SendMsg();
	//TODO: animation
	dbg("THROW BOMB -> exploded\n");
	CHandle(this).getOwner().destroy();
	exploded = true;
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

void CThrowBomb::onImpact(const TMsgActivate& msg) {
	impact = true;
	rd->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
	rd->addForce(PhysxConversion::Vec3ToPxVec3(transform->getFront()*speed) * 10);
	//TMsgSetTag msgTag;
	//msgTag.add = false;
	//msgTag.tag_id = getID("throw_bomb");
	//physics->updateTagsSetupActor();
}