#ifndef INC_COMP_CHARACTER_CONTROLLER_H_
#define INC_COMP_CHARACTER_CONTROLLER_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "physx\physx_manager.h"

using namespace PhysxConversion;

//Character controller, for now only support to capsules collision
class TCompCharacterController : public TCompBase {
	float							m_radius			= 0.0f;
	float							m_height			= 0.0f;
	float							m_mass				= 80.0f;
	float							m_gravitySpeed		= -10.0f;
	float							m_maxSpeed			= 10.0f;
	float							m_friction			= 15.0f;
	float							m_eOffsetSpeed		= 0.2f;

	bool							m_active			= true;
	bool							m_affectGravity		= true;
	bool							m_physxOnground		= false;
	bool							m_OnGround			= false;
	bool							m_lastOnGround		= m_OnGround;

	VEC3							m_toMove			= VEC3(0.0f, 0.0f, 0.0f);
	VEC3							m_accel				= VEC3(0.0f,0.0f,0.0f);
	VEC3							m_speed				= VEC3(0.0f, 0.0f, 0.0f);
	PxVec3							m_last_speed		= PxVec3(0.0f,0.0f,0.0f);

	PxController*					m_pActor			= nullptr;

	PxFilterData					m_filter			= PxFilterData();

	PxControllerCollisionFlags		m_flagsCollisions;
	PxControllerFilters				m_filterController	= PxControllerFilters(&m_filter, g_PhysxManager);


	std::string name = "defaul name";


	void UpdateTags();

	//-----------------------------------------------------------------------------------------------------
	//							Internal update movement values
	//-----------------------------------------------------------------------------------------------------

	void UpdateFriction(float dt);
	void RecalcOnGround();
	void RecalcSpeed(float dt);
	void RecalcMovement(float dt);
	void ApplyPendingMoves(float dt);


	void UpdateMeshTransform();
	

public:
	// physics id
	TCompCharacterController() : m_pActor(nullptr){}
	~TCompCharacterController() {
		// Destroy physics obj

		if (m_pActor)	m_pActor->release();
	}

	// load Xml
	void onCreate(const TMsgEntityCreated&);

	//onread xml
	bool load(MKeyValue& atts);
	PxController* GetController() {
		return m_pActor;
	}

	void update(float dt);
	void renderInMenu();

	//-----------------------------------------------------------------------------------------------------
	//									getters
	//-----------------------------------------------------------------------------------------------------

	bool OnGround() const { return m_OnGround; }
	float GetYAxisSpeed() const {
		return m_speed.y;
	}
	float GetHeight() const{
		return m_height;
	}
	float GetRadius() const {
		return m_radius;
	}

	VEC3 GetPosition() const {
		return PhysxConversion::PxExVec3ToVec3(m_pActor->getPosition());
	}

	VEC3 GetFootPosition() const {
		return PhysxConversion::PxExVec3ToVec3(m_pActor->getFootPosition());
	}

	//-----------------------------------------------------------------------------------------------------
	//									physics setters
	//-----------------------------------------------------------------------------------------------------

	void teleport(const PxVec3 & pos);
	void teleport(const VEC3 & pos);
	void SetCollisions(bool new_collisions);
	void SetActive(bool isActive) { m_active = isActive; }
	void SetGravity(bool isActive) { m_affectGravity = isActive; }

	//-----------------------------------------------------------------------------------------------------
	//									Movement modifiers
	//-----------------------------------------------------------------------------------------------------

	void AddSpeed(const VEC3& direction, float speed = 1.0f);
	void AddAccel(const VEC3& direction, float accel = 1.0f);
	void AddMovement(const VEC3& direction, float speed = 1.0f);
	void AddImpulse(const VEC3 & impulse);
	
};


#endif