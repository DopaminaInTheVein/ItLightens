#include "mcv_platform.h"
#include "comp_charactercontroller.h"

#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"
#include "comp_name.h"

#include "handle\handle.h"
//#include "input\input_wrapper.h"

#define FACTOR_HEIGHT_AABB	1.4f
#define FACTOR_WIDTH_AABB	1.5f

//#########################################################################################################
//									general functions
//#########################################################################################################
#pragma region general functions

bool TCompCharacterController::load(MKeyValue & atts)
{
	m_radius = atts.getFloat("radius", 0.5f);
	m_height = atts.getFloat("height", 0.5f);
	return true;
}

bool TCompCharacterController::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("radius", m_radius);
	atts.put("height", m_height);
	return true;
}

void TCompCharacterController::onCreate(const TMsgEntityCreated &)
{
	m_pActor = g_PhysxManager->CreateCharacterController(m_radius, m_height);
	m_affectGravity = true;
	m_gravitySpeed = -10.0f;
	m_maxSpeed = 7.0f;
	CEntity *e = CHandle(this).getOwner();
	if (e) {	//update position from CC to match the render transform
		TCompName  *nameComp = e->get<TCompName>();
		if (nameComp)
			name = std::string(nameComp->name);
		TCompTransform *mtx = e->get<TCompTransform>();
		PxExtendedVec3 p = Vec3ToPxExVec3(mtx->getPosition());
		p.y += m_height + m_radius;	//add height value from capsule, center from collider at center of the shape
		m_pActor->setPosition(p);

		//save parent handle
		m_pActor->getActor()->userData = (void*)CHandle(this).getOwner().asUnsigned();

		//default flags for character controllers
		m_filter = DEFAULT_DATA_CC;
		UpdateTags();
	}

	InitAABB();
}

void TCompCharacterController::renderInMenu()
{
	ImGui::Text("Editable values:\n");
	ImGui::Checkbox("affectGravity", &m_affectGravity);
	ImGui::DragFloat("gravity", &m_gravitySpeed, -0.1f, 0.1f);
	//ImGui::SliderFloat3("acceleration",&m_accel.x,-10.0f,10.0f);		//will be 0, cleaned each frame
	ImGui::DragFloat3("speed", &m_speed.x, -0.1f, 0.1f);
	ImGui::Separator();
	ImGui::Text("Will be updated by engine\n");
	ImGui::Checkbox("OnGround", &m_OnGround);
	ImGui::Checkbox("PhysxOnGround", &m_physxOnground);
	ImGui::Checkbox("last_onground", &m_lastOnGround);
	ImGui::DragFloat("Friction Ground", &m_friction, 0.f, 10.f);
	ImGui::DragFloat("Friction Air", &m_friction_air, 0.f, 10.f);
	ImGui::Text("Position collider: %f - %f - %f\n", GetFootPosition().x, GetFootPosition().y, GetFootPosition().z);
	//ImGui::SliderFloat3("movement", &m_toMove.x, -1.0f, 1.0f,"%.5f");	//will be 0, cleaned each frame
}

VEC3 TCompCharacterController::GetCameraPointFocus() const
{
	CEntity *e = CHandle(this).getOwner();
	TCompTransform *t = e->get<TCompTransform>();
	return VEC3(0, m_height, 0) + (-t->getLeft()*m_radius);
}

void TCompCharacterController::update(float dt)
{
	PROFILE_FUNCTION("update");
	if (m_active) {
		RecalcOnGround();
		UpdateFriction(dt);
		RecalcSpeed(dt);
		RecalcMovement(dt);
		ApplyPendingMoves(dt);
		UpdateMeshTransform();
	}
}

#pragma endregion

//#########################################################################################################
//									Internal update movement values
//#########################################################################################################
#pragma region Internal update movement values

//recalc actual speed from acceleration
void TCompCharacterController::RecalcSpeed(float dt)
{
	PROFILE_FUNCTION("update: speed");
	//update y speed based on y gravity
	if (m_affectGravity) {
		if (!m_OnGround) {
			assert(isValid(m_speed));
			m_speed.y += m_gravitySpeed*dt;
			assert(isValid(m_speed));
		}
	}

	//update final speed
	assert(isValid(m_speed));
	m_speed += m_accel*dt;
	assert(isValid(m_speed));

	//calc if there are some speed to sum at speed from own rigidbdy speed
	/*if (!m_OnGround && m_lastOnGround) {
		assert(isValid(m_speed));
		VEC3 speed = PhysxConversion::PxVec3ToVec3(m_last_speed);
		speed.y = 0;
		m_speed += speed;
		assert(isValid(m_speed));
	}*/

	//calc if some speed is too low. if true, will be assigned at 0
	float abs_x = abs(m_speed.x);
	float abs_z = abs(m_speed.z);

	if (abs_x < m_eOffsetSpeed)
		m_speed.x = 0;

	if (abs_z < m_eOffsetSpeed)
		m_speed.z = 0;
}

