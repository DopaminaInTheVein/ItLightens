#ifndef INC_PHSYX_MANAGER_H_
#define	INC_PHSYX_MANAGER_H_

#include "app_modules\app_module.h"
#include "ItLightensFilterShader.h"

#define DEFAULT_DATA_DYNAMIC	g_PhysxManager->GetDefaultQueryTagsDynamic()
#define DEFAULT_DATA_STATIC		g_PhysxManager->GetDefaultQueryTagsStatic()
#define DEFAULT_DATA_CC			g_PhysxManager->GetDefaultQueryTagsCC()

#define PX_SAFE_RELEASE(p)  if (p) p->release(), p = nullptr


#define PXM_NO_PLAYER_CRYSTAL ItLightensFilter::eLIQUID | ItLightensFilter::ePLATFORM | ItLightensFilter::eGUARD | ItLightensFilter::ePOSSESSABLE | ItLightensFilter::eBOMB | ItLightensFilter::eOBJECT | ItLightensFilter::eSCENE
#define PXM_NO_CRYSTAL ItLightensFilter::ePLAYER_CONTROLLED | ItLightensFilter::ePLATFORM | ItLightensFilter::ePLAYER_BASE | ItLightensFilter::eLIQUID | ItLightensFilter::eGUARD | ItLightensFilter::ePOSSESSABLE | ItLightensFilter::eBOMB | ItLightensFilter::eOBJECT | ItLightensFilter::eSCENE
#define PXM_NO_PLAYER ItLightensFilter::eLIQUID | ItLightensFilter::ePLATFORM | ItLightensFilter::eCRYSTAL | ItLightensFilter::eGUARD | ItLightensFilter::ePOSSESSABLE | ItLightensFilter::eBOMB | ItLightensFilter::eOBJECT | ItLightensFilter::eSCENE
#define PXM_ALL_LESS_STATIC ItLightensFilter::ePLAYER_CONTROLLED | ItLightensFilter::ePLATFORM | ItLightensFilter::ePLAYER_BASE | ItLightensFilter::eLIQUID | ItLightensFilter::eCRYSTAL | ItLightensFilter::eGUARD | ItLightensFilter::ePOSSESSABLE | ItLightensFilter::eBOMB | ItLightensFilter::eOBJECT
#define PXM_CAMERA_COLLISIONS ItLightensFilter::eSCENE //| ItLightensFilter::eOBJECT

#define GRAVITY -10.0f

using namespace physx;

class CMesh;
class CEntity;
class CHandle;

/*class PxAllocatorCallback
{
public:
	virtual ~PxAllocatorCallback() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename,
		int line) = 0;
	virtual void deallocate(void* ptr) = 0;
};

class UserErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file,
		int line)
	{
		fatal("PHYSX ERROR - %d: %s\n",code,message);
	}
};*/

