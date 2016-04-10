#include "mcv_platform.h"
#include "comp_charactercontroller.h"

#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"
#include "comp_name.h"

#include "handle\handle.h"
#include "app_modules\io\io.h"


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


void TCompCharacterController::onCreate(const TMsgEntityCreated &)
{

	m_pActor = g_PhysxManager->CreateCharacterController(m_radius, m_height);
	m_affectGravity = true;
	m_gravitySpeed = -10.0f;
	CEntity *e = CHandle(this).getOwner();
	if (e) {	//update position from CC to match the render transform
		TCompName  *nameComp = e->get<TCompName>();
		if (nameComp)
			name = std::string(nameComp->name);
		TCompTransform *mtx = e->get<TCompTransform>();
		PxExtendedVec3 p = Vec3ToPxExVec3(mtx->getPosition());
		p.y += m_height + m_radius;	//add height value from capsule, center from collider at center of the shape
		m_pActor->setPosition(p);
		m_pActor->getActor()->userData = CHandle(this).getOwner().ToVoidPt();
		m_filter = DEFAULT_DATA_CC;
		UpdateTags();
	}
}

void TCompCharacterController::renderInMenu()
{
	ImGui::Text("Editable values:\n");
	ImGui::Checkbox("affectGravity", &m_affectGravity);
	ImGui::SliderFloat("gravity", &m_gravitySpeed, -15.0, 15.0f);
	//ImGui::SliderFloat3("acceleration",&m_accel.x,-10.0f,10.0f);		//will be 0, cleaned each frame
	ImGui::SliderFloat3("speed", &m_speed.x, -20.0f, 20.0f);
	ImGui::Separator();
	ImGui::Text("Will be updated by engine\n");
	ImGui::Checkbox("OnGround", &m_OnGround);
	ImGui::Checkbox("PhysxOnGround", &m_physxOnground);
	ImGui::Checkbox("last_onground", &m_lastOnGround);
	//ImGui::SliderFloat3("movement", &m_toMove.x, -1.0f, 1.0f,"%.5f");	//will be 0, cleaned each frame
	
}

void TCompCharacterController::update(float dt)
{
	//PROFILE_FUNCTION("update");
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

	//update y speed based on y gravity
	if (m_affectGravity) {
		if (!m_OnGround) {
			m_speed.y += m_gravitySpeed*dt;
		}
	}

	//update final speed
	m_speed += m_accel*dt;

	//calc if there are some speed to sum at speed from own rigidbdy speed
	if (!m_OnGround && m_lastOnGround) {
		VEC3 speed = PhysxConversion::PxVec3ToVec3(m_last_speed);
		speed.y = 0;
		m_speed += speed;
	}

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
	//updata y speed & accel if controller is on ground
	if (m_physxOnground) {
		if (m_speed.y < 0.0f)	m_speed.y = -0.001f;
		if (m_accel.y < 0.0f)	m_accel.y = 0.0f;
	}

	//update final movement
	m_toMove += m_speed*dt;
}

//apply a friction for the speeds
void TCompCharacterController::UpdateFriction(float dt) {
	PROFILE_FUNCTION("update friction adn gravity");
	//update speeds with friction
	if (m_speed.x != 0.0f) m_speed.x -= m_speed.x*m_friction*dt;
	if (m_speed.z != 0.0f) m_speed.z -= m_speed.z*m_friction*dt;
}

//apply the calculated movement
void TCompCharacterController::ApplyPendingMoves(float dt) {
	std::string toprint = "apply pending moves ";
	toprint = toprint + name;

	if (name == "pj") {
		int i = 0;
	}

	PROFILE_FUNCTION(name.c_str());
	if (m_toMove != VEC3(0.0f, 0.0f, 0.0f)) {
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
	PROFILE_FUNCTION("on ground");
	m_lastOnGround = m_OnGround;

	if (m_flagsCollisions & PxControllerFlag::eCOLLISION_DOWN) {
		m_OnGround = true;
		m_physxOnground = true;
	}
	else {
		//raycast to look for down distance
		PxRaycastBuffer hit;
		bool hit_ground = g_PhysxManager->raycast(GetFootPosition(), PhysxConversion::PxVec3ToVec3(-m_pActor->getUpDirection()), 0.1f, hit);
		m_OnGround = hit_ground;
		m_physxOnground = false;
	}
}


//update position from render mesh
void TCompCharacterController::UpdateMeshTransform()
{
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
			m_mass = 50.0f;

		}
		if (h.hasTag("AI_guard"))
			m_filter.word0 |= ItLightensFilter::eGUARD | ItLightensFilter::eNPC;

		if (h.hasTag("AI_poss"))
			m_filter.word0 |= ItLightensFilter::ePOSSEABLE | ItLightensFilter::eNPC;
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

		free(ptr);
	}
	free(ra);
}

#pragma endregion

//#########################################################################################################
//										Movement modifiers
//#########################################################################################################
#pragma region setters

void TCompCharacterController::AddImpulse(const VEC3& impulse) {
	PROFILE_FUNCTION("add impulse");
	m_speed.y = impulse.y;
	m_speed.x = impulse.x;
	m_speed.z = impulse.z;
}

void TCompCharacterController::AddSpeed(const VEC3 & direction, float speed)
{
	m_speed += direction*speed;
}

void TCompCharacterController::AddAccel(const VEC3 & direction, float accel)
{
	m_accel += direction*accel;
}

void TCompCharacterController::AddMovement(const VEC3& direction, float speed) {
	PROFILE_FUNCTION("add move");
	m_toMove += direction*speed;
}


#pragma endregion











