#include "mcv_platform.h"
#include "comp_physics.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_render_static_mesh.h"
#include "render\static_mesh.h"
#include "windows\app.h"

//will return first material from shape
PxMaterial* getMaterial(PxShape* shape) {
	PxMaterial ** ms = new PxMaterial*[1];
	shape->getMaterials(ms, 1);
	return ms[0];
}

void TCompPhysics::updateTags(PxFilterData filter)
{
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("crystal")) {
			filter.word0 = filter.word0 | CPhysxManager::eCRYSTAL;
			filter.word0 &= ~CPhysxManager::eALL_STATICS;
		}

		if (h.hasTag("water")) {
			filter.word0 = filter.word0 | CPhysxManager::eLIQUID;
		}

		if (h.hasTag("bomb")) {
			filter.word0 = filter.word0 | CPhysxManager::eBOMB;
		}
		
	}

	pShape->setQueryFilterData(filter);
}

//read init values
bool TCompPhysics::load(MKeyValue & atts)
{
	std::string readString = atts.getString("type_collision", "static");
	mCollisionType = getCollisionTypeValueFromString(readString);
	readString = atts.getString("type_shape", "mesh");
	mCollisionShape = getCollisionShapeValueFromString(readString);

	switch (mCollisionShape) {
	case TRI_MESH:
		//nothing extra needed to read
		break;
	case SPHERE:
		mRadius = atts.getFloat("radius",0.5f);
		break;
	case BOX:
		mSize = atts.getPoint("size");
		mSize = mSize / 2;
		break;
	case CAPSULE:
		mRadius = atts.getFloat("radius", 0.5f);
		mHeight = atts.getFloat("height", 0.5f);
		break;
	case CONVEX:
		//nothing extra needed to read
		break;
	default:
		fatal("object type inexistent!!\n");
		break;
	}

	return true;
}

int TCompPhysics::getCollisionTypeValueFromString(std::string str) {

	if (str == "static") {
		return STATIC_OBJECT;
	}else if(str == "dynamic") {
		return DYNAMIC_RB;
	}
	else if (str == "trigger") {
		return TRIGGER;
	}
	else {
		fatal("type object undefined - %s\n!!", str.c_str());
		return STATIC_OBJECT;
	}
}

int TCompPhysics::getCollisionShapeValueFromString(std::string str) {

	if (str == "mesh") {
		return TRI_MESH;
	}
	else if (str == "sphere") {
		return SPHERE;
	}
	else if (str == "box") {
		return BOX;
	}
	else if (str == "capsule") {
		return CAPSULE;
	}
	else if (str == "convex") {
		return CONVEX;
	}
	else {
		fatal("type object undefined - %s\n!!", str.c_str());
		return BOX;
	}
}


//When entity created
void TCompPhysics::onCreate(const TMsgEntityCreated &)
{
	readIniFileAttr();	//load current default values
	switch (mCollisionShape) {
	case TRI_MESH:
		createTriMeshShape();
		break;
	case SPHERE:
		createSphereShape();
		break;
	case BOX:
		createBoxShape();
		break;
	case CAPSULE:
		createCapsuleShape();
		break;
	case CONVEX:
		createConvexShape();
		break;
	default:
		fatal("object type inexistent!!\n");
		break;
	}

	addRigidbodyScene();
}

//fixedUpdate for physix, only needed for dynamic rigidbodys
void TCompPhysics::update(float dt)
{
	if (mCollisionType == DYNAMIC_RB && pActor) {
		// ask physics about the current pos + rotation
		// update my sibling TCompTransform with the physics info
		PxTransform curr_pose = rigidActor->getGlobalPose();

		if (mCollisionShape == BOX) curr_pose.p.y = curr_pose.p.y - 0.5f;		//TODO: Origin from shape at center!!!!! mesh center at foot, FIX THAT, temp solution
		
		CEntity *e = CHandle(this).getOwner();
		TCompTransform *tmx = e->get<TCompTransform>();
		tmx->setPosition(PxVec3ToVec3( curr_pose.p));
		tmx->setRotation(PxQuatToCQuaternion(curr_pose.q));
	}
}


//Private methods to init rigidbodys and actor
//----------------------------------------------------------
bool TCompPhysics::createTriMeshShape()
{
	CHandle entity_h = CHandle(this).getOwner();
	CEntity *e = nullptr;
	if(entity_h.isValid()) e = entity_h;
	if (e) {
		TCompRenderStaticMesh *comp_static_mesh = e->get<TCompRenderStaticMesh>();
		PxTriangleMesh *cookedMesh = PhysxManager->CreateCookedTriangleMesh(comp_static_mesh->static_mesh->slots[0].mesh);		//only will cook from mesh from slot 0
		pShape = PhysxManager->CreateTriangleMesh(cookedMesh,mStaticFriction, mDynamicFriction, mRestitution);
		return true;
	}

	fatal("component without valid entity!\n");
	return false;
}

bool TCompPhysics::createBoxShape()
{
	pShape = PhysxManager->CreatePxBox(Vec3ToPxVec3(mSize), mStaticFriction, mDynamicFriction, mRestitution);
	return true;
}

