#include "mcv_platform.h"
#include "bt_speedy.h"
#include "components\comp_charactercontroller.h"

void bt_speedy::Init() {

	om = getHandleManager<bt_speedy>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	addChild("possessable", "speedy", PRIORITY, (btcondition)&bt_speedy::npcAvailable, NULL);
	addChild("speedy", "dash", RANDOM, (btcondition)&bt_speedy::dashReady, NULL);
	addChild("dash", "dashNextPoint", ACTION, NULL, (btaction)&bt_speedy::actionDashPoint);
	addChild("dash", "dashNewPoint", ACTION, NULL, (btaction)&bt_speedy::actionDashNewPoint);
	addChild("dash", "dashPlayer", ACTION, NULL, (btaction)&bt_speedy::actionDashPlayer);
	addChild("speedy", "patrol", SEQUENCE, NULL, NULL);
	addChild("patrol", "nextWpt", ACTION, NULL, (btaction)&bt_speedy::actionNextWpt);
	addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_speedy::actionSeekWpt);

	// transforms for the speedy and the player
	SetMyEntity();

	// current wpt
	curwpt = 0;

	// dash timer initialization
	dash_timer = (float)dash_timer_reset;
	dash_ready = true;
	dash_target = VEC3(0, 0, 0);

	// drop water timer initialization
	drop_water_timer = drop_water_timer_reset;
	drop_water_ready = false;

	// Mesh management
	mesh = myEntity->get<TCompRenderStaticMesh>();

	pose_idle_route = "static_meshes/speedy/speedy.static_mesh";
	pose_jump_route = "static_meshes/speedy/speedy_jump.static_mesh";
	pose_run_route = "static_meshes/speedy/speedy_run.static_mesh";

}

void bt_speedy::update(float elapsed) {
	// Update transforms
	SetMyEntity();
	transform = myEntity->get<TCompTransform>();
	CEntity * player_e = player;
	player_transform = player_e->get<TCompTransform>();
	// Recalc AI and update timers
	updateDashTimer();
	updateDropWaterTimer();
	// If we become possessed, reset the tree and stop all actions
	if (possessing)
		setCurrent(NULL);
	Recalc();
}

void bt_speedy::onSetPlayer(const TMsgSetPlayer& msg) {
	player = msg.player;
}

// Loading the wpts
#define WPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d_%s", index, nameSufix);

bool bt_speedy::load(MKeyValue& atts) {
	int n = atts.getInt("wpts_size", 0);
	fixedWpts.resize(n);
	for (int i = 0; i < n; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		fixedWpts[i] = atts.getPoint(atrPos);
	}
	return true;
}

// Sets the entity
void bt_speedy::SetMyEntity() {
	myEntity = myParent;
}

// conditions
bool bt_speedy::dashReady() {
	return dash_ready;
}
// actions
int bt_speedy::actionNextWpt() {
	VEC3 front = transform->getFront();
	VEC3 target = fixedWpts[curwpt];
	bool aimed = aimToTarget(target);

	if (aimed) {
		ChangePose(pose_run_route);
		//ChangeState("seekwpt");
		return OK;
	}
	return STAY;
}

int bt_speedy::actionSeekWpt() {

	float distance = squaredDistXZ(fixedWpts[curwpt], transform->getPosition());

	 if (abs(distance) > 0.1f) {
		moveFront(speed);
		return STAY;
	}
	else {
		transform->setPosition(fixedWpts[curwpt]);
		float distance_to_next_wpt = squaredDist(transform->getPosition(), fixedWpts[(curwpt + 1) % fixedWpts.size()]);
		curwpt = (curwpt + 1) % fixedWpts.size();

		return OK;
	}
}

int bt_speedy::actionDashPoint() {	

	bool arrived = dashToTarget(fixedWpts[curwpt]);
	if (arrived) {
		resetDashTimer();
		ChangePose(pose_idle_route);
		return OK;
	}
	return STAY;
}
int bt_speedy::actionDashNewPoint() {

	if (random_wpt == -1)
		random_wpt = rand() % fixedWpts.size();

	bool arrived = dashToTarget(fixedWpts[random_wpt]);
	if (arrived) {
		resetDashTimer();
		ChangePose(pose_idle_route);
		random_wpt = -1;
		return OK;
	}
	return STAY;
}
int bt_speedy::actionDashPlayer() {

	dash_target = player_transform->getPosition();
	float distance_to_player = squaredDistXZ(dash_target, transform->getPosition());
	if (abs(distance_to_player) > max_dash_player_distance || abs(dash_target.y - transform->getPosition().y) > 0.5f) {
		return KO;
	}

	bool arrived = dashToTarget(dash_target);
	if (arrived) {
		resetDashTimer();
		ChangePose(pose_idle_route);
		return OK;
	}
	return STAY;
}

bool bt_speedy::aimToTarget(VEC3 target) {

	float delta_yaw = transform->getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		transform->getAngles(&yaw, &pitch);
		transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
		return false;
	}
	else {
		return true;
	}
}

void bt_speedy::moveFront(float movement_speed) {
	VEC3 front = transform->getFront();
	VEC3 position = transform->getPosition();
	TCompCharacterController *cc = myEntity->get<TCompCharacterController>();
	cc->AddMovement(VEC3(front.x*movement_speed, 0.0f, front.z*movement_speed));
}

bool bt_speedy::dashToTarget(VEC3 target) {
	bool aimed = aimToTarget(target);

	if (aimed) {
		moveFront(dash_speed);
		/*if (drop_water_ready) {
			VEC3 player_pos = transform->getPosition();

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
			atts_name["name"] = "speedy_water";
			new_name_h.load(atts_name);
			e->add(new_name_h);
			// create transform component
			auto hm_transform = CHandleManager::getByName("transform");
			CHandle new_transform_h = hm_transform->createHandle();
			MKeyValue atts;
			// position, rotation and scale
			char position[64]; sprintf(position, "%f %f %f", player_pos.x, player_pos.y, player_pos.z);
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
	}

	float distance = squaredDistXZ(target, transform->getPosition());

	if (distance < 0.5f) {
		return true;
	}
	else {
		return false;
	}
}

void bt_speedy::updateDashTimer() {
	dash_timer -= getDeltaTime();
	if (dash_timer <= 0) {
		dash_ready = true;
	}
}

void bt_speedy::resetDashTimer() {
	dash_timer = dash_timer_reset;
	dash_ready = false;
}

void bt_speedy::updateDropWaterTimer() {
	drop_water_timer -= getDeltaTime();
	if (drop_water_timer <= 0) {
		drop_water_ready = true;
	}
}

void bt_speedy::resetDropWaterTimer() {
	drop_water_timer = drop_water_timer_reset;
	drop_water_ready = false;
}

//Cambio de malla
void bt_speedy::ChangePose(string new_pose_route) {
	if (last_pose != new_pose_route) {
		mesh->unregisterFromRender();
		MKeyValue atts_mesh;
		atts_mesh["name"] = new_pose_route;
		mesh->load(atts_mesh);
		mesh->registerToRender();
		last_pose = new_pose_route;
	}
}

