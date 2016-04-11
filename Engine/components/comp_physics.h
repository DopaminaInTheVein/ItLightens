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
	CONVEX,	
};

// ------------------------------------
class TCompPhysics : public TCompBase {

	int					m_collisionType;
	int					m_collisionShape;

	float				m_staticFriction		= 0.5f;
	float				m_dynamicFriction		= 0.5f;
	float				m_restitution			= 0.25f;
	float				m_mass					= 2.0f;

	float				m_radius				= 0.0f;
	float				m_height				= 0.0f;
	VEC3				m_size					= VEC3(1.0f,1.0f,1.0f);	//default size for box

	PxShape*			m_pShape				= nullptr;
	PxActor*			m_pActor				= nullptr;
	PxRigidActor*		m_pRigidActor			= nullptr;

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
		if (m_pActor) {
			g_PhysxManager->GetActiveScene()->removeActor(*m_pActor);
			m_pActor->release();
		}

		//if (rigidActor) rigidActor->release();
	}

	void updateTagsSetupActor(PxFilterData& filter);

	PxActor* getActor() { return m_pActor; }
	bool load(MKeyValue& atts);

	// load Xml
	void onCreate(const TMsgEntityCreated&);

	bool isKinematic();
	bool setKinematic(bool isKinematic);
	void AddForce(VEC3 force);
	void AddVelocity(VEC3 velocity);
	void setPosition(VEC3 position, CQuaternion rotation);

	float GetMass() const { return m_mass; }

	void update(float dt);
	void renderInMenu();
	void readIniFileAttr();
	void updateAttrMaterial();
	void writeIniFileAttr();
};

#endif