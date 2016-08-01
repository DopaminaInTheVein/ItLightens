#ifndef INC_COMPONENT_PHYSICS_H_
#define INC_COMPONENT_PHYSICS_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "physx/physx_manager.h"
#include "Physx/ItLightensFilterShader.h"

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
	// Provisional?
	DRONE,
};

//Forward declaration
class TCompTransform;

// ------------------------------------
class TCompPhysics : public TCompBase {
	int					m_collisionType;
	int					m_collisionShape;
	bool				m_kinematic;

	float				m_staticFriction = 0.5f;
	float				m_dynamicFriction = 0.5f;
	float				m_restitution = 0.25f;
	float				m_mass = 2.0f;

	float				m_radius = 0.0f;
	float				m_height = 0.0f;
	VEC3				m_size = VEC3(1.0f, 1.0f, 1.0f);	//default size for box

	PxShape*			m_pShape = nullptr;
	PxActor*			m_pActor = nullptr;
	PxRigidActor*		m_pRigidActor = nullptr;

	float				m_smooth = 0.0f;

	int getCollisionTypeValueFromString(std::string str);
	int getCollisionShapeValueFromString(std::string str);

	bool createTriMeshShape();
	bool createBoxShape();
	bool createCapsuleShape();
	bool createSphereShape();
	bool createConvexShape();
	bool createDroneShape();

	bool addRigidbodyScene();

	void updateTagsSetupActor();
	void updateTagsSetupActor(PxFilterData& filter);
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

	PxActor* getActor() { return m_pActor; }
	PxRigidActor* getRigidActor() { return m_pRigidActor; }
	PxShape* getShape() { return m_pShape; }

	// load Xml
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);

	void setBehaviour(ItLightensFilter::descObjectBehaviour tag, bool enabled);
	VEC3 getLinearVelocity();
	void setLinearVelocity(VEC3 speed);
	VEC3 getAngularVelocity();
	void setAngularVelocity(VEC3 speed);
	void setGravity(bool);

	bool isKinematic();
	bool setKinematic(bool isKinematic);
	void AddMovement(VEC3 movement);
	void DisableRotationXZ();
	void AddForce(VEC3 force);
	void ClearForces();
	void AddVelocity(VEC3 velocity);
	void setPosition(TCompTransform *);
	void setPosition(VEC3 position, CQuaternion rotation);

	float GetMass() const { return m_mass; }

	void update(float dt);
	void renderInMenu();
	void readIniFileAttr();
	void updateAttrMaterial();
	PxFilterData getFilterData();
	void writeIniFileAttr();
};

#endif