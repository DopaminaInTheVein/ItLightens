#ifndef INC_COMPONENT_PHYSICS_H_
#define INC_COMPONENT_PHYSICS_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "physx\physx_manager.h"

using namespace PhysxConversion;

enum eTypeCollision {
	STATIC_OBJECT = 0,
	DYNAMIC_RB,
	TRIGGER,
};

enum eTypeShape {
	TRI_MESH = 0,	//by default will take the mesh form the object
	SPHERE,
	CAPSULE,
	BOX,
	CONVEX,		//not implemented yet
};

// ------------------------------------
class TCompPhysics : public TCompBase {

	int mCollisionType;
	int mCollisionShape;

	float mStaticFriction = 0.5f;
	float mDynamicFriction = 0.5f;
	float mRestitution = 0.25f;

	float			mRadius		= 0.0f;
	float			mHeight		= 0.0f;
	VEC3			mSize		= VEC3(1.0f,1.0f,1.0f);	//default size for box

	PxShape*		pShape		= nullptr;
	PxActor*		pActor		= nullptr;
	PxRigidActor*	rigidActor	= nullptr;

	int getCollisionTypeValueFromString(std::string str);
	int getCollisionShapeValueFromString(std::string str);

	bool createTriMeshShape();
	bool createBoxShape();
	bool createCapsuleShape();
	bool createSphereShape();
	bool createConvexShape();

	bool addRigidbodyScene();

public:
	// physics id
	TCompPhysics() {
	}
	~TCompPhysics() {
		// Destroy physics obj
		if (pActor) {
			PhysxManager->GetActiveScene()->removeActor(*pActor);
			pActor->release();
		}

		if (rigidActor) rigidActor->release();

		if (pShape)	pShape->release();
	}


	void updateTags(PxFilterData filter);

	PxActor* getActor() { return pActor; }
	bool load(MKeyValue& atts);

	// load Xml
	void onCreate(const TMsgEntityCreated&);

	void update(float dt);
	void renderInMenu();
};

#endif