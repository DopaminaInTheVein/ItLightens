#include "mcv_platform.h"
#include "player_controller_speedy.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"
#include "app_modules\io\io.h"
#include "components\comp_msgs.h"

#include "components/comp_charactercontroller.h"

void player_controller_speedy::Init()
{
	om = getHandleManager<player_controller_speedy>();	//player

	DeleteState("jumping");
	DeleteState("falling");

	AddState("doublefalling", (statehandler)&player_controller_speedy::DoubleFalling);		//needed to disable double jump on falling
	AddState("doublejump", (statehandler)&player_controller_speedy::DoubleJump);

	AddState("falling", (statehandler)&player_controller_speedy::Falling);
	AddState("jumping", (statehandler)&player_controller_speedy::Jumping);

	AddState("dashing", (statehandler)&player_controller_speedy::Dashing);
	AddState("blink", (statehandler)&player_controller_speedy::Blink);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
	myEntity = myParent;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();

	drop_water_timer = drop_water_timer_reset;

	// Mesh management
	mesh = myEntity->get<TCompRenderStaticMesh>();

	pose_idle_route = "static_meshes/speedy/speedy.static_mesh";
	pose_jump_route = "static_meshes/speedy/speedy_jump.static_mesh";
	pose_run_route = "static_meshes/speedy/speedy_run.static_mesh";

	ChangeState("idle");
}

void player_controller_speedy::myUpdate() {
	energyDecreasal(getDeltaTime()*0.5f);
	updateDashTimer();
	updateBlinkTimer();
	updateDropWaterTimer();

	if (dashing) {
		ChangePose(pose_run_route);
		ChangeState("dashing");
	}
	else if (state == "moving") {
		ChangePose(pose_run_route);
	}
	else if (state == "idle") {
		ChangePose(pose_idle_route);
	}
	else if (state == "jumping" || state == "doublejumping") {
		ChangePose(pose_jump_route);
	}
}

void player_controller_speedy::UpdateInputActions() {
	if (io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed()) {
		if (dash_ready) {
			energyDecreasal(5.0f);
			ChangeState("dashing");
			TCompCharacterController *cc = myEntity->get<TCompCharacterController>();
			cc->SetGravity(false);
			dashing = true;
		}
	}
	if (io->mouse.right.becomesPressed() || io->joystick.button_B.becomesPressed()) {
		if (blink_ready) {
			energyDecreasal(10.0f);
			ChangePose(pose_idle_route);
			ChangeState("blink");
		}
	}
}

void player_controller_speedy::DoubleJump()
{
	UpdateDirection();
	UpdateMovDirection();

	if (jspeed <= 0.1f) {
		jspeed = 0.0f;
		ChangePose(pose_jump_route);
		ChangeState("doublefalling");
	}
}

void player_controller_speedy::DoubleFalling() {
	UpdateDirection();
	UpdateMovDirection();

	if (onGround) {
		jspeed = 0.0f;
		ChangePose(pose_idle_route);
		ChangeState("idle");
	}
}

void player_controller_speedy::Jumping()
{
	UpdateDirection();
	UpdateMovDirection();

	if (onGround) {
		jspeed = 0.0f;
		ChangePose(pose_idle_route);
		ChangeState("idle");
	}

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		jspeed = jimpulse;
		energyDecreasal(5.0f);
		ChangePose(pose_jump_route);
		ChangeState("doublejump");
	}
}

void player_controller_speedy::Falling()
{
	UpdateDirection();
	UpdateMovDirection();

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		jspeed = jimpulse;
		energyDecreasal(5.0f);
		ChangePose(pose_jump_route);
		ChangeState("doublejump");
	}

	if (onGround) {
		jspeed = 0.0f;
		ChangePose(pose_idle_route);
		ChangeState("idle");
	}
}

void player_controller_speedy::Dashing()
{
	if (dash_ready) {
		bool arrived = dashFront();
		if (arrived) {
			dashing = false;
			resetDashTimer();
			ChangePose(pose_idle_route);
			ChangeState("idle");
		}
		else {
		}
	}
}

void player_controller_speedy::Blink()
{
	if (blink_ready) {
		SetMyEntity();
		TCompTransform* player_transform = myEntity->get<TCompTransform>();
		TCompCharacterController *cc = myEntity->get<TCompCharacterController>();
		VEC3 player_position = player_transform->getPosition();
		VEC3 player_front = player_transform->getFront();
		float dist, distCollision;
		if (collisionBlink(distCollision)) {
			dist = distCollision;
		}
		else {
			dist = blink_distance;
		}
		player_position += (player_front * (dist - 0.5f));

		cc->GetController()->setPosition(PhysxConversion::Vec3ToPxExVec3(player_position));

		resetBlinkTimer();
	}
	ChangePose(pose_idle_route);
	ChangeState("idle");
}