class CPhysxManager :	public IAppModule,
						public PxSimulationEventCallback,
						public PxSimulationFilterCallback,
						public PxControllerBehaviorCallback,
						public PxUserControllerHitReport, 
						public PxQueryFilterCallback {


	PxFoundation			*m_pFoundation				= nullptr;
	PxProfileZoneManager	*m_pProfileZoneManager		= nullptr;
	PxPhysics				*m_pPhysics					= nullptr;
	PxCooking				*m_pCooking					= nullptr;
	PxScene					*m_pScene					= nullptr;
	PxDefaultCpuDispatcher  *m_pCpuDispatcher			= nullptr;

	PxControllerManager		*m_pManagerControllers		= nullptr;		//Characters controllers manager

	PxCudaContextManager	*m_pCudaContextManager		= nullptr;

	PxGeometryQuery			*m_pGeomQuerys				= nullptr;


#ifndef NDEBUG
	PxVisualDebuggerConnection *m_pConnection			= nullptr;		//physx debugger
#endif

	PxReal t_to_update = 0.0f;
	PxReal t_max_update = 0.0f;


	//memory raycast, used for point-to-point
	PxReal m_last_distance = 0.0f;

	PxVec3 m_last_direction = PxVec3(0.0f, 0.0f, 0.0f);
	PxVec3 m_last_origin = PxVec3(0.0f, 0.0f, 0.0f);
	PxVec3 m_last_end = PxVec3(0.0f, 0.0f, 0.0f);

	//default filter data
	PxFilterData m_ft_dynamic;
	PxFilterData m_ft_static;
	PxFilterData m_ft_cc;

	void setFtDynamic();
	void setFtStatic();
	void setFtCC();

	int m_NbThreads = 1;

	//-----------------------------------------------------------------------------------------------------
	//							Customize functions
	//-----------------------------------------------------------------------------------------------------

	//customize options
	void customizeSceneDesc(PxSceneDesc & sceneDesc);

public:

	CPhysxManager() :	m_pCooking(nullptr),
						m_pCpuDispatcher(nullptr),
						m_pCudaContextManager(nullptr),
						m_pFoundation(nullptr),
						m_pManagerControllers(nullptr),
						m_pPhysics(nullptr),
						m_pProfileZoneManager(nullptr),
						m_pScene(nullptr),
						m_pGeomQuerys(nullptr)
	{}

	~CPhysxManager() { stop(); }

	//runtime funcions

	bool					start() override;
	void					stop() override;
	void					update(float dt) override;

	//set flags
	void					setupFiltering(PxRigidActor * actor, PxFilterData& filterData);

	//gets

	PxControllerManager*	GetCharacterControllerManager() const { return m_pManagerControllers; }
	PxScene*				GetActiveScene() { return m_pScene; }

	PxFilterData			GetDefaultQueryTagsDynamic() { return m_ft_dynamic; }
	PxFilterData			GetDefaultQueryTagsStatic() { return m_ft_static; }
	PxFilterData			GetDefaultQueryTagsCC() { return m_ft_cc; }

	//-----------------------------------------------------------------------------------------------------
	//							Primitives Gemoetries
	//-----------------------------------------------------------------------------------------------------

	void					CreateSphereGeometry(const PxReal& radius, PxSphereGeometry& g) const;
	void					CreateBoxGeometry(const PxVec3& size, PxBoxGeometry& g) const;
	void					CreateCapsuleGeometry(const PxReal& radius, const PxReal& halfheight, PxCapsuleGeometry& g) const;
	void					CreatePlaneGeometry(const PxReal& radius, const PxReal& halfheight, PxPlaneGeometry& g) const;


	//-----------------------------------------------------------------------------------------------------
	//							Primitives PxShapes
	//-----------------------------------------------------------------------------------------------------


	/**
	\brief							Create and return the shape of a sphere with material.

	\param[in] radius				Radius of the sphere
	\param[in] staticfriction		Friction when object at rest and someone trying to move it.
	\param[in] dynamicfriction		Friction when object is moving.
	\param[in] restitution			Quantity of energy absorbed on contact. Bigger values, bigger the rebound.
	\return							Pointer to a sphere shape.
	*/
	PxShape*						CreatePxSphere	(PxReal radius, PxReal staticFriction = 0.5f, PxReal dynamicFriction = 0.5f, PxReal restitution = 0.5f);
	
	/**
	\brief							Create and return the shape of a capsule with material.

	\param[in] radius				Radius of the sphere
	\param[in] halheight			Half of the cilinder of the capsule
	\param[in] staticfriction		Friction when object at rest and someone trying to move it.
	\param[in] dynamicfriction		Friction when object is moving.
	\param[in] restitution			Quantity of energy absorbed on contact. Bigger values, bigger the rebound.
	\return							Pointer to a capsule shape.
	*/
	PxShape*						CreatePxCapsule(PxReal radius, PxReal halfHeight, PxReal staticFriction = 0.5f, PxReal dynamicFriction = 0.5f, PxReal restitution = 0.5f);
	
	/**
	\brief							Create and return the shape of a box with material.

	\param[in] size					Vector of the size of the box. WARNING: size is from center
	\param[in] staticfriction		Friction when object at rest and someone trying to move it.
	\param[in] dynamicfriction		Friction when object is moving.
	\param[in] restitution			Quantity of energy absorbed on contact. Bigger values, bigger the rebound.
	\return							Pointer to a box shape.
	*/
	PxShape*						CreatePxBox	(const PxVec3& size_from_center = PxVec3(1,1,1), PxReal staticFriction = 0.5f, PxReal dynamicFriction = 0.5f, PxReal restitution = 0.5f);



	//-----------------------------------------------------------------------------------------------------
	//										complex shapes
	//-----------------------------------------------------------------------------------------------------

	PxShape*			CreateTriangleMesh(PxTriangleMesh *mesh, PxReal staticFriction = 0.5f, PxReal dynamicFriction = 0.5f, PxReal restitution = 0.5f);
	PxShape*			CreateConvexShape(const CMesh * mesh, PxReal staticFriction = 0.5f, PxReal dynamicFriction = 0.5f, PxReal restitution = 0.5f);
	

	
	//-----------------------------------------------------------------------------------------------------
	//							game components, create and add to scene
	//-----------------------------------------------------------------------------------------------------

	/**
	\param[in] actor		PxActor to add at the scene.
	*/
	void					AddToActiveScene(PxActor& actor);

	/**
	\brief					Create and add a CHaracter controller to the scene.

	\param[in] radius		Radius of the capsule.
	\param[in] height		Half of the cilinder of the capsule
	\return					Pointer to the character controller created.
	*/
	PxController*			CreateCharacterController(PxReal radius, PxReal height);

	/**
	\brief					Create and add a dynamic actor to the scene.

	\param[in] transform	Transform with the initial position and rotation
	\param[in] shape		Shape of the collider
	\return					Pointer to the actor added to the scene.
	*/
	PxActor*				CreateAndAddRigidDynamic(const PxTransform *transform, PxShape *shape, PxReal density);

	/**
	\brief					Create and add a static actor to the scene.

	\param[in] transform	Transform with the initial position and rotation.
	\param[in] shape		Shape of the collider.
	\return					Pointer to the actor added to the scene.
	*/
	PxActor*				CreateAndAddRigidStatic	(const PxTransform *transform, PxShape *shape);

	/**
	\brief					Create and add a trigger actor to the scene.

	\param[in] transform	Transform with the initial position and rotation.
	\param[in] shape		Shape of the collider for the trigger.
	\return					Pointer to the actor added to the scene.
	*/
	PxActor *				CreateAndAddTrigger		(const PxTransform *transform, PxShape* shape);

	//-----------------------------------------------------------------------------------------------------
	//								Cooking meshes
	//-----------------------------------------------------------------------------------------------------

	PxTriangleMesh *		CreateCookedTriangleMesh(const CMesh * mesh);
	PxConvexMesh *			CreateCookedConvexMesh(const CMesh * mesh);

	//-----------------------------------------------------------------------------------------------------
	//						Messages callbacks, virtuals from Physx
	//-----------------------------------------------------------------------------------------------------

	void					onTrigger(PxTriggerPair * pairs, PxU32 count);
	void					onWake(PxActor **actors, PxU32 count);
	void					onSleep(PxActor **actors, PxU32 count);
	void					onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs);
	void					onConstraintBreak(PxConstraintInfo *constraints, PxU32 count);

	//-----------------------------------------------------------------------------------------------------
	//						virtuals from PxQueryFilterCallback
	//-----------------------------------------------------------------------------------------------------

	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) {
		if (filterData.word1 & ItLightensFilter::eCOLLISION)
			return PxSceneQueryHitType::eBLOCK;
		else
			return PxSceneQueryHitType::eNONE;
	}
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) {
		if (filterData.word1 & ItLightensFilter::eCOLLISION)
			return PxSceneQueryHitType::eBLOCK;
		else
			return PxSceneQueryHitType::eNONE;
	}

	//-----------------------------------------------------------------------------------------------------
	//						virtuals from PxSimulationFilterCallback
	//-----------------------------------------------------------------------------------------------------
	virtual  PxFilterFlags pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor *a0, const PxShape *s0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor *a1, const PxShape *s1, PxPairFlags &pairFlags);
	virtual void pairLost(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, bool objectRemoved) {}
	virtual bool statusChange(PxU32 &pairID, PxPairFlags &pairFlags, PxFilterFlags &filterFlags) { return false; }
	
	//-----------------------------------------------------------------------------------------------------
	//						virtuals from PxUserControllerHitReport
	//-----------------------------------------------------------------------------------------------------

	virtual void 	onShapeHit(const PxControllerShapeHit &hit) {}
	virtual void 	onControllerHit(const PxControllersHit &hit) {}
	virtual void 	onObstacleHit(const PxControllerObstacleHit &hit) {}

	//-----------------------------------------------------------------------------------------------------
	//						virtuals from PxControllerBehaviorCallback
	//-----------------------------------------------------------------------------------------------------

	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) {
		//TODO: filter platforms
		return PxControllerBehaviorFlag::eCCT_SLIDE | PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
	}
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) {
		return PxControllerBehaviorFlags(0);
	}
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) {
		//TODO: filter platforms
		return PxControllerBehaviorFlag::eCCT_SLIDE | PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
	}

	//-----------------------------------------------------------------------------------------------------
	//								scene querys (raycast)
	//-----------------------------------------------------------------------------------------------------

	/**
	\param[in] origin					Origin of the ray.
	\param[in] unitDir					Normalized direction of the ray.
	\param[in] distance					Length of the ray. Has to be in the [0, inf) range.
	\param[out] hitCall					Raycast hit buffer or callback object used to report raycast hits.
	\param[in] filterData(optional)		Tags against who will be checked.
	\param[in] hitFlags(optional)		Specifies which properties per hit should be computed and returned via the hit callback.
	
	\return True if any touching or blocking hits were found or any hit was found in case PxQueryFlag::eANY_HIT was specified.
	*/
	bool raycast(PxVec3 origin, PxVec3 unitDir, PxReal maxDistance, PxRaycastBuffer& hit,
							PxQueryFilterData filterData = PxQueryFilterData(), 
							const PxHitFlags outputFlags = PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);

	/**
	\param[in] origin					Origin of the ray.
	\param[in] unitDir					Normalized direction of the ray.
	\param[in] distance					Length of the ray. Has to be in the [0, inf) range.
	\param[out] hitCall					Raycast hit buffer or callback object used to report raycast hits.
	\param[in] filterData(optional)		Tags against who will be checked.
	\param[in] hitFlags(optional)		Specifies which properties per hit should be computed and returned via the hit callback.

	\return True if any touching or blocking hits were found or any hit was found in case PxQueryFlag::eANY_HIT was specified.
	*/
	bool raycast(VEC3 origin, VEC3 unitDir, PxReal maxDistance, PxRaycastBuffer& hit,
							PxQueryFilterData filterData = PxQueryFilterData(),
							const PxHitFlags outputFlags = PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);


	/**
	\brief								Sqrt calc, use direction if you can
	\param[in] origin					Origin of the ray.
	\param[in] end						End of the ray.
	\param[out] hitCall					Raycast hit buffer or callback object used to report raycast hits.
	\param[in] filterData(optional)		Tags against who will be checked
	\param[in] hitFlags(optional)		Specifies which properties per hit should be computed and returned via the hit callback.

	\return True if any touching or blocking hits were found or any hit was found in case PxQueryFlag::eANY_HIT was specified.
	*/
	bool raycast(PxVec3 origin, PxVec3 end, PxRaycastBuffer& hit,
							PxQueryFilterData filterData = PxQueryFilterData(),
							const PxHitFlags outputFlags = PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);



	/**
	\brief								WARNING: high cost. Sqrt calc, use direction if you can
	\param[in] origin					Origin of the ray.
	\param[in] end						End of the ray.
	\param[out] hitCall					Raycast hit buffer or callback object used to report raycast hits.
	\param[in] filterData(optional)		Tags against who will be checked
	\param[in] hitFlags(optional)		Specifies which properties per hit should be computed and returned via the hit callback.

	\return True if any touching or blocking hits were found or any hit was found in case PxQueryFlag::eANY_HIT was specified.
	*/
	bool raycast(VEC3 origin, VEC3 end, PxRaycastBuffer& hit,
							PxQueryFilterData filterData = PxQueryFilterData(),
							const PxHitFlags outputFlags = PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);


	bool raySphere(PxReal radius, PxVec3 & start, PxVec3 & direction, PxReal distance, PxSweepCallback & hit,
																					PxQueryFilterData filter = PxQueryFilterData(), 
																					const PxHitFlags outputflags = PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);

	bool raySphere(PxReal radius, VEC3 & start, VEC3 & direction, PxReal distance, PxSweepCallback & hit,
																					PxQueryFilterData filter = PxQueryFilterData(),
																					const PxHitFlags outputflags = PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);

	float SquaredDistancePointToGeometry(const PxVec3& point, const PxGeometry& geometry, const PxVec3& originGeometry, const PxQuat& quaternionGeometry = PxQuat(0, 0, 0, 1));
	float SquaredDistancePointToGeometry(const VEC3& point, const PxGeometry& geometry, const VEC3& originGeometry, const CQuaternion& quaternionGeometry = CQuaternion(0, 0, 0, 1));


	//-----------------------------------------------------------------------------------------------------
	//								joints
	//-----------------------------------------------------------------------------------------------------

	enum typeJoints {
		eFixed = 0,
		eDistance,
		eSpherical,
		eRevolute,
		ePrismatic,
		eD6,
	};

	//Not needed for now
	//bool Createjoint(PxRigidActor* a1, PxRigidActor* a2, const PxTransform& tmx1, const PxTransform& tmx2, int typeJoint= eFixed);



	//-----------------------------------------------------------------------------------------------------
	//								Particles
	//-----------------------------------------------------------------------------------------------------

	PxParticleSystem * CreateParticleSystem(int max_particles);

	//name for module
	const char* getName() const {
		return "physx";
	}
};


//-----------------------------------------------------------------------------------------------------
//							nameSpace for conversions
//-----------------------------------------------------------------------------------------------------

namespace PhysxConversion {
	PxVec3			Vec3ToPxVec3		(const VEC3& vec);
	VEC3			PxVec3ToVec3		(const PxVec3& vec);
	PxExtendedVec3	Vec3ToPxExVec3		(const VEC3& vec);
	VEC3			PxExVec3ToVec3		(const PxExtendedVec3& vec);
	PxQuat			CQuaternionToPxQuat	(const CQuaternion& quat);
	CQuaternion		PxQuatToCQuaternion	(const PxQuat& quat);
	VEC4			PxVec4ToVec4		(const PxVec4& vec);
	PxVec4			VEC4ToPxVec4		(const VEC4& vec);

	PxTransform		ToPxTransform		(const VEC3& pos, const CQuaternion& rot);

	CHandle			GetEntityHandle		(const PxActor& a);
	
}


#endif


