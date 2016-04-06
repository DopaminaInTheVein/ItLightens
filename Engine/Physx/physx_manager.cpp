#include "mcv_platform.h"
#include "physx_manager.h"
#include "handle\object_manager.h"
#include "render\mesh.h"
#include "components\entity.h"
#include "components\comp_name.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_physics.h"

#include "components\entity_tags.h"


void CPhysxManager::setFtDynamic()
{
	ft_dynamic = PxFilterData();
	ft_dynamic.word0 = ItLightensFilter::eOBJECT | ItLightensFilter::eALL_OBJECTS;
	ft_dynamic.word1 = ItLightensFilter::eCAN_TRIGGER | ItLightensFilter::eCOLLISION;
}

void CPhysxManager::setFtStatic()
{
	ft_static = PxFilterData();
	ft_static.word0 = ItLightensFilter::eSTATIC_OBJECT | ItLightensFilter::eALL_OBJECTS | ItLightensFilter::eALL_STATICS;
	ft_static.word1 = ItLightensFilter::eCOLLISION;
}

void CPhysxManager::setFtCC()
{
	ft_cc = PxFilterData();
	ft_cc.word0 = ItLightensFilter::eANYONE | ItLightensFilter::eALL_OBJECTS;
	ft_cc.word1 = ItLightensFilter::eCAN_TRIGGER | ItLightensFilter::eCOLLISION;
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
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);

	assert(mFoundation);
	if (!mFoundation)
		fatal("PxCreateFoundation failed!");

	//init ProfileZoneManager
	bool recordMemoryAllocations = true;
	mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
	if (!mProfileZoneManager)
		fatal("PxProfileZoneManager::createProfileZoneManager failed!");


	//init Physics
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		PxTolerancesScale(), recordMemoryAllocations, mProfileZoneManager);

	assert(mPhysics);
	if (!mPhysics)
		fatal("PxCreatePhysics failed!");

	//init cooking
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(scale));

	assert(mCooking);
	if (!mCooking)
		fatal("PxCreateCooking failed!");

	//init extensions
	if (!PxInitExtensions(*mPhysics))
		fatal("PxInitExtensions failed!");

	//init scene
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	customizeSceneDesc(sceneDesc);

	if (!sceneDesc.cpuDispatcher)
	{
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(mNbThreads);
		if (!mCpuDispatcher)
			fatal("PxDefaultCpuDispatcherCreate failed!");
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = gDefaultFilterShader;


#ifdef PX_WINDOWS
	if (!sceneDesc.gpuDispatcher && mCudaContextManager)
	{
		sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	}
#endif

	mScene = mPhysics->createScene(sceneDesc);
	assert(mScene);
	if (!mScene)
		fatal("createScene failed!");

	//init managers characters controllers
	mManagerControllers = PxCreateControllerManager(*mScene);
	if (!mManagerControllers)
		fatal("manager controllers failed!");


#ifndef NDEBUG
	//Physx Debbuger
	if (mPhysics->getPvdConnectionManager() == NULL)
		return true;	// no Pvd support for debugging physx

	// setup connection parameters
	const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
	int             port = 5425;         // TCP port to connect to, where PVD is listening
	unsigned int    timeout = 100;          // timeout in milliseconds to wait for PVD to respond,
											// consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();

	// and now try to connect
	mConnection = PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(),
		pvd_host_ip, port, timeout, connectionFlags);

	mScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	mScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
#endif

	setFtDynamic();
	setFtStatic();
	setFtCC();

	return true;
}

//stop function: release memory
void CPhysxManager::stop()
{
	if(mPhysics)mPhysics->release();
	if(mFoundation)mFoundation->release();
	if(mCooking)mCooking->release();
	if(mProfileZoneManager)mProfileZoneManager->release();
	if (mManagerControllers) {
		//mManagerControllers->purgeControllers();	//TODO: free memory
	}
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
		mScene->simulate(t_max_update);
		mScene->fetchResults(true);

		//getHandleManager<TCompPhysics>()->updateAll(t_max_update);
		//getHandleManager<TCompCharacterController>()->updateAll(t_max_update);
	}
}