//recalc how much have to move from speed
void TCompCharacterController::RecalcMovement(float dt)
{
	PROFILE_FUNCTION("update: movement");
	//update y speed & accel if controller is on ground
	if (m_physxOnground) {
		if (m_speed.y < 0.0f)	m_speed.y = -0.001f;
	}

	//update speed if collision is up
	if (m_flagsCollisions & PxControllerFlag::eCOLLISION_UP) {
		if (m_speed.y > 0.0f) m_speed.y = 0.0f;
	}

	//check is some speed is too big
	//speed x axis
	if (m_speed.x > m_maxSpeed) {
		m_speed.x = m_maxSpeed;
	}
	else if (m_speed.x < -m_maxSpeed) {
		m_speed.x = -m_maxSpeed;
	}

	//speed y axis
	if (m_speed.y > m_maxSpeed) {
		m_speed.y = m_maxSpeed;
	}
	else if (m_speed.y < -m_maxSpeed) {
		m_speed.y = -m_maxSpeed;
	}

	//speed z axis
	if (m_speed.z > m_maxSpeed) {
		m_speed.z = m_maxSpeed;
	}
	else if (m_speed.z < -m_maxSpeed) {
		m_speed.z = -m_maxSpeed;
	}

	//update final movement
	assert(isValid(m_toMove));
	m_toMove += m_speed*dt;
	assert(isValid(m_toMove));
}

//apply a friction for the speeds
void TCompCharacterController::UpdateFriction(float dt) {
	PROFILE_FUNCTION("update: friction");
	//update speeds with friction
	assert(isValid(m_speed));
	float friction = OnGround() ? m_friction : m_friction_air;
	if (m_speed.x != 0.0f) m_speed.x -= m_speed.x*friction*dt;
	if (m_speed.z != 0.0f) m_speed.z -= m_speed.z*friction*dt;
	assert(isValid(m_speed));
}

//apply the calculated movement
void TCompCharacterController::ApplyPendingMoves(float dt) {
	PROFILE_FUNCTION("apply moves");
	std::string toprint = "apply pending moves ";
	toprint = toprint + name;

	if (name == "pj") {
		int i = 0;
	}

	if (m_toMove != VEC3(0.0f, 0.0f, 0.0f)) {
		PROFILE_FUNCTION(name.c_str());
		assert(isValid(m_toMove));
		PxVec3 moved = PxVec3(m_toMove.x, m_toMove.y, m_toMove.z);
		m_last_speed = m_pActor->getActor()->getLinearVelocity();
		m_flagsCollisions = m_pActor->move(moved, 0.0f, dt, m_filterController);
		//clean acceleration & pendent displacement
		m_toMove = VEC3(0.0f, 0.0f, 0.0f);
		m_accel = m_toMove;
	}
}

//recalc if the controller is on ground
void TCompCharacterController::RecalcOnGround()
{
	PROFILE_FUNCTION("recalc: onground");
	m_lastOnGround = m_OnGround;
	if (!m_affectGravity) return;
	if (m_flagsCollisions & PxControllerFlag::eCOLLISION_DOWN) {
		m_OnGround = true;
		m_physxOnground = true;
	}
	else {
		//raycast to look for down distance
		PxQueryFilterData filterData;
		filterData.data.word0 = ItLightensFilter::eSCENE | ItLightensFilter::eOBJECT;
		PxRaycastBuffer hit;
		bool hit_ground = g_PhysxManager->raycast(GetFootPosition(), PhysxConversion::PxVec3ToVec3(-m_pActor->getUpDirection()), 0.1f, hit, filterData);
		m_OnGround = hit_ground;
		m_physxOnground = false;
	}
}

//update position from render mesh
void TCompCharacterController::UpdateMeshTransform()
{
	PROFILE_FUNCTION("update: transform");
	PxExtendedVec3 curr_pos = m_pActor->getFootPosition();
	//PxVec3 up_v = pActor->getUpDirection();	//get rotation
	CEntity *e = CHandle(this).getOwner();
	if (!e) return;
	TCompTransform *tmx = e->get<TCompTransform>();
	tmx->setPosition(PxExVec3ToVec3(curr_pos));
	//tmx->setRotation(PxQuatToCQuaternion(curr_pose.q));
}

#pragma endregion

//#########################################################################################################
//										physics setters propierties
//#########################################################################################################
#pragma region setters

