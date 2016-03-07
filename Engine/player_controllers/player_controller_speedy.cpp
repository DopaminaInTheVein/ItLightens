#include "mcv_platform.h"
#include "player_controller_speedy.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"
#include "app_modules\io\io.h"
#include "components\comp_msgs.h"

#include "physics/physics.h"

void player_controller_speedy::Init()
{
	om = getHandleManager<player_controller_speedy>();	//player

	AddState("dashing", (statehandler)&player_controller_speedy::Dashing);
	AddState("blinking", (statehandler)&player_controller_speedy::Blinking);
	AddState("blink", (statehandler)&player_controller_speedy::Blink);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
	myEntity = myParent;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();

	drop_water_timer = drop_water_timer_reset;

	//Mallas
	pose_run = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_jump = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	CEntity* myEntity = myParent;
	pose_idle = myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	actual_render = pose_idle;

	pose_idle.setOwner(myEntity);
	pose_run.setOwner(myEntity);
	pose_jump.setOwner(myEntity);

	TCompRenderStaticMesh *mesh;

	mesh = pose_jump;
	mesh->static_mesh = Resources.get("static_meshes/speedy_jump.static_mesh")->as<CStaticMesh>();

	mesh = pose_run;
	mesh->static_mesh = Resources.get("static_meshes/speedy_run.static_mesh")->as<CStaticMesh>();

	actual_render->registerToRender();

	ChangeState("idle");
}

void player_controller_speedy::myUpdate() {
	energyDecreasal(getDeltaTime()*0.5f);
	updateDashTimer();
	updateBlinkTimer();
	updateDropWaterTimer();
	if (dashing) {
		ChangePose(pose_run);
		ChangeState("dashing");
	}
}

void player_controller_speedy::UpdateInputActions() {
	if (io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed()) {
		if (dash_ready) {
			energyDecreasal(5.0f);
			ChangePose(pose_run);
			ChangeState("dashing");
			dashing = true;
		}
	}
	if (io->mouse.right.becomesPressed() || io->joystick.button_B.becomesPressed()) {
		if (blink_ready) {
			energyDecreasal(10.0f);
			ChangeState("blink");
		}
	}
}

void player_controller_speedy::Dashing()
{
	if (dash_ready) {
		bool arrived = dashFront();
		if (arrived) {
			dashing = false;
			resetDashTimer();
			ChangeState("idle");
			ChangePose(pose_idle);
		}
		else {
			ChangePose(pose_run);
		}
	}
}

void player_controller_speedy::Blinking()
{
	if (io->mouse.right.isPressed() || io->joystick.button_B.becomesPressed()) {
		ChangePose(pose_jump);
		blink_duration -= getDeltaTime();

		if (blink_ready && blink_duration <= 0)
			ChangeState("blink");
	}
	else {
		//blink_duration = max_blink_duration;
		ChangePose(pose_idle);
		ChangeState("idle");
	}
}

void player_controller_speedy::Blink()
{
	if (blink_ready) {
		SetMyEntity();
		TCompTransform* player_transform = myEntity->get<TCompTransform>();
		VEC3 player_position = player_transform->getPosition();
		VEC3 player_front = player_transform->getFront();
		float dist, distCollision;
		if (collisionBlink(distCollision)) {
			dist = distCollision;
		}
		else {
			dist = blink_distance;
		}
		player_position += player_front * dist;

		player_transform->setPosition(player_position);

		resetBlinkTimer();
	}
	ChangeState("idle");
	ChangePose(pose_idle);
}

bool player_controller_speedy::dashFront()
{
	dash_duration += getDeltaTime();

	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	VEC3 player_front = player_transform->getFront();

	VEC3 new_position = VEC3(player_position.x + player_front.x*dash_speed*getDeltaTime(), player_position.y, player_position.z + player_front.z*dash_speed*getDeltaTime());
	player_transform->setPosition(new_position);

	if (drop_water_ready) {
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
	}

	if (dash_duration > dash_max_duration || collisionWall()) {
		dash_duration = 0;
		return true;
	}
	else {
		return false;
	}
}
bool player_controller_speedy::collisionWall() {
	float distFirstCollider; // No lo uso
	CHandle collider = rayCastToFront(COL_TAG_SOLID, 1.0f, distFirstCollider);
	return collider.isValid();
}

bool player_controller_speedy::collisionBlink(float& distCollision) {
	CHandle collider = rayCastToFront(COL_TAG_SOLID_OPAQUE, blink_distance, distCollision);
	return collider.isValid();
}

CHandle player_controller_speedy::rayCastToFront(int types, float reach, float& distRay) {
	CHandle me = CHandle(this).getOwner();
	CEntity* eMe = me;
	TCompTransform* tMe = eMe->get<TCompTransform>();

	ray_cast_query rcQuery;
	rcQuery.position = tMe->getPosition() + VEC3(0, 0.5, 0);
	rcQuery.direction = tMe->getFront();
	rcQuery.maxDistance = reach;
	rcQuery.types = types;
	ray_cast_result res = Physics::calcRayCast(rcQuery);
	distRay = realDist(res.positionCollision, tMe->getPosition());
	return res.firstCollider;
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

void player_controller_speedy::DisabledState() {
}

void player_controller_speedy::InitControlState() {
	ChangeState("idle");
	ChangePose(pose_idle);
}
CEntity* player_controller_speedy::getMyEntity() {
	CHandle me = CHandle(this);
	return me.getOwner();
}

//Cambio de malla
void player_controller_speedy::ChangePose(CHandle new_pos_h)
{
	TCompRenderStaticMesh *new_pose = new_pos_h;
	if (new_pose == actual_render) return;
	actual_render->unregisterFromRender();
	actual_render = new_pose;
	actual_render->registerToRender();
}