bool TCompPhysics::createCapsuleShape()
{
	pShape = PhysxManager->CreatePxCapsule(mRadius, mHeight, mStaticFriction, mDynamicFriction, mRestitution);
	return true;
}

bool TCompPhysics::createSphereShape()
{
	pShape = PhysxManager->CreatePxSphere(mRadius, mStaticFriction, mDynamicFriction, mRestitution);
	return true;
}

bool TCompPhysics::createConvexShape() {
	CHandle entity_h = CHandle(this).getOwner();
	CEntity *e = nullptr;
	if (entity_h.isValid()) e = entity_h;
	if (e) {
		TCompRenderStaticMesh *comp_static_mesh = e->get<TCompRenderStaticMesh>();
		pShape = PhysxManager->CreateConvexShape(comp_static_mesh->static_mesh->slots[0].mesh);
		return true;
	}

	fatal("component without valid entity!\n");
	return false;
}

bool TCompPhysics::addRigidbodyScene()
{
	CEntity *e = CHandle(this).getOwner();
	TCompTransform *tmx = e->get<TCompTransform>();

	if (mCollisionType == STATIC_OBJECT) {

		PxVec3 p = Vec3ToPxVec3(tmx->getPosition());
		PxQuat q = CQuaternionToPxQuat(tmx->getRotation());
		PxTransform curr_pose = PxTransform(p,q);
		PxFilterData mFilterData = DEFAULT_DATA_STATIC;
		updateTags(mFilterData);
		pActor = PhysxManager->CreateAndAddRigidStatic(&curr_pose, pShape);
		CEntity *m = CHandle(this).getOwner();
		pActor->userData = m;

		return true;
	}
	else if (mCollisionType == DYNAMIC_RB) {
		
		PxVec3 p = Vec3ToPxVec3(tmx->getPosition());
		PxQuat q = CQuaternionToPxQuat(tmx->getRotation());
		PxTransform curr_pose = PxTransform(p, q);
		PxFilterData mFilterData = DEFAULT_DATA_DYNAMIC;
		updateTags(mFilterData);
		pActor = PhysxManager->CreateAndAddRigidDynamic(&curr_pose, pShape, 0.5f);
		rigidActor = pActor->isRigidDynamic();
		CEntity *m = CHandle(this).getOwner();
		pActor->userData = m;
		
		return true;
	}else if (mCollisionType == TRIGGER) {

		PxVec3 p = Vec3ToPxVec3(tmx->getPosition());
		PxQuat q = CQuaternionToPxQuat(tmx->getRotation());
		PxTransform curr_pose = PxTransform(p, q);
		pActor = PhysxManager->CreateAndAddTrigger(&curr_pose, pShape);
		CEntity *m = CHandle(this).getOwner();
		//CHandle h = CHandle(this).getOwner();
		pActor->userData = m;

		return true;
	}

	assert(false);	//never should do this line
	fatal("collision type unsupported!!\n");
	return false;
}

bool TCompPhysics::isKinematic()
{
	PxRigidDynamic *rd = pActor->isRigidDynamic();

	if (rd) {
		return rd->getRigidDynamicFlags().isSet(PxRigidDynamicFlag::eKINEMATIC);
	}
	return true;
}

bool TCompPhysics::setKinematic(bool isKinematic)
{
	PxRigidDynamic *rd = pActor->isRigidDynamic();

	if (rd) {
		rd->setRigidBodyFlag(PxRigidDynamicFlag::eKINEMATIC, isKinematic);
		return true;
	}
	return false;
}

void TCompPhysics::setPosition(VEC3 position, CQuaternion rotation)
{
	PxTransform tr = PxTransform(PhysxConversion::Vec3ToPxVec3(position), PhysxConversion::CQuaternionToPxQuat(rotation));
	pActor->isRigidActor()->setGlobalPose(tr);
}

//----------------------------------------------------------


