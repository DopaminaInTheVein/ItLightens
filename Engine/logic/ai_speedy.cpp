#include "mcv_platform.h"
#include "ai_speedy.h"

void ai_speedy::Init()
{
	// insert all states in the map
//	AddState("idle", (statehandler)&ai_speedy::IdleState);
	AddState("nextwpt", (statehandler)&ai_speedy::NextWptState);
	AddState("seekwpt", (statehandler)&ai_speedy::SeekWptState);
	AddState("dashtoplayer", (statehandler)&ai_speedy::DashToPlayerState);
	AddState("dashtopoint", (statehandler)&ai_speedy::DashToPointState);
	AddState("dashtonewpoint", (statehandler)&ai_speedy::DashToNewPointState);
	om = getHandleManager<ai_speedy>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

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

	// reset the state
	ChangeState("idle");
}

void ai_speedy::onSetPlayer(const TMsgSetPlayer& msg) {
	player = msg.player;
}

void ai_speedy::update(float elapsed) {
	// Update transforms
	SetMyEntity();
	transform = myEntity->get<TCompTransform>();
	CEntity * player_e = player;
	player_transform = player_e->get<TCompTransform>();
	// Recalc AI and update timers
	updateDashTimer();
	updateDropWaterTimer();
	Recalc();
}

// Loading the wpts
#define WPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d_%s", index, nameSufix);

bool ai_speedy::load(MKeyValue& atts) {
	int n = atts.getInt("wpts_size", 0);
	fixedWpts.resize(n);
	for (unsigned int i = 0; i < n; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		fixedWpts[i] = atts.getPoint(atrPos);
	}
	return true;
}

// Sets the entity
void ai_speedy::SetMyEntity() {
	myEntity = myParent;
}

// Speedy states

void ai_speedy::IdleState() {
	ChangeState("nextwpt");
}

void ai_speedy::NextWptState()
{
	VEC3 front = transform->getFront();
	VEC3 target = fixedWpts[curwpt];

	bool aimed = aimToTarget(target);

	if (aimed) {
		ChangeState("seekwpt");
	}
}

void ai_speedy::SeekWptState() 
{
	float distance = squaredDistXZ(fixedWpts[curwpt], transform->getPosition());

	string next_action = decide_next_action();

	if (next_action == "dashtoplayer" && dash_ready) {
		dash_target = player_transform->getPosition();
		float distance_to_player = squaredDistXZ(dash_target, transform->getPosition());
		if (abs(distance_to_player) <= max_dash_player_distance)
			ChangeState(next_action);
		else if (abs(distance) > 0.1f) 
			moveFront(speed);
	}
	else if (next_action == "dashtonewpoint" && dash_ready) {
		dash_target = VEC3(30.f, 0.0f, 0.0f);
		ChangeState(next_action);
	}
	else if (next_action == "dashtopoint" && dash_ready) {
		ChangeState(next_action);
	}
	else if (abs(distance) > 0.1f) {
		moveFront(speed);
	}
	else {
		transform->setPosition(fixedWpts[curwpt]);
		curwpt = (curwpt + 1) % fixedWpts.size();
		ChangeState("nextwpt");
	}
}

void ai_speedy::DashToPlayerState() {
	bool arrived = dashToTarget(dash_target);
	if (arrived) {
		resetDashTimer();
		ChangeState("nextwpt");
	}
}
void ai_speedy::DashToPointState() {
	bool arrived = dashToTarget(fixedWpts[curwpt]);
	if (arrived) {
		resetDashTimer();
		ChangeState("nextwpt");
	}
}

void ai_speedy::DashToNewPointState() {
	bool arrived = dashToTarget(dash_target);
	if (arrived) {
		resetDashTimer();
		ChangeState("nextwpt");
	}
}

string ai_speedy::decide_next_action() {
	int next_action = rand() % 100;

	if (next_action < dash_to_point_chance) {
		return "dashtopoint";
	}
	else if (next_action < dash_to_point_chance + dash_to_new_point_chance) {
		return "dashtonewpoint";
	}
	else if (next_action < dash_to_point_chance + dash_to_new_point_chance + dash_to_player_chance) {
		return "dashtoplayer";
	}
	else {
		return "seekwpt";
	}
}

bool ai_speedy::aimToTarget(VEC3 target) {
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

void ai_speedy::moveFront(float movement_speed) {
	VEC3 front = transform->getFront();
	VEC3 position = transform->getPosition();

	transform->setPosition(VEC3(position.x + front.x*movement_speed*getDeltaTime(), position.y, position.z + front.z*movement_speed*getDeltaTime()));
}

bool ai_speedy::dashToTarget(VEC3 target) {
	bool aimed = aimToTarget(target);

	if (aimed) {
		moveFront(dash_speed);
		if (drop_water_ready) {

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
		}
	}

	float distance = squaredDistXZ(target, transform->getPosition());

	if (distance < 0.1f) {
		return true;
	}
	else {
		return false;
	}
}

void ai_speedy::updateDashTimer() {
	dash_timer -= getDeltaTime();
	if (dash_timer <= 0) {
		dash_ready = true;
	}
}

void ai_speedy::resetDashTimer() {
	dash_timer = dash_timer_reset;
	dash_ready = false;
}

void ai_speedy::updateDropWaterTimer() {
	drop_water_timer -= getDeltaTime();
	if (drop_water_timer <= 0) {
		drop_water_ready = true;
	}
}

void ai_speedy::resetDropWaterTimer() {
	drop_water_timer = drop_water_timer_reset;
	drop_water_ready = false;
}