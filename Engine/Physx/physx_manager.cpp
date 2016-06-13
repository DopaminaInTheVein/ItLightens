#include "mcv_platform.h"
#include "physx_manager.h"
#include "handle\object_manager.h"
#include "render\mesh.h"
#include "components\entity.h"
#include "components\comp_name.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_physics.h"

#include "components\entity_tags.h"
#include "app_modules\entities.h"


void CPhysxManager::setFtDynamic()
{	
	m_ft_dynamic.word0 = 0;
	m_ft_dynamic.word1 = ItLightensFilter::eALL;
	m_ft_dynamic.word2 = ItLightensFilter::eCAN_TRIGGER | ItLightensFilter::eCOLLISION;
}

void CPhysxManager::setFtStatic()
{
	m_ft_static.word0 = 0;
	m_ft_static.word1 = PXM_ALL_LESS_STATIC;
	m_ft_static.word2 = ItLightensFilter::eCOLLISION;
}

void CPhysxManager::setFtCC()
{
	m_ft_cc.word0 = 0;
	m_ft_cc.word1 = ItLightensFilter::eALL;
	m_ft_cc.word2 = ItLightensFilter::eCAN_TRIGGER | ItLightensFilter::eCOLLISION;
}

//#########################################################################################################
//									runtime functions
//#########################################################################################################
#pragma region runtime functions

//start function: called at start from engine, to init PhysxManager
bool CPhysxManager::start()
{

	static PxDefaultErrorCallback	gDefaultErrorCallback;
	static PxDefaultAllocator		gDefaultAllocatorCallback;
	PxTolerancesScale scale = PxTolerancesScale();

	//init foundation
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);

	assert(m_pFoundation);
	if (!m_pFoundation)
		fatal("PxCreateFoundation failed!");

	//init ProfileZoneManager
	/*bool recordMemoryAllocations = true;
	m_pProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(m_pFoundation);
	if (!m_pProfileZoneManager)
		fatal("PxProfileZoneManager::createProfileZoneManager failed!");


	//init Physics
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation,
		scale, recordMemoryAllocations, m_pProfileZoneManager);
		*/

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation,
		scale);

	assert(m_pPhysics);
	if (!m_pPhysics)
		fatal("PxCreatePhysics failed!");

	//init cooking
	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES | PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, params);

	assert(m_pCooking);
	if (!m_pCooking)
		fatal("PxCreateCooking failed!");

	//init extensions
	if (!PxInitExtensions(*m_pPhysics))
		fatal("PxInitExtensions failed!");

	//init scene
	PxSceneDesc sceneDesc(scale);
	customizeSceneDesc(sceneDesc);

	if (!sceneDesc.cpuDispatcher)
	{
		m_pCpuDispatcher = PxDefaultCpuDispatcherCreate(m_NbThreads);
		if (!m_pCpuDispatcher)
			fatal("PxDefaultCpuDispatcherCreate failed!");

		assert(m_pCpuDispatcher);
		sceneDesc.cpuDispatcher = m_pCpuDispatcher;
	}
	
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = gDefaultFilterShader;


#ifdef PX_WINDOWS
	if (!sceneDesc.gpuDispatcher && m_pCudaContextManager)
	{
		sceneDesc.gpuDispatcher = m_pCudaContextManager->getGpuDispatcher();
	}
#endif

	m_pScene = m_pPhysics->createScene(sceneDesc);
	assert(m_pScene);
	if (!m_pScene)
		fatal("createScene failed!");

	//init managers characters controllers
	m_pManagerControllers = PxCreateControllerManager(*m_pScene);
	if (!m_pManagerControllers)
		fatal("manager controllers failed!");

	m_pGeomQuerys = new PxGeometryQuery();


#ifndef NDEBUG
	//Physx Debbuger
	if (m_pPhysics->getPvdConnectionManager() == NULL)
		return true;	// no Pvd support for debugging physx

	// setup connection parameters
	const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
	int             port = 5425;         // TCP port to connect to, where PVD is listening
	unsigned int    timeout = 100;          // timeout in milliseconds to wait for PVD to respond,
											// consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();

	// and now try to connect
	m_pConnection = PxVisualDebuggerExt::createConnection(m_pPhysics->getPvdConnectionManager(),
		pvd_host_ip, port, timeout, connectionFlags);

	m_pScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
#endif

	setFtDynamic();
	setFtStatic();
	setFtCC();

	return true;
}