#pragma endregion

//#########################################################################################################
//											customize functions
//#########################################################################################################
#pragma region customize functions

// set filter tags
void CPhysxManager::setupFiltering(PxRigidActor* actor, PxFilterData& filterData)
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

	free(ptr);
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

#pragma endregion

//#########################################################################################################
//									Primitive Shapes
//#########################################################################################################
#pragma region Primitive shapes

PxShape * CPhysxManager::CreatePxSphere(PxReal radius, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)
{
	PxShape *sphere = mPhysics->createShape(PxSphereGeometry(radius), 
											*mPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);
	
	return sphere;
}

PxShape * CPhysxManager::CreatePxCapsule(PxReal radius, PxReal halfHeight, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)
{
	PxShape *capsule = mPhysics->createShape(PxCapsuleGeometry(radius,halfHeight),
											*mPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);
	
	return capsule;
}

PxShape * CPhysxManager::CreatePxBox(const PxVec3& size, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)
{
	PxShape *box = mPhysics->createShape(PxBoxGeometry(size), 
										*mPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);
	
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
		PxShape* triMesh = mPhysics->createShape(meshGeometry,
												*mPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);

		return triMesh;
}

PxShape * CPhysxManager::CreateConvexShape(const CMesh * mesh, PxReal staticFriction, PxReal dynamicFriction, PxReal restitution) {

	
	PxConvexMesh* convexMesh = CreateCookedConvexMesh(mesh);
	PxShape* aConvexShape = mPhysics->createShape(PxConvexMeshGeometry(convexMesh), 
												*mPhysics->createMaterial(staticFriction, dynamicFriction, restitution),true);

	return aConvexShape;

}

#pragma endregion

//#########################################################################################################
//							game components, create and add to scene
//#########################################################################################################
#pragma region Game elements
//Add actor to the scene
void CPhysxManager::AddToActiveScene(PxActor& actor) {
	mScene->addActor(actor);
}

//Create Character controller with capsule collider
PxController * CPhysxManager::CreateCharacterController(PxReal radius, PxReal height)
{
	PxCapsuleControllerDesc descCapsule;
	descCapsule.radius = radius;
	descCapsule.height = height;
	//descCapsule.slopeLimit = 0.3f;	//slope can walk
	descCapsule.stepOffset = 0.01f;	//height can pass walking
	descCapsule.material = mPhysics->createMaterial(0.5f,0.5f,0.25f);
	descCapsule.contactOffset = 0.01f;
	descCapsule.behaviorCallback = this;
	descCapsule.reportCallback = this;
	PxController* c = mManagerControllers->createController(descCapsule);
	return c;
}

//create actor with dynamic rigidbody
PxActor * CPhysxManager::CreateAndAddRigidDynamic(const PxTransform *transform, PxShape *shape, PxReal density)
{
	PxRigidDynamic *actor = PxCreateDynamic(*mPhysics,*transform,*shape,density);
	AddToActiveScene(*actor);
	return actor;
}

//create actor with static rigidbody
PxActor * CPhysxManager::CreateAndAddRigidStatic(const PxTransform *transform, PxShape* shape)
{
	PxRigidStatic *actor = PxCreateStatic(*mPhysics, *transform, *shape);
	AddToActiveScene(*actor);
	return actor;
}

