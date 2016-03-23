#include "mcv_platform.h"
#include "comp_charactercontroller.h"

#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"

#include "app_modules\io\io.h"

void TCompCharacterController::ApplyGravity(float dt)
{
	if (mAffectGravity) {
		if (!OnGround() || mSpeedYAxis != 0.0f) {
			dt = getDeltaTime();
			mSpeedYAxis += -9.8f*dt;	//update y-axis speed with gravity
			mFlagsCollisions = pActor->move(PxVec3(0, mSpeedYAxis, 0)*dt, 0.0f, dt, PxControllerFilters());
			if (OnGround())
				mSpeedYAxis = 0.0f;
		}
	}
}

void TCompCharacterController::AddImpulse(const VEC3& impulse) {
	mSpeedYAxis += impulse.y;
	if (mSpeedYAxis > mMaxYimpulse)		//check if acum impulse passed max impulse
		mSpeedYAxis = mMaxYimpulse;		
	mToMove.x += impulse.x;
	mToMove.z += impulse.z;
}

void TCompCharacterController::ApplyPendingMoves() {
	if (mToMove != VEC3(0.0f, 0.0f, 0.0f)) {
		float dt = getDeltaTime();
		if(OnGround())mFlagsCollisions = pActor->move(PxVec3(mToMove.x, mToMove.y, mToMove.z), 0.0f, dt, PxControllerFilters());
		else mFlagsCollisions = pActor->move(PxVec3(mToMove.x, mToMove.y, mToMove.z)/2, 0.0f, dt, PxControllerFilters());
		mToMove = VEC3(0.0f,0.0f,0.0f);
	}
}

void TCompCharacterController::AddMovement(const VEC3& direction, float speed) {

	mToMove += direction*speed;
}

bool TCompCharacterController::load(MKeyValue & atts)
{
	mRadius = atts.getFloat("radius", 0.5f);
	mHeight = atts.getFloat("height", 0.5f);
	return true;
}

bool TCompCharacterController::OnGround() {

	if (mFlagsCollisions & PxControllerFlag::eCOLLISION_DOWN)
		return true;

	return false;
}

VEC3 TCompCharacterController::getPosition()
{
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
	if (mActive) {
		ApplyGravity(dt);
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