void TCompCharacterController::UpdateTags()
{
	//PROFILE_FUNCTION("update tags");
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("player")) { //player
			m_filter.word0 |= ItLightensFilter::ePLAYER_BASE;
			m_filter.word0 |= ItLightensFilter::ePLAYER_CONTROLLED;
			m_filter.word1 &= ~ItLightensFilter::eTHROW;

			m_mass = 50.0f;
		}
		if (h.hasTag("AI_guard")) {
			m_filter.word0 |= ItLightensFilter::eGUARD;
		}

		if (h.hasTag("AI_poss"))
			m_filter.word0 |= ItLightensFilter::ePOSSESSABLE;
	}

	g_PhysxManager->setupFiltering(m_pActor->getActor(), m_filter);
}

void TCompCharacterController::teleport(const VEC3& pos)
{
	m_pActor->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
}

void TCompCharacterController::teleport(const PxVec3& pos)
{
	m_pActor->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
}

void TCompCharacterController::SetCollisions(bool new_collisions)
{
	PxRigidActor *ra = m_pActor->getActor()->isRigidActor();
	if (ra) {
		const PxU32 numShapes = ra->getNbShapes();
		PxShape **ptr;
		ptr = new PxShape*[numShapes];
		ra->getShapes(ptr, numShapes);
		for (PxU32 i = 0; i < numShapes; i++)
		{
			PxShape* shape = ptr[i];
			if (!new_collisions) {
				m_filter.word1 &= ~ItLightensFilter::eCOLLISION;
				m_filter.word1 &= ~ItLightensFilter::eCAN_TRIGGER;	//for test only
			}
			else {
				m_filter.word1 |= ItLightensFilter::eCOLLISION;
				m_filter.word1 |= ItLightensFilter::eCAN_TRIGGER;		//for test only
			}
			shape->setSimulationFilterData(m_filter);
			shape->setQueryFilterData(m_filter);
		}
	}
}

void TCompCharacterController::SetFilterData(PxFilterData& filter)
{
	PxRigidActor *ra = m_pActor->getActor()->isRigidActor();
	m_filter = filter;
	if (ra) {
		const PxU32 numShapes = ra->getNbShapes();
		PxShape **ptr;
		ptr = new PxShape*[numShapes];
		ra->getShapes(ptr, numShapes);
		for (PxU32 i = 0; i < numShapes; i++)
		{
			PxShape* shape = ptr[i];
			shape->setSimulationFilterData(m_filter);
			shape->setQueryFilterData(m_filter);
		}
	}
}

#pragma endregion

//#########################################################################################################
//										Movement modifiers
//#########################################################################################################
#pragma region setters

void TCompCharacterController::AddImpulse(const VEC3& impulse, bool prevalent) {
	PROFILE_FUNCTION("add impulse");
	assert(isValid(m_speed));
	if (prevalent) {
		if (!sameSign(m_speed.x, impulse.x)) m_speed.x = 0;
		if (!sameSign(m_speed.y, impulse.y)) m_speed.y = 0;
		if (!sameSign(m_speed.z, impulse.z)) m_speed.z = 0;
	}
	m_speed.x += impulse.x;
	m_speed.y += impulse.y;
	m_speed.z += impulse.z;
	assert(isValid(m_speed));
}

void TCompCharacterController::AddSpeed(const VEC3 & direction, float speed)
{
	assert(isValid(m_speed));
	m_speed += direction*speed;
	assert(isValid(m_speed));
}

void TCompCharacterController::AddAccel(const VEC3 & direction, float accel)
{
	m_accel += direction*accel;
}

void TCompCharacterController::AddMovement(const VEC3& direction, float speed) {
	PROFILE_FUNCTION("add move");
	assert(isValid(m_toMove));
	m_toMove += direction*speed;
	assert(isValid(m_toMove));
}

void TCompCharacterController::ResetMovement()
{
	m_accel = VEC3(0, 0, 0);
	m_speed = VEC3(0, 0, 0);
}

void TCompCharacterController::ChangeSpeed(float speed)
{
	m_accel = VEC3(0, 0, 0);
	assert(isValid(m_speed));
	m_speed.Normalize();
	m_speed *= speed;
	assert(isValid(m_speed));
}

void TCompCharacterController::InitAABB()
{
	aabb.Center = VEC3(0, m_height / 2.f * FACTOR_HEIGHT_AABB, 0);
	aabb.Extents.x = aabb.Extents.z = m_radius * FACTOR_WIDTH_AABB;
	aabb.Extents.y = m_height / 2 * FACTOR_HEIGHT_AABB;
}

void TCompCharacterController::onGetLocalAABB(const TMsgGetLocalAABB& msg)
{
	// Ojo, aabb de charcontroller manda sobre el resto (no las combino)
	msg.aabb->Center = aabb.Center;
	msg.aabb->Extents = aabb.Extents;
}

#pragma endregion