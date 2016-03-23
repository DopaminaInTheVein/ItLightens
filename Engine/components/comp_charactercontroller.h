#ifndef INC_COMP_CHARACTER_CONTROLLER_H_
#define INC_COMP_CHARACTER_CONTROLLER_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "physx\physx_manager.h"

using namespace PhysxConversion;

//Character controller, for now only support to capsules collision
class TCompCharacterController : public TCompBase, public PxUserControllerHitReport {
	float mRadius		= 0.0f;
	float mHeight		= 0.0f;
	float mSpeedYAxis	= 0.0f;
	float mMaxYimpulse	= 5.0f;		//max impulse for yaxis, to block acumulating impulse forces
	bool mActive = true;
	bool mAffectGravity = true;

	VEC3 mToMove = VEC3(0.0f,0.0f,0.0f);

	PxController* pActor = nullptr;

	PxControllerCollisionFlags mFlagsCollisions;

	void ApplyGravity(float dt);

	void ApplyPendingMoves();

	

public:
	// physics id
	TCompCharacterController() {
	}
	~TCompCharacterController() {
		// Destroy physics obj

		if (pActor)	pActor->release();
	}

	bool load(MKeyValue& atts);
	PxController* GetController() {
		return pActor;
	}
	bool OnGround();
	float GetYAxisSpeed() const {
		return mSpeedYAxis;
	}
	float GetHeight() const{
		return mHeight;
	}
	float GetRadius() const {
		return mRadius;
	}

	// load Xml
	void onCreate(const TMsgEntityCreated&);

	void updateTags(PxFilterData filter);

	void AddMovement(const VEC3& direction, float speed = 1.0f);
	void AddImpulse(const VEC3 & impulse);
	void SetActive(bool isActive) { mActive = isActive; }
	void SetGravity(bool isActive) { mAffectGravity = isActive; }
	VEC3 getPosition();
	void update(float dt);

	//virtual fucntions from PxUserControllerHitReport
	void onShapeHit(const PxControllerShapeHit &hit);
	void onControllerHit(const PxControllersHit &hit);
	void onObstacleHit(const PxControllerObstacleHit &hit);
};


#endif