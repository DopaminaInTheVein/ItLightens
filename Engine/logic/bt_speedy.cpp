#include "mcv_platform.h"
#include "bt_speedy.h"
#include "components\comp_charactercontroller.h"
#include "components\entity_parser.h"
#include "app_modules\logic_manager\logic_manager.h"

map<string, btnode *> bt_speedy::tree = {};
map<string, btaction> bt_speedy::actions = {};
map<string, btcondition> bt_speedy::conditions = {};
map<string, btevent> bt_speedy::events = {};
btnode* bt_speedy::root = nullptr;

void bt_speedy::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_speedy")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields = readIniAtrData(file_ini, "bt_speedy");

			assignValueToVar(speed, fields);
			assignValueToVar(rotation_speed, fields);
			rotation_speed = deg2rad(rotation_speed);
			assignValueToVar(dash_speed, fields);
			assignValueToVar(max_dash_player_distance, fields);
			assignValueToVar(dash_timer_reset, fields);
			assignValueToVar(drop_water_timer_reset, fields);
		}
	}
}

void bt_speedy::Init() {
	//read main attributes from file
	readIniFileAttr();

	om = getHandleManager<bt_speedy>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	if (tree.empty()) {
		addBtPossStates();
		addChild("possessable", "speedy", PRIORITY, (btcondition)&bt_speedy::npcAvailable, NULL);
		addChild("speedy", "dash_start", DECORATOR_LUA, (btcondition)&bt_speedy::dashReady, logic_manager->OnDash, "PARAMETROS");
		addChild("dash_start", "dash", RANDOM, NULL, NULL);
		addChild("dash", "dashNextPoint", ACTION, NULL, (btaction)&bt_speedy::actionDashPoint);
		addChild("dash", "dashNewPoint", ACTION, NULL, (btaction)&bt_speedy::actionDashNewPoint);
		addChild("dash", "dashPlayer", ACTION, NULL, (btaction)&bt_speedy::actionDashPlayer);
		addChild("speedy", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "nextWpt", ACTION, NULL, (btaction)&bt_speedy::actionNextWpt);
		addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_speedy::actionSeekWpt);
	}

	// transforms for the speedy and the player
	SetMyEntity();

	// current wpt
	curwpt = 0;

	// dash timer initialization
	dash_timer = (float)dash_timer_reset;
	dash_ready = false;
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
	if (!myEntity) return;
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
	float dt = getDeltaTime();
	cc->AddMovement(VEC3(front.x*movement_speed*dt, 0.0f, front.z*movement_speed*dt));
}

bool bt_speedy::dashToTarget(VEC3 target) {
	bool aimed = aimToTarget(target);

	if (aimed) {
		moveFront(dash_speed);
		if (drop_water_ready) {
			// CREATE WATER
			CEntity* e = spawnPrefab("speedy_water");
			if (e) {
				VEC3 posWater = transform->getPosition();
				float yaw_water, pitch_water;
				transform->getAngles(&yaw_water, &pitch_water);
				TCompTransform* t = e->get<TCompTransform>();
				t->setPosition(posWater);
				t->setAngles(yaw_water, pitch_water);
			}

			// reset drop water cooldown
			resetDropWaterTimer();
		}
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