//stop function: release memory
void CPhysxManager::stop()
{
	if(m_pPhysics)m_pPhysics->release();
	if(m_pFoundation)m_pFoundation->release();
	if(m_pCooking)m_pCooking->release();
	if(m_pProfileZoneManager)m_pProfileZoneManager->release();
	if (m_pManagerControllers) {
		//m_pManagerControllers->purgeControllers();	//TODO: free memory
	}

	if (m_pGeomQuerys) delete m_pGeomQuerys;

#ifndef NDEBUG
		
	//memory already free¿?
	/*if (mConnection) {
		if (mConnection->isConnected()) mConnection->disconnect();
		mConnection->release();
	}*/
#endif
}

//update function: to update at fixed rate
void CPhysxManager::update(float dt)
{
	//calculate fixed update
	t_to_update += getDeltaTime();
	if (t_to_update >= t_max_update) {

		CEntitiesModule::fixedUpdate(t_max_update);

		m_pScene->simulate(t_max_update);
		m_pScene->fetchResults(true);

		//getHandleManager<TCompPhysics>()->updateAll(t_max_update);
		//getHandleManager<TCompCharacterController>()->updateAll(t_max_update);

		t_to_update -= t_max_update;
		t_to_update = 0;
	}
}

#pragma endregion

//#########################################################################################################
//											customize functions
//#########################################################################################################
#pragma region customize functions

// set filter tags
void CPhysxManager::setupFiltering(PxRigidActor* actor, const PxFilterData& filterData)
{
	const PxU32 numShapes = actor->getNbShapes();
	PxShape **ptr;
	ptr = new PxShape*[numShapes];

	actor->getShapes(ptr, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = ptr[i];
		shape->setSimulationFilterData(filterData);
		shape->setQueryFilterData(filterData);
	}

	//free(ptr);
}

#pragma endregion

//#########################################################################################################
//											customize functions
//#########################################################################################################
#pragma region customize functions

void CPhysxManager::customizeSceneDesc(PxSceneDesc& sceneDesc)
{
	sceneDesc.gravity = PxVec3(0.0f, GRAVITY, 0.0f);
	sceneDesc.filterShader = ItLightensFilter::ItLightensFilterShader;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_PAIRS | PxSceneFlag::eENABLE_CCD;
	sceneDesc.filterCallback = this;
	//sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;
}

#pragma endregion

//#########################################################################################################
//											primitive geometries
//#########################################################################################################
#pragma region primitive geometries

void CPhysxManager::CreateSphereGeometry(const PxReal& radius, PxSphereGeometry& g) const
{
	g = PxSphereGeometry(radius);
}
void CPhysxManager::CreateBoxGeometry(const PxVec3& size, PxBoxGeometry& g) const
{
	g = PxBoxGeometry(size);
}
void CPhysxManager::CreateCapsuleGeometry(const PxReal& radius, const PxReal& height, PxCapsuleGeometry& g) const
{
	g = PxCapsuleGeometry(radius, height);
}

void CPhysxManager::CreatePlaneGeometry(const PxReal & radius, const PxReal & halfheight, PxPlaneGeometry & g) const
{
	g = PxPlaneGeometry();
}

#pragma endregion

//#########################################################################################################
//									Primitive Shapes
//#########################################################################################################
#pragma region Primitive shapes

PxShape * CPhysxManager::CreatePxSphere(PxReal radius, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)
{
	PxShape *sphere = m_pPhysics->createShape(PxSphereGeometry(radius), 
											*m_pPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);
	
	return sphere;
}

PxShape * CPhysxManager::CreatePxCapsule(PxReal radius, PxReal halfHeight, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)
{
	PxShape *capsule = m_pPhysics->createShape(PxCapsuleGeometry(radius,halfHeight),
											*m_pPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);
	
	return capsule;
}

PxShape * CPhysxManager::CreatePxBox(const PxVec3& size, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)
{
	PxMaterial *material = m_pPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
	material->setRestitutionCombineMode(PxCombineMode::eMIN);
	PxBoxGeometry geom = PxBoxGeometry(size);
	if (!geom.isValid()) {
		int i = 0;
	}
	PxShape *box = m_pPhysics->createShape(geom,
		*material, true);
	
	

	return box;
}

#pragma endregion

//#########################################################################################################

//#########################################################################################################
//										Complex Shapes
//#########################################################################################################
#pragma region Complex shapes

PxShape * CPhysxManager::CreateTriangleMesh(PxTriangleMesh * mesh, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)
{
		PxTriangleMeshGeometry meshGeometry(mesh);
		PxShape* triMesh = m_pPhysics->createShape(meshGeometry,
												*m_pPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);

		return triMesh;
}

