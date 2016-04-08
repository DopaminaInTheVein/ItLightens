#ifndef INC_COMP_CHARACTER_CONTROLLER_H_
#define INC_COMP_CHARACTER_CONTROLLER_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "physx\physx_manager.h"

using namespace PhysxConversion;

//Character controller, for now only support to capsules collision
class TCompCharacterController : public TCompBase {
	float							mRadius				= 0.0f;
	float							mHeight				= 0.0f;
	float							mSpeedYAxis			= 0.0f;
	float							mMaxYimpulse		= 7.0f;		//max impulse for yaxis, to block acumulating impulse forces
	float							mFriction			= 15.0f;
	float							eOffsetSpeed		= 0.02f;

	bool							mActive				= true;
	bool							mAffectGravity		= true;
	bool							mOnGround			= false;

	VEC3							mToMove				= VEC3(0.0f, 0.0f, 0.0f);
	VEC3							mSpeed				= VEC3(0.0f, 0.0f, 0.0f);

	PxController*					pActor				= nullptr;

	PxFilterData					mFilter				= PxFilterData();

	PxControllerCollisionFlags		mFlagsCollisions;
	PxControllerFilters				mFilterController	= PxControllerFilters(&mFilter, PhysxManager);

	

	void ApplyGravity(float dt);

	void ApplyPendingMoves();
	void recalcOnGround();

	std::string name = "defaul name";

	

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
	bool OnGround() const { return mOnGround; }
	float GetYAxisSpeed() const {
		return mSpeed.y;
	}
	float GetHeight() const{
		return mHeight;
	}
	float GetRadius() const {
		return mRadius;
	}

	// load Xml
	void onCreate(const TMsgEntityCreated&);

	void updateTags();

	void teleport(const PxVec3 & pos);

	void SetCollisions(bool new_collisions);

	void AddMovement(const VEC3& direction, float speed = 1.0f);
	void AddImpulse(const VEC3 & impulse);
	void updateFriction();
	void SetActive(bool isActive) { mActive = isActive; }
	void SetGravity(bool isActive) { mAffectGravity = isActive; }
	VEC3 getPosition();
	void update(float dt);
};


#endif