//create trigger actor
PxActor* CPhysxManager::CreateAndAddTrigger(const PxTransform *transform, PxShape* shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	PxRigidStatic *actor = PxCreateStatic(*mPhysics, *transform, *shape);
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
	bool status = mCooking->cookTriangleMesh(meshDesc, writeBuffer);
	if (!status)
		return NULL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh * cookedMesh = mPhysics->createTriangleMesh(readBuffer);

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
	if (!mCooking->cookConvexMesh(convexDesc, buf, &result))
		return NULL;
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = mPhysics->createConvexMesh(input);

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
		if (fd.word1 & (ItLightensFilter::eCAN_TRIGGER)) {
			//CHandle* h = CHandle::getHandleFromVoid(pair.otherActor->userData);
			//CEntity* e_trigger = CHandle::getHandleFromptr(h);
			if (pair.status & (PxPairFlag::eNOTIFY_TOUCH_LOST)) {
				CEntity* e_trigger = static_cast<CEntity*>(pair.triggerActor->userData);
				CEntity* e_active = static_cast<CEntity*>(pair.otherActor->userData);
				//OnTriggerExit()
				TMsgTriggerOut msg;
				msg.other = CHandle(e_active);
				e_trigger->sendMsg(msg);
			}

			if (pair.status & (PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
				CEntity* e_trigger = static_cast<CEntity*>(pair.triggerActor->userData);
				CEntity* e_active = static_cast<CEntity*>(pair.otherActor->userData);
				//OnTriggerEnter()
				TMsgTriggerIn msg;
				msg.other = CHandle(e_active);
				e_trigger->sendMsg(msg);
			}

		}

		
	}
}

void	CPhysxManager::onWake(PxActor **actors, PxU32 count) {}
void	CPhysxManager::onSleep(PxActor **actors, PxU32 count) {}
void	CPhysxManager::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {}
void	CPhysxManager::onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}

/*PxQueryHitType::Enum CPhysxManager::postFilter(const PxFilterData & filterData, const PxQueryHit & hit)
{
	return PxQueryHitType::Enum();
}

PxQueryHitType::Enum CPhysxManager::preFilter(const PxFilterData & filterData, const PxShape * shape, const PxRigidActor * actor, PxHitFlags & queryFlags)
{
	return PxQueryHitType::Enum();
}
*/
#pragma endregion

//#########################################################################################################
//									       scene querys
//#########################################################################################################
#pragma region scene querys

bool CPhysxManager::raycast(PxVec3 origin, PxVec3 unitDir, PxReal maxDistance, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{

	bool status = mScene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);

	return status;
}

bool CPhysxManager::raycast(VEC3 origin, VEC3 unitDir, PxReal maxDistance, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{
	return raycast(Vec3ToPxVec3(origin), Vec3ToPxVec3(unitDir), maxDistance, hit, filterData, outputFlags);
}

bool CPhysxManager::raycast(PxVec3 origin, PxVec3 end, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{
	if (!(origin == last_origin && end == last_end)) {
		last_origin = origin;
		last_end = end;
		last_direction = end - origin;
		last_distance = sqrt(squared(last_direction.x) + squared(last_direction.y) + squared(last_direction.z));
	}

	bool status = raycast(origin, last_direction,last_distance, hit, filterData, outputFlags);
	return status;
}

bool CPhysxManager::raycast(VEC3 origin, VEC3 end, PxRaycastBuffer& hit, PxQueryFilterData filterData, const PxHitFlags outputFlags)
{
	return raycast(Vec3ToPxVec3(origin), Vec3ToPxVec3(end), hit, filterData, outputFlags);
}

#pragma endregion

//#########################################################################################################
//									joints
//#########################################################################################################
#pragma region namespace joints

/*
bool CPhysxManager::Createjoint(PxRigidActor * a1, PxRigidActor * a2, const PxTransform & tmx1, const PxTransform & tmx2, int typeJoint)
{
PxControllerBehaviorFlag::
//PxRevoluteJoint *joint = PxRevoluteJointCreate(*mPhysics, a1, tmx1, a2, tmx2);

//PxD6Joint *joint = PxD6JointCreate(*mPhysics, a1, tmx1, a2, tmx2);
PxFixedJoint *joint = PxFixedJointCreate(*mPhysics, a1, tmx1, a2, tmx2);
//joint->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);

joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
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
CHandle PhysxConversion::GetEntityHandle(PxActor & a)
{
	if(&a){
		CEntity* e = static_cast<CEntity*>(a.userData);
		return CHandle(e);
	}
	else{
		return CHandle(); //handle not valid
	}
}
//----------------------------------------------------------

#pragma endregion