void TCompPhysics::renderInMenu()
{
	PxRigidDynamic *rigidDynamic = pActor->isRigidDynamic();
	PxRigidStatic *rigidStatic = pActor->isRigidStatic();

	if (rigidDynamic) {
		ImGui::Text("Object dynamic, recommended to turn on kinematic before moving");

		bool isKinematic = rigidDynamic->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
		if (ImGui::Checkbox("isKinematic", &isKinematic))
			rigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);

		ImGui::Separator();
		if (ImGui::TreeNode("Temporal values")) {

			PxTransform trans = rigidDynamic->getGlobalPose();

			if (ImGui::SliderFloat3("Pos", &trans.p.x, -50.f, 50.f)) {
				rigidDynamic->setGlobalPose(trans);
			}

			if (ImGui::SliderFloat4("rot", &trans.q.x, -1.f, 1.f)) {
				trans.q.normalize();
				rigidDynamic->setGlobalPose(trans);
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Shapes")) {

			int nBShapes = rigidDynamic->getNbShapes();
			PxShape **ptr;
			ptr = new PxShape*[nBShapes];
			rigidDynamic->getShapes(ptr, 1);

			for (int i = 0; i < nBShapes; i++) {
				std::string name = "shape_noname";
				if (ptr[i]->getName()) {
					name = ptr[i]->getName();
					name += "_shape";
				}
				if (ImGui::TreeNode(name.c_str())) {

					if (ImGui::TreeNode("Configurables values")) {

						if (ImGui::SliderFloat("dynamic friction", &mDynamicFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if(m)
								getMaterial(ptr[i])->setDynamicFriction(mDynamicFriction);
						}
						if (ImGui::SliderFloat("static friction", &mStaticFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setStaticFriction(mStaticFriction);
						}
						if (ImGui::SliderFloat("restitution", &mRestitution, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setRestitution(mRestitution);
						}


						ImGui::TreePop();
					}


					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::SmallButton("Read values from file")) {
			readIniFileAttr();
			updateAttrMaterial();
		}
		
		if (ImGui::SmallButton("Save config values")) {
			writeIniFileAttr();
		}
	}

	if (rigidStatic) {
		ImGui::Text("Object static");

		ImGui::Separator();
		if (ImGui::TreeNode("Temporal values")) {

			PxTransform trans = rigidStatic->getGlobalPose();

			if (ImGui::SliderFloat3("Pos", &trans.p.x, -50.f, 50.f)) {
				rigidStatic->setGlobalPose(trans);
			}

			if (ImGui::SliderFloat4("rot", &trans.q.x, -1.f, 1.f)) {
				trans.q.normalize();
				rigidStatic->setGlobalPose(trans);
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Shapes")) {

			int nBShapes = rigidStatic->getNbShapes();
			PxShape **ptr;
			ptr = new PxShape*[nBShapes];
			rigidStatic->getShapes(ptr, 1);

			for (int i = 0; i < nBShapes; i++) {
				std::string name = "shape_noname";
				if (ptr[i]->getName()) {
					name = ptr[i]->getName();
					name += "_shape";
				}
				if (ImGui::TreeNode(name.c_str())) {

					if (ImGui::TreeNode("Configurables values")) {

						if (ImGui::SliderFloat("dynamic friction", &mDynamicFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setDynamicFriction(mDynamicFriction);
						}
						if (ImGui::SliderFloat("static friction", &mStaticFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setStaticFriction(mStaticFriction);
						}
						if (ImGui::SliderFloat("restitution", &mRestitution, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setRestitution(mRestitution);
						}


						ImGui::TreePop();
					}


					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::SmallButton("Read values from file")) {
			readIniFileAttr();
			updateAttrMaterial();
		}

		if (ImGui::SmallButton("Save config values")) {
			writeIniFileAttr();
		}
	}
}


void TCompPhysics::readIniFileAttr() {
	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr;

	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("box")) {
			char read[64];
			std::string read_s;

			//dynamic friction
			GetPrivateProfileStringA("box", "dynamic_friction", "not_found", read, 64, file_ini.c_str());
			read_s = std::string(read);
			if (read_s != "not_found") {
				mDynamicFriction = std::stof(read_s);
			}

			//static friction
			GetPrivateProfileStringA("box", "static_friction", "not_found", read, 64, file_ini.c_str());
			read_s = std::string(read);
			if (read_s != "not_found") {
				mStaticFriction = std::stof(read_s);
			}

			//restitution
			GetPrivateProfileStringA("box", "restitution", "not_found", read, 64, file_ini.c_str());
			read_s = std::string(read);
			if (read_s != "not_found") {
				mRestitution = std::stof(read_s);
			}
		}
	}
}

void TCompPhysics::updateAttrMaterial() {
	PxRigidDynamic* rd = pActor->isRigidDynamic();
	PxRigidStatic* rs = pActor->isRigidStatic();
	int numShapes = 0;
	if (rd) {
		numShapes = rd->getNbShapes();
	}
	if (rs) {
		numShapes = rs->getNbShapes();
	}
	if (!rd && !rs) return;
	PxShape ** buff_s = new PxShape*[numShapes];
	if (rd) {
		rd->getShapes(buff_s, numShapes);
	}
	if (rs) {
		rs->getShapes(buff_s, numShapes);
	}
	for (int i = 0; i < numShapes; i++) {
		int numMats = buff_s[i]->getNbMaterials();
		PxMaterial **buff_m = new PxMaterial*[numMats];
		buff_s[i]->getMaterials(buff_m,numMats);
		for (int j = 0; j < numMats; j++) {
			buff_m[j]->setDynamicFriction(mDynamicFriction);
			buff_m[j]->setStaticFriction(mStaticFriction);
			buff_m[j]->setRestitution(mRestitution);
		}
	}
}

void TCompPhysics::writeIniFileAttr() {
	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr;

	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("box")) {
			char read[64];
			
			//dynamic friction
			sprintf(read, "%.2f", mDynamicFriction);
			WritePrivateProfileStringA("box", "dynamic_friction", read, file_ini.c_str());

			//static friction
			sprintf(read, "%.2f", mStaticFriction);
			WritePrivateProfileStringA("box", "static_friction", read, file_ini.c_str());

			//restitution
			sprintf(read, "%.2f", mRestitution);
			WritePrivateProfileStringA("box", "restitution", read, file_ini.c_str());
		}
	}
}