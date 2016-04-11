#include "mcv_platform.h"
#include "comp_platform.h"
#include "handle\handle.h"
#include "comp_transform.h"
#include "comp_physics.h"
#include "entity.h"

void TCompPlatform::onCreate(const TMsgEntityCreated &)
{
	CHandle h = CHandle(this).getOwner();
	CEntity *e = h;
	if (e) {
		TCompTransform *t = e->get<TCompTransform>();
		mOrigin = t->getPosition();

		mEnd = mOrigin + mMovement;

		TCompPhysics *p = e->get<TCompPhysics>();
		p->setKinematic(true);
	}
	else
		h.destroy();
}

void TCompPlatform::update(float elapsed)
{
	CHandle h = CHandle(this).getOwner();
	CEntity *e = h;
	if (!e) return;
	TCompTransform *t = e->get<TCompTransform>();
	

	TCompPhysics *p = e->get<TCompPhysics>();

	PxRigidDynamic *rd = p->getActor()->isRigidDynamic();
	if (rd) {
		float d;
		PxTransform tmx = rd->getGlobalPose();
		PxVec3 curr_pos = tmx.p;
		if(mDirection > 0) d = simpleDist(PhysxConversion::PxVec3ToVec3(curr_pos), mEnd);
		else d = simpleDist(PhysxConversion::PxVec3ToVec3(curr_pos), mOrigin);
	
		if (d < mEpsilon)	//if end, inverse direction	
			mDirection = -mDirection;


		curr_pos = curr_pos + PhysxConversion::Vec3ToPxVec3(mMovement*mDirection*mSpeed*getDeltaTime());

		rd->setKinematicTarget(PxTransform(	curr_pos,
											tmx.q));
	

	}
}

bool TCompPlatform::load(MKeyValue & atts)
{
	mMovement = VEC3(0,0,0);
	mMovement = atts.getPoint("movement");
	mSpeed = atts.getFloat("speed", 0.0f);

	return true;
}