PxShape * CPhysxManager::CreateConvexShape(const CMesh * mesh, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution) {

	
	PxConvexMesh* convexMesh = CreateCookedConvexMesh(mesh);
	PxShape* aConvexShape = m_pPhysics->createShape(PxConvexMeshGeometry(convexMesh), 
												*m_pPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);

	return aConvexShape;

}

#pragma endregion

//#########################################################################################################
//							game components, create and add to scene
//#########################################################################################################
#pragma region Game elements
//Add actor to the scene
void CPhysxManager::AddToActiveScene(PxActor& actor) {
	m_pScene->addActor(actor);
}

//Create Character controller with capsule collider
PxController * CPhysxManager::CreateCharacterController(PxReal radius, PxReal height)
{
	PxCapsuleControllerDesc descCapsule;
	descCapsule.radius = radius;
	descCapsule.height = height;
	//descCapsule.slopeLimit = 0.3f;	//slope can walk
	descCapsule.stepOffset = 0.01f;	//height can pass walking
	descCapsule.material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.25f);
	descCapsule.contactOffset = 0.01f;
	descCapsule.behaviorCallback = this;
	descCapsule.reportCallback = this;
	PxController* c = m_pManagerControllers->createController(descCapsule);
	return c;
}

//create actor with dynamic rigidbody
PxActor * CPhysxManager::CreateAndAddRigidDynamic(const PxTransform *transform, PxShape *shape, PxReal density)
{
	PxRigidDynamic *actor = PxCreateDynamic(*m_pPhysics,*transform,*shape,density);
	AddToActiveScene(*actor);
	return actor;
}

//create actor with static rigidbody
PxActor * CPhysxManager::CreateAndAddRigidStatic(const PxTransform *transform, PxShape* shape)
{
	PxRigidStatic *actor = PxCreateStatic(*m_pPhysics, *transform, *shape);
	AddToActiveScene(*actor);
	return actor;
}

//create trigger actor
PxActor* CPhysxManager::CreateAndAddTrigger(const PxTransform *transform, PxShape* shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	PxRigidStatic *actor = PxCreateStatic(*m_pPhysics, *transform, *shape);
	AddToActiveScene(*actor);
	return actor;
}

#pragma endregion

//#########################################################################################################
//											Cooking meshes
//#########################################################################################################
#pragma region cooking meshes

//function CreateCookedTriangleMesh: Cook Triangle Mesh to be read for physx
PxTriangleMesh * CPhysxManager::CreateCookedTriangleMesh(const CMesh * mesh)
{
	std::string full_path = mesh->getDataPath() + mesh->getName();
	CFileDataProvider dp(full_path.c_str());
	FileDataMesh dataM = CMesh::loadData(mesh->getDataPath(), dp);		//TODO: way of reading from CMesh buffers

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = dataM.numVtx;
	meshDesc.points.stride = dataM.numVtxPerBytes;
	meshDesc.points.data = dataM.vtxs.data();

	meshDesc.triangles.count = dataM.numIdx / 3;
	meshDesc.triangles.stride = 3 * dataM.numIdxPerBytes;

	meshDesc.triangles.data = dataM.idxs.data();


	meshDesc.flags = PxMeshFlag::eFLIPNORMALS | PxMeshFlag::e16_BIT_INDICES;

	PxDefaultMemoryOutputStream writeBuffer;
	bool status = m_pCooking->cookTriangleMesh(meshDesc, writeBuffer);
	if (!status)
		return NULL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh * cookedMesh = m_pPhysics->createTriangleMesh(readBuffer);

	return cookedMesh;
}

PxConvexMesh * CPhysxManager::CreateCookedConvexMesh(const CMesh * mesh) {
	std::string full_path = mesh->getDataPath() + mesh->getName();
	CFileDataProvider dp(full_path.c_str());
	FileDataMesh dataM = CMesh::loadData(mesh->getDataPath(), dp);		//TODO: way of reading from CMesh buffers

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = dataM.numVtx;
	convexDesc.points.stride = dataM.numVtxPerBytes;
	convexDesc.points.data = dataM.vtxs.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	if (!m_pCooking->cookConvexMesh(convexDesc, buf, &result))
		return NULL;
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = m_pPhysics->createConvexMesh(input);

	return convexMesh;
}

#pragma endregion

//#########################################################################################################
//										Messages
//#########################################################################################################
#pragma region Messages callbacks