bool player_controller_speedy::dashFront()
{
	dash_duration += getDeltaTime();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	TCompCharacterController *cc = myEntity->get<TCompCharacterController>();
	VEC3 player_position = player_transform->getPosition();
	VEC3 player_front = player_transform->getFront();
	cc->AddMovement(VEC3(player_front.x*dash_speed*getDeltaTime(),0.0f, player_front.z*dash_speed*getDeltaTime()));

	/*if (drop_water_ready) {
		// CREATE WATER
		// Creating the new handle
		CHandle curr_entity;
		auto hm = CHandleManager::getByName("entity");
		CHandle new_h = hm->createHandle();
		curr_entity = new_h;
		CEntity* e = curr_entity;
		// Adding water tag
		tags_manager.addTag(curr_entity, getID("water"));
		// Creating the new entity components
		// create name component
		auto hm_name = CHandleManager::getByName("name");
		CHandle new_name_h = hm_name->createHandle();
		MKeyValue atts_name;
		atts_name["name"] = "possessed_speedy_water";
		new_name_h.load(atts_name);
		e->add(new_name_h);
		// create transform component
		auto hm_transform = CHandleManager::getByName("transform");
		CHandle new_transform_h = hm_transform->createHandle();
		MKeyValue atts;
		// position, rotation and scale
		char position[64]; sprintf(position, "%f %f %f", player_position.x, player_position.y, player_position.z);
		atts["pos"] = position;
		char rotation[64]; sprintf(rotation, "%f %f %f %f", 1.f, 1.f, 1.f, 1.f);
		atts["rotation"] = rotation;
		char scale[64]; sprintf(scale, "%f %f %f", 1.f, 1.f, 1.f);
		atts["scale"] = scale;
		// load transform attributes and add transform to the entity
		new_transform_h.load(atts);
		e->add(new_transform_h);
		// create static_mesh component
		auto hm_mesh = CHandleManager::getByName("render_static_mesh");
		CHandle new_mesh_h = hm_mesh->createHandle();
		MKeyValue atts_mesh;
		atts_mesh["name"] = water_static_mesh;
		new_mesh_h.load(atts_mesh);
		e->add(new_mesh_h);
		// create water component and add it to the entity
		CHandleManager* hm_water = CHandleManager::getByName("water");
		CHandle new_water_h = hm_water->createHandle();
		e->add(new_water_h);
		// init the new water component
		auto hm_water_cont = getHandleManager<water_controller>();
		water_controller* water_cont = hm_water_cont->getAddrFromHandle(new_water_h);
		water_cont->Init();
		// init entity and send message to the new water entity with its type
		TMsgSetWaterType msg_water;
		msg_water.type = 1;
		e->sendMsg(msg_water);
		// end the entity creation
		e->sendMsg(TMsgEntityCreated());
		curr_entity = CHandle();

		// reset drop water cooldown
		resetDropWaterTimer();
	}*/

	if (dash_duration > dash_max_duration ) {
		dash_duration = 0;
		TCompCharacterController *cc = myEntity->get<TCompCharacterController>();
		cc->SetGravity(true);
		return true;
	}
	else {
		return false;
	}
}
bool player_controller_speedy::collisionWall() {
	float distFirstCollider; // No lo uso
	//TODO RAYCAST SOLID
	bool ret = rayCastToFront(1, 1.0f, distFirstCollider);
	return ret;
}

bool player_controller_speedy::collisionBlink(float& distCollision) {
	//TODO RAYCAST CRYSTAL
	bool ret = rayCastToFront(2, blink_distance + 0.5f, distCollision);
	return ret;
}

bool player_controller_speedy::rayCastToFront(int types, float reach, float& distRay) {
	CHandle me = CHandle(this).getOwner();
	CEntity* eMe = me;
	TCompTransform* tMe = eMe->get<TCompTransform>();

	VEC3 origin = tMe->getPosition() + VEC3(0, 1.0f, 0);
	VEC3 direction = tMe->getFront();
	float dist = reach;
	Debug->DrawLine(origin, direction, dist);

	//PROVISINAL FOR TEST:
	PxQueryFilterData filter = PxQueryFilterData();
	if (types == 2) {
		filter.data.word0 = CPhysxManager::eALL_STATICS | CPhysxManager::eOBJECT;	//ignore crystal and people
	}
	//END PROV

	PxRaycastBuffer hit;
	bool ret = PhysxManager->raycast(origin,direction,dist,hit,filter);
	distRay = hit.getAnyHit(0).distance;	//first hit
	return ret;
}

// Timers update functions

void player_controller_speedy::updateDashTimer()
{
	dash_timer -= getDeltaTime();
	if (dash_timer <= 0) {
		dash_ready = true;
	}
}

void player_controller_speedy::resetDashTimer()
{
	dash_timer = dash_cooldown;
	dash_ready = false;
}

void player_controller_speedy::updateBlinkTimer()
{
	blink_timer -= getDeltaTime();
	if (blink_timer <= 0) {
		blink_ready = true;
	}
}

void player_controller_speedy::resetBlinkTimer()
{
	blink_timer = blink_cooldown;
	blink_ready = false;
}

void player_controller_speedy::updateDropWaterTimer() {
	drop_water_timer -= getDeltaTime();
	if (drop_water_timer <= 0) {
		drop_water_ready = true;
	}
}

void player_controller_speedy::resetDropWaterTimer() {
	drop_water_timer = drop_water_timer_reset;
	drop_water_ready = false;
}

void player_controller_speedy::UpdateUnpossess() {
	CHandle h = CHandle(this);
	tags_manager.removeTag(h.getOwner(), getID("target"));
}

void player_controller_speedy::DisabledState() {
}

void player_controller_speedy::InitControlState() {
	CHandle h = CHandle(this);
	tags_manager.addTag(h.getOwner(), getID("target"));
	ChangeState("idle");
	//ChangePose(pose_idle);
}
CEntity* player_controller_speedy::getMyEntity() {
	CHandle me = CHandle(this);
	return me.getOwner();
}

//Cambio de malla
void player_controller_speedy::ChangePose(string new_pose_route) {
	mesh->unregisterFromRender();
	MKeyValue atts_mesh;
	atts_mesh["name"] = new_pose_route;
	mesh->load(atts_mesh);
	mesh->registerToRender();
}

void player_controller_speedy::SetCharacterController()
{
	SetMyEntity();
	cc = myEntity->get<TCompCharacterController>();
}
