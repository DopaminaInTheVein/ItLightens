#include "mcv_platform.h"
#include "comp_physics.h"

#include "resources/resources_manager.h"

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

void TCompPhysics::setBehaviour(ItLightensFilter::descObjectBehaviour tag, bool enabled)
{
	auto ra = m_pActor->isRigidActor();
	if (ra) g_PhysxManager->setBehaviour(ra, tag, enabled);
}

void TCompPhysics::setGravity(bool enabled)
{
	auto rd = m_pActor->isRigidDynamic();
	if (rd) rd->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !enabled);
}

VEC3 TCompPhysics::getLinearVelocity()
{
	VEC3 result = VEC3(0.f, 0.f, 0.f);
	auto rd = m_pActor->isRigidDynamic();
	if (rd)	result = PhysxConversion::PxVec3ToVec3(rd->getLinearVelocity());
	return result;
}

void TCompPhysics::setLinearVelocity(VEC3 speed)
{
	auto rd = m_pActor->isRigidDynamic();
	if (rd) rd->setLinearVelocity(PhysxConversion::Vec3ToPxVec3(speed));
}

VEC3 TCompPhysics::getAngularVelocity()
{
	VEC3 result = VEC3(0.f, 0.f, 0.f);
	auto rd = m_pActor->isRigidDynamic();
	if (rd)	result = PhysxConversion::PxVec3ToVec3(rd->getAngularVelocity());
	return result;
}

void TCompPhysics::setAngularVelocity(VEC3 speed)
{
	auto rd = m_pActor->isRigidDynamic();
	if (rd) rd->setAngularVelocity(PhysxConversion::Vec3ToPxVec3(speed));
}

void TCompPhysics::updateTagsSetupActor()
{
	PxFilterData mFilterData;
	if (m_collisionType == STATIC_OBJECT) {
		mFilterData = DEFAULT_DATA_STATIC;
	}
	else if (m_collisionType == DYNAMIC_RB) {
		mFilterData = DEFAULT_DATA_DYNAMIC;
	}
	else {
		assert(false || fatal("CompPhysics: update tags. Must static or dynamic\n"));
	}
	updateTagsSetupActor(mFilterData);
}

void TCompPhysics::updateTagsSetupActor(PxFilterData& filter)
{
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("trigger"))
			return;
		if (h.hasTag("crystal")) {
			filter.word0 |= ItLightensFilter::eCRYSTAL;
		}

		else if (h.hasTag("water")) {
			filter.word0 |= ItLightensFilter::eLIQUID;
		}

		else if (h.hasTag("bomb")) {
			filter.word0 |= ItLightensFilter::eBOMB;
		}
		else if (h.hasTag("platform")) {
			filter.word0 |= ItLightensFilter::ePLATFORM;
		}
		else if (h.hasTag("fragment")) {
			filter.word0 = ItLightensFilter::eFRAGMENT;
			filter.word1 = PXM_NO_PLAYER_NPC;
		}
		else if (h.hasTag("throw_bomb")) {
			filter.word0 = ItLightensFilter::eTHROW;
			filter.word1 = 0;
			filter.word2 &= ~(ItLightensFilter::eCOLLISION | ItLightensFilter::eCAN_TRIGGER);
			filter.word2 |= ItLightensFilter::eIGNORE_PLAYER; // No se usa ahora
		}
		else if (m_collisionType == STATIC_OBJECT) {
			filter.word0 |= ItLightensFilter::eSCENE;
		}
		else {
			filter.word0 |= ItLightensFilter::eOBJECT;
		}
	}

	if (!m_pActor) return;
	PxRigidActor *actor = m_pActor->isRigidActor();
	if (actor) {
		//filter.word1 = ItLightensFilter::eALL;
		g_PhysxManager->setupFiltering(actor, filter);
		/*if (h.hasTag("fragment") || h.hasTag("throw_bomb")) {
		  PxRigidDynamic * rd = actor->isRigidDynamic();
		  //if (rd) {
			//rd->setRigidDynamicFlag(PxRigidDynamicFlag::eENABLE_CCD, true);
			//rd->setRigidDynamicFlag(PxRigidDynamicFlag::eENABLE_CCD_FRICTION, true);
		  }
		}*/
	}
	//filter.word0 = ItLightensFilter::eALL;
}