//Function OnTrigger: Manager trigger events
void CPhysxManager::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++) {
		const PxTriggerPair& pair = pairs[i];

		// ignore pairs when shapes have been deleted
		if (pair.flags & (PxTriggerPairFlag::eDELETED_SHAPE_TRIGGER | PxTriggerPairFlag::eDELETED_SHAPE_OTHER))
			continue;


		
		PxFilterData fd = pair.otherShape->getQueryFilterData();
		if (fd.word2 & (ItLightensFilter::eCAN_TRIGGER)) {

			if (pair.status & (PxPairFlag::eNOTIFY_TOUCH_LOST)) {
				CHandle h_active = CHandle();
				h_active.fromUnsigned(HandleToUlong(pair.otherActor->userData));

				CHandle h_trigger = CHandle();
				h_trigger.fromUnsigned(HandleToUlong(pair.triggerActor->userData));

				if (!h_trigger.isValid() || !h_active.isValid()) return;

				CEntity *e_trigger = h_trigger;
				CEntity *e_active = h_active;

				TMsgTriggerOut msg;
				msg.other = CHandle(e_active);
				e_trigger->sendMsg(msg);
			}

			if (pair.status & (PxPairFlag::eNOTIFY_TOUCH_FOUND)) {


				CHandle h_active = CHandle();
				h_active.fromUnsigned(HandleToUlong(pair.otherActor->userData));

				CHandle h_trigger = CHandle();
				h_trigger.fromUnsigned(HandleToUlong(pair.triggerActor->userData));

				if (!h_trigger.isValid() || !h_active.isValid()) return;

				CEntity *e_trigger = h_trigger;
				CEntity *e_active = h_active;

				TMsgTriggerIn msg;
				msg.other = CHandle(e_active);
				e_trigger->sendMsg(msg);
	
			}

		}

		
	}
}

#pragma endregion

//#########################################################################################################
//									       simulation callback
//#########################################################################################################
#pragma region simulation callback

void CPhysxManager::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {

	//only will treat collision with player and platforms.

	//beahaviour:
	//platform will push player

	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{

			CEntity *e0 = GetEntityHandle(*pairHeader.actors[0]);
			CEntity *e1 = GetEntityHandle(*pairHeader.actors[1]);

			TCompCharacterController *cc;

			bool isPlayer = true;

			//if there isnt any player or guard, stop collision solver
			if (e0->hasTag("player"))
				cc = e0->get<TCompCharacterController>();
			else if (e1->hasTag("player"))
				cc = e1->get<TCompCharacterController>();
			else {
				isPlayer = false;
				if (e0->hasTag("AI_guard"))
					cc = e0->get<TCompCharacterController>();
				else if (e1->hasTag("AI_guard"))
					cc = e1->get<TCompCharacterController>();
				else return;
			}

			if (isPlayer) {
				//if player dont collide with platform, stop collision solver
				if (e1->hasTag("platform")) {
					auto rd = pairHeader.actors[1]->isRigidDynamic();
					cc->AddSpeed(PxVec3ToVec3(rd->getLinearVelocity())*2.0f);
					//cc->AddMovement(VEC3(0, 1, 0));
				}
				else if (e0->hasTag("platform")) {
					auto rd = pairHeader.actors[0]->isRigidDynamic();
					cc->AddSpeed(PxVec3ToVec3(rd->getLinearVelocity())*2.0f);
					//cc->AddMovement(VEC3(0,1,0));
				}
				else return;
			} else {
				// Send TMsgBoxHit
			}
		}
	}

}

PxFilterFlags	CPhysxManager::pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor * a0, const PxShape * s0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor * a1, const PxShape * s1, PxPairFlags & pairFlags)
{
	PxFilterFlags flags = PxFilterFlag::eNOTIFY;
	return flags;
}
void	CPhysxManager::onWake(PxActor **actors, PxU32 count) {}
void	CPhysxManager::onSleep(PxActor **actors, PxU32 count) {}
//void	CPhysxManager::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {}
void	CPhysxManager::onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}

#pragma endregion

//#########################################################################################################
//									       scene querys
//#########################################################################################################
#pragma region scene querys

bool CPhysxManager::raycast(PxVec3 origin, PxVec3 unitDir, PxReal maxDistance, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{

	bool status = m_pScene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);

	return status;
}

