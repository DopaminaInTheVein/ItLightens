#include "mcv_platform.h"
#include "comp_charactercontroller.h"

#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"
#include "comp_name.h"

#include "app_modules\io\io.h"

void TCompCharacterController::ApplyGravity(float dt)
{
	PROFILE_FUNCTION("apply gravity");
	if (mAffectGravity) {
		if (!OnGround()){
				dt = getDeltaTime();
				mSpeed.y += -12.0f*dt;	//update y-axis speed with gravity
		}
	}
	else mSpeed.y = 0.0f;
}

void TCompCharacterController::AddImpulse(const VEC3& impulse) {
	PROFILE_FUNCTION("add impulse");
	mSpeed.y = impulse.y;
	if (mSpeed.y > mMaxYimpulse)		//check if acum impulse passed max impulse
		mSpeed.y = mMaxYimpulse;
	mSpeed.x += impulse.x;
	mSpeed.z += impulse.z;
}

void TCompCharacterController::updateFriction() {
	PROFILE_FUNCTION("update friction");
	float dt = getDeltaTime();
	if(mSpeed.x > 0) mSpeed.x -= mSpeed.x*mFriction*dt;
	else if(mSpeed.x < 0) mSpeed.x += mSpeed.x*mFriction*dt;
	if(mSpeed.z > 0) mSpeed.z -= mSpeed.z*mFriction*dt;
	else if (mSpeed.z < 0) mSpeed.z += mSpeed.z*mFriction*dt;

	float abs_x = abs(mSpeed.x);
	float abs_z = abs(mSpeed.z);

	if (abs_x < eOffsetSpeed)
		mSpeed.x = 0;

	if (abs_z < eOffsetSpeed)
		mSpeed.z = 0;
	
}

void TCompCharacterController::ApplyPendingMoves() {
	std::string toprint = "apply pending moves ";
	toprint = toprint + name;
	PROFILE_FUNCTION(name.c_str());
	float dt = getDeltaTime();
	mToMove += mSpeed;
	if (mToMove != VEC3(0.0f, 0.0f, 0.0f)) {
		PxVec3 moved = PxVec3(mToMove.x, mToMove.y, mToMove.z)*dt;
		if(OnGround() | !mAffectGravity) mFlagsCollisions = pActor->move(moved, 0, dt, mFilter);
		else mFlagsCollisions = pActor->move(moved/2, 0, dt, mFilter);
		mToMove = VEC3(0.0f,0.0f,0.0f);
	}
}

void TCompCharacterController::recalcOnGround()
{
	PROFILE_FUNCTION("on ground");
	if (mFlagsCollisions & PxControllerFlag::eCOLLISION_DOWN) {
		mOnGround = true;
	}
	else
		mOnGround = false;
}

void TCompCharacterController::AddMovement(const VEC3& direction, float speed) {
	PROFILE_FUNCTION("add move");
	mToMove += direction*speed;
}

bool TCompCharacterController::load(MKeyValue & atts)
{
	mRadius = atts.getFloat("radius", 0.5f);
	mHeight = atts.getFloat("height", 0.5f);
	return true;
}

VEC3 TCompCharacterController::getPosition()
{
	PROFILE_FUNCTION("get position");
	return PhysxConversion::PxExVec3ToVec3(pActor->getPosition());
}

void TCompCharacterController::onCreate(const TMsgEntityCreated &)
{

	pActor = PhysxManager->CreateCharacterController(mRadius, mHeight);
	mAffectGravity = true;
	CEntity *e = CHandle(this).getOwner();
	if (e) {	//update position from CC to match the render transform
		TCompName  *nameComp = e->get<TCompName>();
		if(nameComp)
			name = std::string(nameComp->name);
		TCompTransform *mtx = e->get<TCompTransform>();
		PxExtendedVec3 p = Vec3ToPxExVec3(mtx->getPosition());
		p.y += mHeight + mRadius;	//add height value from capsule, center from collider at center of the shape
		pActor->setPosition(p);
		pActor->getActor()->userData = e;
		updateTags(DEFAULT_DATA_CC);
	}
}

void TCompCharacterController::updateTags(PxFilterData& filter)
{
	//PROFILE_FUNCTION("update tags");
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("player")) { //player
			filter.word0 = filter.word0 | ItLightensFilter::ePLAYER_BASE;
			
		}
		if (h.hasTag("AI_guard"))
			filter.word0 = filter.word0 | ItLightensFilter::eGUARD | ItLightensFilter::eNPC;

		if (h.hasTag("AI_poss"))
			filter.word0 = filter.word0 | ItLightensFilter::ePOSSEABLE | ItLightensFilter::eNPC;
	}

	mFlagsBehavior = PxControllerBehaviorFlag::eCCT_SLIDE | PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

	PxRigidActor *ra = pActor->getActor()->isRigidActor();
	const PxU32 numShapes = ra->getNbShapes();
	PxShape **ptr;
	ptr = new PxShape*[numShapes];
	ra->getShapes(ptr, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = ptr[i];
		shape->setSimulationFilterData(filter);
		shape->setQueryFilterData(filter);
	}

	free(ptr);

}

void TCompCharacterController::teleport(const PxVec3& pos)
{
	pActor->setPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
}

void TCompCharacterController::SetCollisions(bool new_collisions)
{
	PxRigidActor *ra = pActor->getActor()->isRigidActor();
	if (ra) {
		const PxU32 numShapes = ra->getNbShapes();
		PxShape **ptr;
		ptr = new PxShape*[numShapes];
		ra->getShapes(ptr, numShapes);
		for (PxU32 i = 0; i < numShapes; i++)
		{
			PxShape* shape = ptr[i];
			PxFilterData ft = shape->getSimulationFilterData();
			if (!new_collisions) {
				ft.word1 &= ~ItLightensFilter::eCOLLISION;
				ft.word1 &= ~ItLightensFilter::eCAN_TRIGGER;	//for test only
			}
			else {
				ft.word1 |= ItLightensFilter::eCOLLISION;
				ft.word1 |= ItLightensFilter::eCAN_TRIGGER;		//for test only
			}
			shape->setSimulationFilterData(ft);
			shape->setQueryFilterData(ft);
		}

		free(ptr);
	}
}

void TCompCharacterController::update(float dt)
{
	//PROFILE_FUNCTION("update");
	if (mActive) {
		recalcOnGround();
		
		ApplyGravity(dt);
		updateFriction();
		ApplyPendingMoves();
		PxExtendedVec3 curr_pos = pActor->getFootPosition();
		//PxVec3 up_v = pActor->getUpDirection();	//get rotation
		CEntity *e = CHandle(this).getOwner();
		TCompTransform *tmx = e->get<TCompTransform>();
		tmx->setPosition(PxExVec3ToVec3(curr_pos));
		//tmx->setRotation(PxQuatToCQuaternion(curr_pose.q));
	}
}

void TCompCharacterController::onShapeHit(const PxControllerShapeHit & hit)
{
}

void TCompCharacterController::onControllerHit(const PxControllersHit & hit)
{
}

void TCompCharacterController::onObstacleHit(const PxControllerObstacleHit & hit)
{
}