//read init values
bool TCompPhysics::load(MKeyValue & atts)
{
	std::string readString = atts.getString("type_collision", "static");
	m_collisionType = getCollisionTypeValueFromString(readString);
	readString = atts.getString("type_shape", "mesh");
	m_collisionShape = getCollisionShapeValueFromString(readString);
	m_mass = atts.getFloat("mass", 2.0f);		//default enough to pass polarize threshold
	m_kinematic = atts.getBool("kinematic", false);		//default enough to pass polarize threshold
	m_smooth = atts.getFloat("smooth", 0.0f);
	switch (m_collisionShape) {
	case TRI_MESH:
		m_mesh_name = atts.getString("mesh_name", "");
		m_use_navmesh = atts.getBool("use_navmesh", true);
		m_only_navmesh = atts.getBool("only_navmesh", false);
		break;
	case SPHERE:
		m_radius = atts.getFloat("radius", 0.5f);
		break;
	case BOX:
		m_size = atts.getPoint("size");
		m_size = m_size / 2;
		break;
	case CAPSULE:
		m_radius = atts.getFloat("radius", 0.5f);
		m_height = atts.getFloat("height", 0.5f);
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

bool TCompPhysics::save(std::ofstream& os, MKeyValue& atts)
{
	std::string t_collisions[] = { "static", "dynamic", "trigger" };
	atts.put("type_collision", t_collisions[m_collisionType]);

	std::string t_shapes[] = { "mesh", "sphere", "capsule", "box", "convex", "drone" };
	atts.put("type_shape", t_shapes[m_collisionShape]);

	if (m_collisionShape == BOX) {
		atts.put("size", m_size);
	}
	else if (m_collisionShape == CAPSULE || m_collisionShape == SPHERE) {
		atts.put("radius", m_radius);
		if (m_collisionShape == CAPSULE) atts.put("height", m_height);
	}
	else if (m_collisionShape == TRI_MESH) {
		atts.put("mesh_name", m_mesh_name);
	}

	atts.put("mass", m_mass);
	atts.put("kinematic", m_kinematic);
	atts.put("smooth", m_smooth);
	if (!m_use_navmesh) atts.put("use_navmesh", m_use_navmesh);
	if (m_only_navmesh) atts.put("only_navmesh", m_only_navmesh);

	return true;
}

int TCompPhysics::getCollisionTypeValueFromString(std::string str) {
	if (str == "static") {
		return STATIC_OBJECT;
	}
	else if (str == "dynamic") {
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
	//Provisonal?
	else if (str == "drone") {
		return DRONE;
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
	bool ok = false;
	switch (m_collisionShape) {
	case TRI_MESH:
		ok = createTriMeshShape();
		break;
	case SPHERE:
		ok = createSphereShape();
		break;
	case BOX:
		ok = createBoxShape();
		break;
	case CAPSULE:
		ok = createCapsuleShape();
		break;
	case CONVEX:
		ok = createConvexShape();
		break;
	case DRONE:
		ok = createDroneShape();
		break;
	default:
		fatal("object type inexistent!!\n");
		break;
	}
	if (!ok) CHandle(this).destroy();
}

//fixedUpdate for physix, only needed for dynamic rigidbodys
void TCompPhysics::update(float dt)
{
	if (!isInRoom(CHandle(this).getOwner())) return;
	if (m_collisionType == DYNAMIC_RB && m_pActor) {
		// ask physics about the current pos + rotation
		// update my sibling TCompTransform with the physics info

		//Render transform
		GET_MY(tmx, TCompTransform);
		VEC3 render_pos = tmx->getPosition();
		CQuaternion render_rot = tmx->getRotation();

		//Physx transform
		PxTransform curr_pose = m_pRigidActor->getGlobalPose();
		VEC3 phys_pos = PxVec3ToVec3(curr_pose.p);
		CQuaternion phys_rot = PxQuatToCQuaternion(curr_pose.q);

		//New render transform
		VEC3 render_pos_new = render_pos * m_smooth + phys_pos * (1.f - m_smooth);
		CQuaternion render_rot_new = render_rot * m_smooth + phys_rot * (1.f - m_smooth);
		render_rot_new.Normalize();
		tmx->setPosition(render_pos_new);
		tmx->setRotation(render_rot_new);

		//quat.CreateFromAxisAngle(anglesEuler,0.0f);
		//VEC3 up_mesh = tmx->getUp();
		//if (m_collisionShape == BOX) pos -= 0.5*up_mesh; 		//TODO: Origin from shape at center!!!!! mesh center at foot, FIX THAT, temp solution
	}
}

//Private methods to init rigidbodys and actor
//----------------------------------------------------------
bool TCompPhysics::createTriMeshShape()
{
	const CMesh* mesh = readMesh();
	dbg("[Physx]: Cooking static mesh on %s", MY_NAME);
	PxTriangleMesh *cookedMesh = g_PhysxManager->CreateCookedTriangleMesh(mesh);		//only will cook from mesh from slot 0
	m_pShape = g_PhysxManager->CreateTriangleMesh(cookedMesh, m_staticFriction, m_dynamicFriction, m_restitution);
	if (!m_pShape) return false;
	addRigidbodyScene();

	//int size_slots = comp_static_mesh->static_mesh->slots.size();
	//if (size_slots > 1) {
	//	for (int i = 1; i < size_slots; i++) {
	//		PxTriangleMesh *cookedMesh = g_PhysxManager->CreateCookedTriangleMesh(comp_static_mesh->static_mesh->slots[i].mesh);		//only will cook from mesh from slot 0
	//		m_pShape = g_PhysxManager->CreateTriangleMesh(cookedMesh, m_staticFriction, m_dynamicFriction, m_restitution);
	//		PxRigidActor *ra = m_pActor->isRigidActor();
	//		if (ra)
	//			ra->attachShape(*m_pShape);
	//	}
	//}
	PxFilterData mFilterData = DEFAULT_DATA_DYNAMIC;
	updateTagsSetupActor(mFilterData);
	return true;

	fatal("component without valid entity!\n");
	return false;
}

const CMesh* TCompPhysics::readMesh()
{
	const CMesh* res = nullptr;
	if (m_mesh_name != "")
		res = Resources.get(m_mesh_name.c_str())->as<CMesh>();
	else {
		GET_MY(r_st_mesh, TCompRenderStaticMesh);
		if (r_st_mesh) {
			auto st_mesh = r_st_mesh->static_mesh;
			if (st_mesh) {
				auto slots = st_mesh->slots;
				if (slots.size() > 0) {
					assert(slots.size() == 1);
					return slots[0].mesh;
				}
			}
		}
	}
	if (!res) {
		char err[1024];
		sprintf(err, "Error comp_physics: Object [%s] without mesh", MY_OWNER);
		MessageBox(NULL, err, NULL, MB_OK);
	}
	return nullptr;
}

bool TCompPhysics::createBoxShape()
{
	m_pShape = g_PhysxManager->CreatePxBox(Vec3ToPxVec3(m_size), m_staticFriction, m_dynamicFriction, m_restitution);
	addRigidbodyScene();
	return true;
}

bool TCompPhysics::createCapsuleShape()
{
	m_pShape = g_PhysxManager->CreatePxCapsule(m_radius, m_height, m_staticFriction, m_dynamicFriction, m_restitution);
	addRigidbodyScene();
	return true;
}

bool TCompPhysics::createSphereShape()
{
	m_pShape = g_PhysxManager->CreatePxSphere(m_radius, m_staticFriction, m_dynamicFriction, m_restitution);
	addRigidbodyScene();
	return true;
}

bool TCompPhysics::createConvexShape() {
	CHandle entity_h = CHandle(this).getOwner();
	CEntity *e = nullptr;
	if (entity_h.isValid()) e = entity_h;
	if (e) {
		TCompRenderStaticMesh *comp_static_mesh = e->get<TCompRenderStaticMesh>();
		m_pShape = g_PhysxManager->CreateConvexShape(comp_static_mesh->static_mesh->slots[0].mesh);
		addRigidbodyScene();

		int size_slots = comp_static_mesh->static_mesh->slots.size();
		if (size_slots > 1) {
			for (int i = 1; i < size_slots; i++) {
				m_pShape = g_PhysxManager->CreateConvexShape(comp_static_mesh->static_mesh->slots[i].mesh);
				PxRigidActor *ra = m_pActor->isRigidActor();
				if (ra)
					ra->attachShape(*m_pShape);
			}
		}

		return true;
	}

	fatal("component without valid entity!\n");
	return false;
}

bool TCompPhysics::createDroneShape()
{
	//TODO: Ojo Hardcore hardcode!
	m_pShape = g_PhysxManager->CreatePxSphere(0.58f, m_staticFriction, m_dynamicFriction, m_restitution);
	addRigidbodyScene();
	PxRigidActor * ra = m_pActor->isRigidActor();
	m_pShape = g_PhysxManager->CreatePxBox(
		PhysxConversion::Vec3ToPxVec3(VEC3(0.3f, 0.15f, 1.7f)),
		m_staticFriction, m_dynamicFriction, m_restitution);
	ra->attachShape(*m_pShape);
	return true;
}

bool TCompPhysics::addRigidbodyScene()
{
	CEntity *e = CHandle(this).getOwner();
	TCompTransform *tmx = e->get<TCompTransform>();

	if (m_collisionType == STATIC_OBJECT) {
		PxVec3 p = Vec3ToPxVec3(tmx->getPosition());
		PxQuat q = CQuaternionToPxQuat(tmx->getRotation());
		PxTransform curr_pose = PxTransform(p, q);
		PxFilterData mFilterData = DEFAULT_DATA_STATIC;
		m_pActor = g_PhysxManager->CreateAndAddRigidStatic(&curr_pose, m_pShape);
		m_pShape->release();
		updateTagsSetupActor(mFilterData);

		//save parent handle to physx
		m_pActor->userData = (void*)CHandle(this).getOwner().asUnsigned();
		return true;
	}
	else if (m_collisionType == DYNAMIC_RB) {
		PxVec3 p = Vec3ToPxVec3(tmx->getPosition());
		PxQuat q = CQuaternionToPxQuat(tmx->getRotation());
		PxTransform curr_pose = PxTransform(p, q);
		PxFilterData mFilterData = DEFAULT_DATA_DYNAMIC;
		m_pActor = g_PhysxManager->CreateAndAddRigidDynamic(&curr_pose, m_pShape, 0.5f);
		m_pShape->release();
		m_pRigidActor = m_pActor->isRigidDynamic();
		updateTagsSetupActor(mFilterData);

		//save parent handle to physx
		m_pActor->userData = (void*)CHandle(this).getOwner().asUnsigned();
		m_pActor->isRigidBody()->setMass(m_mass);
		if (m_kinematic) setKinematic(true);
		return true;
	}
	else if (m_collisionType == TRIGGER) {
		PxVec3 p = Vec3ToPxVec3(tmx->getPosition());
		PxQuat q = CQuaternionToPxQuat(tmx->getRotation());
		PxTransform curr_pose = PxTransform(p, q);
		m_pActor = g_PhysxManager->CreateAndAddTrigger(&curr_pose, m_pShape);
		m_pShape->release();

		//save parent handle to physx
		m_pActor->userData = (void*)CHandle(this).getOwner().asUnsigned();

		return true;
	}

	fatal("collision type unsupported!!\n");
	assert(false);	//never should do this line
	return false;
}

bool TCompPhysics::isKinematic()
{
	if (m_collisionType == STATIC_OBJECT) return true;
	PxRigidDynamic *rd = m_pActor->isRigidDynamic();

	if (rd) {
		return rd->getRigidDynamicFlags().isSet(PxRigidDynamicFlag::eKINEMATIC);
	}
	return true;
}

bool TCompPhysics::setKinematic(bool isKinematic)
{
	PxRigidDynamic *rd = m_pActor->isRigidDynamic();

	if (rd) {
		rd->setRigidBodyFlag(PxRigidDynamicFlag::eKINEMATIC, isKinematic);
		return true;
	}
	return false;
}
void TCompPhysics::AddMovement(VEC3 movement)
{
	PxRigidDynamic *rd = m_pActor->isRigidDynamic();
	PxTransform tmx = rd->getGlobalPose();
	tmx.p += PhysxConversion::Vec3ToPxVec3(movement);
	setPosition(PhysxConversion::PxVec3ToVec3(tmx.p), PhysxConversion::PxQuatToCQuaternion(tmx.q));
}

void TCompPhysics::DisableRotationXZ()
{
	PxRigidDynamic *rd = m_pActor->isRigidDynamic();
	if (rd) rd->setMassSpaceInertiaTensor(PxVec3(0.f, 1.f, 0.f));
}
void TCompPhysics::AddForce(VEC3 force)
{
	PxRigidDynamic *rb = m_pActor->isRigidDynamic();
	if (rb) {
		rb->addForce(PhysxConversion::Vec3ToPxVec3(force));
	}
}

void TCompPhysics::ClearForces()
{
	PxRigidDynamic *rb = m_pActor->isRigidDynamic();
	if (rb) {
		rb->clearForce();
	}
}

void TCompPhysics::AddVelocity(VEC3 velocity)
{
	PxRigidDynamic *rb = m_pActor->isRigidDynamic();
	if (rb) {
		rb->addForce(PhysxConversion::Vec3ToPxVec3(velocity), physx::PxForceMode::eVELOCITY_CHANGE);
	}
}

void TCompPhysics::setPosition(TCompTransform * tr)
{
	setPosition(tr->getPosition(), tr->getRotation());
}

void TCompPhysics::setPosition(VEC3 position, CQuaternion rotation)
{
	bool isKinematic = false;	//by default no kinematic
	PxRigidDynamic *rd = m_pActor->isRigidDynamic();
	PxTransform tr = PxTransform(PhysxConversion::Vec3ToPxVec3(position), PhysxConversion::CQuaternionToPxQuat(rotation));

	if (rd) {
		isKinematic = rd->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
	}

	if (!isKinematic) {	//no kinematic object,
		m_pActor->isRigidActor()->setGlobalPose(tr);		//setposition without using simulation physx
	}
	else {				//if kinematic use setkinematicTarget
		assert(rd);
		rd->setKinematicTarget(tr);	//use physx, can push, etc
	}
}

//----------------------------------------------------------

void TCompPhysics::renderInMenu()
{
	PxRigidDynamic *rigidDynamic = m_pActor->isRigidDynamic();
	PxRigidStatic *rigidStatic = m_pActor->isRigidStatic();

	if (rigidDynamic) {
		ImGui::Text("Object dynamic, recommended to turn on kinematic before moving");

		bool isKinematic = rigidDynamic->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
		if (ImGui::Checkbox("isKinematic", &isKinematic))
			rigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);

		ImGui::Separator();
		if (ImGui::TreeNode("Temporal values")) {
			PxTransform trans = rigidDynamic->getGlobalPose();

			if (ImGui::DragFloat3("Pos", &trans.p.x, -0.1f, 0.1f)) {
				rigidDynamic->setGlobalPose(trans);
			}

			if (ImGui::DragFloat4("rot", &trans.q.x, -0.1f, 0.1f)) {
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
						if (ImGui::SliderFloat("dynamic friction", &m_dynamicFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setDynamicFriction(m_dynamicFriction);
						}
						if (ImGui::SliderFloat("static friction", &m_staticFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setStaticFriction(m_staticFriction);
						}
						if (ImGui::SliderFloat("restitution", &m_restitution, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setRestitution(m_restitution);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}

			free(ptr);

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

			if (ImGui::DragFloat3("Pos", &trans.p.x, -0.1f, 0.1f)) {
				rigidStatic->setGlobalPose(trans);
			}

			if (ImGui::DragFloat4("rot", &trans.q.x, -0.1f, 0.1f)) {
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
						if (ImGui::SliderFloat("dynamic friction", &m_dynamicFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setDynamicFriction(m_dynamicFriction);
						}
						if (ImGui::SliderFloat("static friction", &m_staticFriction, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setStaticFriction(m_staticFriction);
						}
						if (ImGui::SliderFloat("restitution", &m_restitution, 0.0f, 2.0f)) {
							PxMaterial *m = getMaterial(ptr[i]);
							if (m)
								getMaterial(ptr[i])->setRestitution(m_restitution);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}

			free(ptr);
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

	ImGui::Separator();
	PxFilterData filterData = getFilterData();
	int fdata[4] = { filterData.word0, filterData.word1, filterData.word2, filterData.word3 };

	if (ImGui::DragInt4("Filter data", fdata)) {
		filterData.word0 = fdata[0];
		filterData.word1 = fdata[1];
		filterData.word2 = fdata[2];
		filterData.word3 = fdata[3];
		auto actor = m_pActor->isRigidActor();
		if (actor) g_PhysxManager->setupFiltering(actor, filterData);
	}
}

void TCompPhysics::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("box")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			std::map<std::string, float> fields_box = readIniAtrData(file_ini, "box");

			m_dynamicFriction = fields_box["dynamic_friction"];
			m_staticFriction = fields_box["static_friction"];
			m_restitution = fields_box["restitution"];
		}
	}
}

void TCompPhysics::updateAttrMaterial() {
	PxRigidDynamic* rd = m_pActor->isRigidDynamic();
	PxRigidStatic* rs = m_pActor->isRigidStatic();
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
		buff_s[i]->getMaterials(buff_m, numMats);
		for (int j = 0; j < numMats; j++) {
			buff_m[j]->setDynamicFriction(m_dynamicFriction);
			buff_m[j]->setStaticFriction(m_staticFriction);
			buff_m[j]->setRestitution(m_restitution);
		}
	}
}

PxFilterData TCompPhysics::getFilterData() {
	PxRigidActor *actor = m_pActor->isRigidActor();
	PxFilterData res;
	if (actor) {
		res = g_PhysxManager->getFiltering(actor);
		//*fData = res.word0;
		//*(fData + 1) = res.word1;
		//*(fData + 2) = res.word2;
		//*(fData + 3) = res.word3;
	}
	return res;
}

void TCompPhysics::writeIniFileAttr() {
	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr_json;

	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("box")) {
			char read[64];

			std::map<std::string, float> atributes_map;

			atributes_map["dynamic_friction"] = m_dynamicFriction;
			atributes_map["static_friction"] = m_staticFriction;
			atributes_map["restitution"] = m_restitution;

			writeIniAtrData(file_ini, "box", atributes_map);
		}
	}
}