bool CPhysxManager::raycast(VEC3 origin, VEC3 unitDir, PxReal maxDistance, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{
	return raycast(Vec3ToPxVec3(origin), Vec3ToPxVec3(unitDir), maxDistance, hit, filterData, outputFlags);
}

bool CPhysxManager::raycast(PxVec3 origin, PxVec3 end, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{
	if (!(origin == m_last_origin && end == m_last_end)) {
		m_last_origin = origin;
		m_last_end = end;
		m_last_direction = end - origin;
		m_last_distance = sqrt(squared(m_last_direction.x) + squared(m_last_direction.y) + squared(m_last_direction.z));
	}

	bool status = raycast(origin, m_last_direction,m_last_distance, hit, filterData, outputFlags);
	return status;
}

bool CPhysxManager::raycast(VEC3 origin, VEC3 end, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{
	return raycast(Vec3ToPxVec3(origin), Vec3ToPxVec3(end), hit, filterData, outputFlags);
}

bool CPhysxManager::raySphere(PxReal radius, PxVec3& start, PxVec3& direction, PxReal distance, PxSweepCallback& hit, PxQueryFilterData filter, PxHitFlags outputflags)
{
	PxSphereGeometry sphere;
	CreateSphereGeometry(radius, sphere);
	PxTransform transform = PxTransform(start,PxQuat(0,0,0,1));

	bool status = m_pScene->sweep(sphere, transform, direction, distance, hit, outputflags, filter);

	return status;
}

bool CPhysxManager::raySphere(PxReal radius, VEC3& start, VEC3& direction, PxReal distance, PxSweepCallback& hit, PxQueryFilterData filter, PxHitFlags outputflags)
{
	return raySphere(radius, Vec3ToPxVec3(start), Vec3ToPxVec3(direction), distance, hit, filter, outputflags);
}

float CPhysxManager::SquaredDistancePointToGeometry(const PxVec3& point, const PxGeometry& geometry, const PxVec3& originTransform, const PxQuat& quaternionTransform)
{
	float distance = -1;	//default exit, object not supported

	distance = m_pGeomQuerys->pointDistance(point, geometry, PxTransform(originTransform, quaternionTransform));

	return distance;
}

float CPhysxManager::SquaredDistancePointToGeometry(const VEC3& point, const PxGeometry& geometry, const VEC3& originTransform, const CQuaternion& quaternionTransform)
{
	return SquaredDistancePointToGeometry(Vec3ToPxVec3(point), geometry, Vec3ToPxVec3(originTransform), CQuaternionToPxQuat(quaternionTransform));
}

#pragma endregion

//#########################################################################################################
//									joints
//#########################################################################################################
#pragma region namespace joints

//not needed for now
/*
bool CPhysxManager::Createjoint(PxRigidActor * a1, PxRigidActor * a2, const PxTransform & tmx1, const PxTransform & tmx2, int typeJoint)
{
PxControllerBehaviorFlag::
//PxRevoluteJoint *joint = PxRevoluteJointCreate(*m_pPhysics, a1, tmx1, a2, tmx2);

//PxD6Joint *joint = PxD6JointCreate(*m_pPhysics, a1, tmx1, a2, tmx2);
PxFixedJoint *joint = PxFixedJointCreate(*m_pPhysics, a1, tmx1, a2, tmx2);
//joint->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);

joint->setConstraintFlag(PxConstraintFlag::
IZATION, true);
return false;
}
*/

#pragma endregion

//#########################################################################################################
//									namespace physx helper
//#########################################################################################################
#pragma region namespace physx helper

//Conversion functions
//----------------------------------------------------------
PxVec3 PhysxConversion::Vec3ToPxVec3(const VEC3 & vec)
{
	return PxVec3(vec.x,vec.y,vec.z);
}

VEC3 PhysxConversion::PxVec3ToVec3(const PxVec3 & vec)
{
	return VEC3((float)vec.x, (float)vec.y, (float)vec.z);
}

PxExtendedVec3 PhysxConversion::Vec3ToPxExVec3(const VEC3 & vec)
{
	return PxExtendedVec3(vec.x,vec.y,vec.z);
}

VEC3 PhysxConversion::PxExVec3ToVec3(const PxExtendedVec3 & vec)
{
	return VEC3((float)vec.x, (float)vec.y, (float)vec.z);
}

PxQuat PhysxConversion::CQuaternionToPxQuat(const CQuaternion & quat)
{
	return PxQuat(quat.x,quat.y,quat.z,quat.w);
}

CQuaternion PhysxConversion::PxQuatToCQuaternion(const PxQuat & quat)
{
	return CQuaternion(quat.x,quat.y, quat.z, quat.w);
}

PxTransform PhysxConversion::ToPxTransform(const VEC3 & pos, const CQuaternion & rot)
{
	return PxTransform(Vec3ToPxVec3(pos), CQuaternionToPxQuat(rot));
}
CHandle PhysxConversion::GetEntityHandle(const PxActor & a)
{
	if(&a){
		CHandle h;
		h.fromUnsigned(HandleToUlong(a.userData));
		return h;
	}
	else{
		return CHandle(); //handle not valid
	}
}
//----------------------------------------------------------

#pragma endregion
