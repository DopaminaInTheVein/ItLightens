#include "mcv_platform.h"
#include "comp_charactercontroller.h"

#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"

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
	else mSpeed.x = 0;
	if(mSpeed.z) mSpeed.z -= mSpeed.z*mFriction*dt;
	else mSpeed.z = 0;
}

void TCompCharacterController::ApplyPendingMoves() {
	PROFILE_FUNCTION("apply pending moves");
	float dt = getDeltaTime();
	mToMove += mSpeed;
	if (mToMove != VEC3(0.0f, 0.0f, 0.0f)) {
		PxVec3 moved = PxVec3(mToMove.x, mToMove.y, mToMove.z)*dt;
		if(OnGround())mFlagsCollisions = pActor->move(moved, 0, dt, mFilter);
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
		TCompTransform *mtx = e->get<TCompTransform>();
		PxExtendedVec3 p = Vec3ToPxExVec3(mtx->getPosition());
		p.y += mHeight + mRadius;	//add height value from capsule, center from collider at center of the shape
		pActor->setPosition(p);
		pActor->getActor()->userData = e;
		updateTags(DEFAULT_DATA_CC);
	}
}

void TCompCharacterController::updateTags(PxFilterData filter)
{
	//PROFILE_FUNCTION("update tags");
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("target")) //player
			filter.word0 = filter.word0 | CPhysxManager::ePLAYER_BASE;

		if (h.hasTag("AI_guard"))
			filter.word0 = filter.word0 | CPhysxManager::eGUARD | CPhysxManager::eNPC;

		if (h.hasTag("AI_poss"))
			filter.word0 = filter.word0 | CPhysxManager::ePOSSEABLE | CPhysxManager::eNPC;
	}

	PxShape **ptr;
	ptr = new PxShape*[1];
	pActor->getActor()->getShapes(ptr, 1);
	ptr[0]->setQueryFilterData(filter);

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
