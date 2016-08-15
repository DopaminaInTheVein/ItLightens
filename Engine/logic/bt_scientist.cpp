#include "mcv_platform.h"
#include "bt_scientist.h"
#include "components\comp_charactercontroller.h"
#include "components\entity_parser.h"
#include "app_modules\logic_manager\logic_manager.h"
#include "ai_workbench.h"

#define SET_ANIM_SCI_BT(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_SCI_BT_P(state) SET_ANIM_STATE_P(animController, state)

map<int, std::string> bt_scientist::out = {};
map<string, bt_scientist::KptTipo> bt_scientist::kptTypes = {
  { "seek", KptTipo::Seek }
  ,{ "look", KptTipo::Look }
};

map<string, btnode *> bt_scientist::tree = {};
map<string, btaction> bt_scientist::actions = {};
map<string, btcondition> bt_scientist::conditions = {};
map<string, btevent> bt_scientist::events = {};
btnode* bt_scientist::root = nullptr;

void bt_scientist::readIniFileAttr() {
	ClHandle h = ClHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_cientifico")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields = readIniAtrData(file_ini, "bt_scientist");

			assignValueToVar(move_speed, fields);
			assignValueToVar(rot_speed, fields);
			assignValueToVar(square_range_action, fields);
			assignValueToVar(max_wb_distance, fields);
			assignValueToVar(max_beacon_distance, fields);
			assignValueToVar(reach_dist_pnt, fields);
			assignValueToVar(d_epsilon, fields);
			assignValueToVar(d_beacon_simple, fields);
			assignValueToVar(waiting_time, fields);
			assignValueToVar(t_waitInPos, fields);
			assignValueToVar(ws_wait_time, fields);
			assignValueToVar(ws_wait_time_offset, fields);
		}
	}
}

void bt_scientist::Init() {
	getUpdateInfoBase(ClHandle(this).getOwner());
	//read main attributes from file
	readIniFileAttr();

	om = getHandleManager<bt_scientist>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	if (tree.empty()) {
		addBtPossStates();
		addChild("possessable", "scientist", PRIORITY, (btcondition)&bt_scientist::npcAvailable, NULL);
		// formation toggle
		addChild("scientist", "busystate", SEQUENCE, (btcondition)&bt_scientist::checkBusy, NULL);
		addChild("busystate", "selectworkstation", ACTION, NULL, (btaction)&bt_scientist::actionSelectWorkstation);
		addChild("busystate", "gotoworkstation", ACTION, NULL, (btaction)&bt_scientist::actionGoToWorkstation);
		addChild("busystate", "waitinworkstation", ACTION, NULL, (btaction)&bt_scientist::actionWaitInWorkstation);
		// stunned state
		addChild("scientist", "stunned", ACTION, (btcondition)&bt_scientist::playerStunned, (btaction)&bt_scientist::actionStunned);
		// create beacon sequence
		addChild("scientist", "createbeacon", SEQUENCE, (btcondition)&bt_scientist::workbenchAvailable, NULL);
		addChild("createbeacon", "aimcreate", ACTION, NULL, (btaction)&bt_scientist::actionAimToPos);
		addChild("createbeacon", "gocreate", ACTION, NULL, (btaction)&bt_scientist::actionMoveToPos);
		// patrol sequence
		addChild("scientist", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "nextwpt", ACTION, NULL, (btaction)&bt_scientist::actionNextWpt);
		addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_scientist::actionSeekWpt);
		addChild("patrol", "waitwpt", ACTION, NULL, (btaction)&bt_scientist::actionWaitWpt);
	}
	// transforms for the player
	SetMyEntity();

	out[IDLE] = "IDLE";
	out[WANDER] = "WANDER";

	actual_action = IDLE;

	SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?
	____TIMER_REDEFINE_(timerStunt, 15);
	t_waitInPos = 0;

	if (animController) {
		animController->setState(AST_IDLE);
	}
	SET_ANIM_SCI_BT(AST_IDLE);
	curkpt = 0;
	initParent();
}

bool bt_scientist::getUpdateInfo()
{
	animController = GETH_MY(SkelControllerScientist);
	return true;
}

void bt_scientist::update(float elapsed) {
	// If we become possessed, reset the tree and stop all actions
	SetMyEntity();
	if (!myEntity) return;
	if (!isInRoom(myParent))return;

	if (possessing)
		setCurrent(NULL);
	if (stunned)
		SET_ANIM_SCI_BT(AST_STUNNED);
	Recalc();
}

// Loading the wpts
#define KPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "kpt%d_%s", index, nameSufix);

bool bt_scientist::load(MKeyValue& atts) {
	int n = atts.getInt("kpt_size", 0);
	keyPoints.resize(n);
	for (int i = 0; i < n; i++) {
		KPT_ATR_NAME(atrType, "type", i);
		KPT_ATR_NAME(atrPos, "pos", i);
		KPT_ATR_NAME(atrWait, "wait", i);
		keyPoints[i] = KeyPoint(
			kptTypes[atts.getString(atrType, "seek")]
			, atts.getPoint(atrPos)
			, atts.getFloat(atrWait, 0.0f)
			);
	}

	zmin = atts.getFloat("zmin", 100.0f);
	zmax = atts.getFloat("zmax", 0.1f);

	load_bt(atts);
	return true;
}

bool bt_scientist::save(std::ofstream& os, MKeyValue& atts)
{
	int n = keyPoints.size();
	atts.put("kpt_size", n);
	for (int i = 0; i < n; i++) {
		KPT_ATR_NAME(atrType, "type", i);
		KPT_ATR_NAME(atrPos, "pos", i);
		KPT_ATR_NAME(atrWait, "wait", i);
		atts.put(atrType, keyPoints[i].type);
		atts.put(atrPos, keyPoints[i].pos);
		atts.put(atrWait, keyPoints[i].time);
	}

	atts.put("zmin", zmin);
	atts.put("zmax", zmax);
	save_bt(os, atts);

	return true;
}

// conditions
bool bt_scientist::playerStunned() {
	PROFILE_FUNCTION("scientist: player stunned");
	if (stunned == true) {
		logic_manager->throwEvent(logic_manager->OnStunned, "");
		logic_manager->throwEvent(logic_manager->OnGuardAttackEnd, "");
		return true;
	}
	return false;
}

bool bt_scientist::workbenchAvailable() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	int wbs = workbench_controller::id_curr_max_wb;	//the last number exists
	std::string base_name = "workbench_";
	bool found = false;

	for (int i = 1; i <= wbs; i++) {		//fisrt start at 1
		std::string name = base_name + std::to_string(i);
		if (SBB::readInt(name) == workbench_controller::INACTIVE) {
			VEC3 wb_pos = SBB::readVEC3(name);
			float square_dist = squaredDistXZ(curr_pos, wb_pos);
			if (square_dist > max_wb_distance)
				continue;
			obj_position = wb_pos;
			wb_to_go_name = name;
			SBB::postInt(name, workbench_controller::INACTIVE_TAKEN);
			return true;
		}
	}

	return false;
}

//toggle conditions
bool bt_scientist::checkBusy() {
	return busy_state_toggle;
}

// actions
int bt_scientist::actionStunned() {
	PROFILE_FUNCTION("scientist: actionstunned");
	if (!myParent.isValid()) return false;
	if (timerStunt < 0) {
		stunned = false;
		logic_manager->throwEvent(logic_manager->OnStunnedEnd, "");
		return OK;
	}
	else {
		SET_ANIM_SCI_BT(AST_STUNNED);
		if (timerStunt > -1)
			timerStunt -= getDeltaTime();
		return STAY;
	}
	return OK;
}

int bt_scientist::actionAimToPos() {
	//Look to next target position
	if (turnTo(obj_position)) {
		SetMyEntity(); //needed in case address Entity moved by handle_manager
		if (!myEntity) return KO;
		TCompTransform *me_transform = myEntity->get<TCompTransform>();
		VEC3 myPos = me_transform->getPosition();
		getPath(myPos, obj_position);
		return OK;
	}
	else {
		SET_ANIM_SCI_BT(AST_IDLE);
		return STAY;
	}
}

int bt_scientist::actionMoveToPos() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return KO;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 myPos = me_transform->getPosition();

	float dist = simpleDistXZ(myPos, obj_position);

	//reach waypoint?
	if (dist < reach_dist_pnt) {
		curkpt = (curkpt + 1) % keyPoints.size();
		return OK;
	}
	else {
		//getPath(myPos, obj_position);
		SET_ANIM_SCI_BT(AST_MOVE);
		goTo(obj_position);
		return STAY;
	}
}

int bt_scientist::actionSeekWpt() {
	PROFILE_FUNCTION("scientist: actionseekwpt");
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return KO;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 myPos = me_transform->getPosition();
	VEC3 dest = keyPoints[curkpt].pos;
	//Go to waypoint
	if (keyPoints[curkpt].type == Seek) {
		//reach waypoint?
		if (simpleDistXZ(myPos, dest) < reach_dist_pnt) {
			curkpt = (curkpt + 1) % keyPoints.size();
			return OK;
		}
		else {
			//getPath(myPos, dest);
			//animController.setState(AST_RUN);
			goTo(dest);
			SET_ANIM_SCI_BT(AST_MOVE);
			return STAY;
		}
	}
	//Look to waypoint
	else if (keyPoints[curkpt].type == Look) {
		//Look to waypoint
		if (turnTo(dest)) {
			curkpt = (curkpt + 1) % keyPoints.size();
			return OK;
		}
		else {
			SET_ANIM_SCI_BT(AST_IDLE);
			return STAY;
		}
	}

	return OK;
}

int bt_scientist::actionNextWpt() {
	PROFILE_FUNCTION("scientist: actionnextwpt");
	if (!myParent.isValid()) return false;
	//animController.setState(AST_TURN);
	VEC3 dest = keyPoints[curkpt].pos;
	//Look to waypoint
	if (turnTo(dest)) {
		SetMyEntity(); //needed in case address Entity moved by handle_manager
		if (!myEntity) return KO;
		TCompTransform *me_transform = myEntity->get<TCompTransform>();
		VEC3 myPos = me_transform->getPosition();
		getPath(myPos, dest);
		return OK;
	}
	else {
		SET_ANIM_SCI_BT(AST_IDLE);
		return STAY;
	}
}

int bt_scientist::actionWaitWpt() {
	PROFILE_FUNCTION("scientist: actionwaitwpt");
	if (!myParent.isValid()) return false;
	//animController.setState(AST_IDLE);

	if (t_waitInPos > keyPoints[curkpt].time) {
		t_waitInPos = 0;
		return OK;
	}
	else {
		t_waitInPos += getDeltaTime();
		SET_ANIM_SCI_BT(AST_IDLE);
		return STAY;
	}
}

// toggle actions
int bt_scientist::actionSelectWorkstation() {
	PROFILE_FUNCTION("scientist: selectworkbench");
	if (!myParent.isValid()) return false;

	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return KO;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 myPos = me_transform->getPosition();

	VHandles stations = tags_manager.getHandlesByTag(getID("workstation"));
	int num_stations = stations.size();

	vector<int> visited;

	while (visited.size() < num_stations) {
		int posi = rand() % num_stations;
		auto it = std::find(visited.begin(), visited.end(), posi);
		// if we already visited this workstation, go to the next
		if (it != visited.end())
			continue;
		// if we are close enough, we go to this workbench
		CEntity* entity = stations[posi];
		TCompWorkstation* workstation = entity->get<TCompWorkstation>();
		VEC3 station_pos = workstation->getActionPosition();

		float dist_to_ws = squaredDistXZ(myPos, station_pos);
		//if the workstation is not close enough, go the the next
		//if (dist_to_ws > max_wb_distance) {
		if (fabs(myPos.y - station_pos.y) > 2.0f) {
			//visited.push_back(posi);
			//continue
			return KO;
		}

		ws_anim = workstation->getAnimation();
		ws_to_go = station_pos;
		ws_yaw = workstation->getActionYaw();

		getPath(myPos, ws_to_go);
		return OK;
	}

	return KO;
}

int bt_scientist::actionGoToWorkstation() {
	PROFILE_FUNCTION("scientist: gotoworkbench");
	if (!myParent.isValid()) return false;

	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return KO;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 myPos = me_transform->getPosition();
	//reach waypoint?
	if (simpleDistXZ(myPos, ws_to_go) < reach_dist_pnt) {
		return OK;
	}
	else {
		//getPath(myPos, ws_to_go);
		//animController.setState(AST_RUN);
		SET_ANIM_SCI_BT(AST_MOVE);
		goTo(ws_to_go);
		return STAY;
	}

	return OK;
}

int bt_scientist::actionWaitInWorkstation() {
	PROFILE_FUNCTION("scientist: waitinworkbench");
	if (!myParent.isValid()) return false;

	// Turn to the workstation
	if (!turnToYaw(ws_yaw)) {
		return STAY;
	}

	// Execute the workstation animation
	//animController.setAnim(ws_anim);

	// we add a randomly generated offset to the waiting time
	int sign = rand() % 100;
	int offset = rand() % (int)ws_wait_time_offset;

	float total_wait_time = ws_wait_time;

	if (sign < 50)
		total_wait_time -= offset;
	else
		total_wait_time += offset;

	if (ws_time_waited > total_wait_time) {
		ws_time_waited = 0.f;
		return OK;
	}
	else {
		ws_time_waited += getDeltaTime();
		SET_ANIM_SCI_BT(AST_IDLE);
		return STAY;
	}

	return OK;
}

// -- Go To -- //
void bt_scientist::goTo(const VEC3& dest) {
	PROFILE_FUNCTION("scientist: go to");
	if (!SBB::readBool("navmesh")) {
		return;
	}
	VEC3 target = dest;
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 npcPos = me_transform->getPosition();
	while (totalPathWpt > 0 && currPathWpt < totalPathWpt && fabsf(squaredDistXZ(pathWpts[currPathWpt], npcPos)) < 0.5f) {
		++currPathWpt;
	}
	if (currPathWpt < totalPathWpt) {
		target = pathWpts[currPathWpt];
	}

	if (needsSteering(npcPos, me_transform, move_speed, myParent)) {
		goForward(move_speed);
	}
	else if (!me_transform->isHalfConeVision(target, deg2rad(5.0f))) {
		turnTo(target);
	}
	else {
		float distToWPT = simpleDistXZ(target, me_transform->getPosition());
		if (fabsf(distToWPT) > 0.1f && currPathWpt < totalPathWpt || fabsf(distToWPT) > 0.2f) {
			goForward(move_speed);
		}
		else if (totalPathWpt == 0) {
			goForward(-move_speed);
		}
	}
}

// -- Go Forward -- //
void bt_scientist::goForward(float stepForward) {
	PROFILE_FUNCTION("scientist: go forward");
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 myPos = me_transform->getPosition();
	float dt = getDeltaTime();
	TCompCharacterController * cc = myEntity->get<TCompCharacterController>();
	cc->AddMovement(me_transform->getFront()*stepForward*dt);
}

// -- Turn To -- //
bool bt_scientist::turnTo(VEC3 dest) {
	PROFILE_FUNCTION("scientist: turn to");
	if (!myParent.isValid()) return false;
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 myPos = me_transform->getPosition();
	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);

	float deltaAngle = rot_speed * getDeltaTime();
	float deltaYaw = me_transform->getDeltaYawToAimTo(dest);
	float angle_epsilon = deg2rad(5);

	if (abs(deltaYaw) < angle_epsilon || abs(deltaYaw) > deg2rad(355))
		return true;

	if (deltaYaw > 0) {
		if (deltaAngle < deltaYaw) yaw += deltaAngle;
		else yaw += deltaYaw;
	}
	else {
		if (deltaAngle < abs(deltaYaw)) yaw -= deltaAngle;
		else yaw += deltaYaw;
	}

	if (!me_transform->isHalfConeVision(dest, deg2rad(5.0f))) {
		bool inLeft = me_transform->isInLeft(dest);
		if (inLeft) {
			yaw += deltaAngle;
		}
		else {
			yaw -= deltaAngle;
		}
		me_transform->setAngles(yaw, pitch);
	}

	//Ha acabado el giro?
	return abs(deltaYaw) < angle_epsilon || abs(deltaYaw) > deg2rad(355);
}

// -- Turn To -- //
bool bt_scientist::turnToYaw(float yaw_dest) {
	PROFILE_FUNCTION("scientist: turn to");
	if (!myParent.isValid()) return false;
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 myPos = me_transform->getPosition();
	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);

	float deltaAngle = rot_speed * getDeltaTime();
	float deltaYaw = yaw_dest - yaw;
	float angle_epsilon = deg2rad(5);

	if (abs(deltaYaw) < angle_epsilon || abs(deltaYaw) > deg2rad(355))
		return true;

	if (deltaYaw > 0) {
		if (deltaAngle < deltaYaw) yaw += deltaAngle;
		else yaw += deltaYaw;
	}
	else {
		if (deltaAngle < abs(deltaYaw)) yaw -= deltaAngle;
		else yaw -= deltaYaw;
	}

	me_transform->setAngles(yaw, pitch);

	//Ha acabado el giro?
	return abs(deltaYaw) < angle_epsilon || abs(deltaYaw) > deg2rad(355);
}

void bt_scientist::onEmptyWB(const TMsgWBEmpty & msg)
{
}

void bt_scientist::onTakenWB(const TMsgWBTakenByPlayer & msg)
{
}

void bt_scientist::onStaticBomb(const TMsgStaticBomb & msg)
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	float d = squaredDist(msg.pos, curr_pos);

	if (d < msg.r) {
		setCurrent(NULL);
		stunned = true;
	}
}

// ImGui support
void bt_scientist::renderInMenu()
{
	ImGui::Text("Node: %s", out[actual_action].c_str());
	ImGui::Text("Timer: %.4f", waiting_time);
}

// Aux functions
void bt_scientist::SetMyEntity() {
	myEntity = myParent;
}

void bt_scientist::SetHandleMeInit()
{
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
}

bool bt_scientist::aimToTarget(VEC3 target) {
	SetMyEntity();
	if (!myEntity) return false;
	TCompTransform* transform = myEntity->get<TCompTransform>();

	float delta_yaw = transform->getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		transform->getAngles(&yaw, &pitch);
		transform->setAngles(yaw + delta_yaw*rot_speed*getDeltaTime(), pitch);
		return false;
	}
	else {
		return true;
	}
}

void bt_scientist::moveFront(float movement_speed) {
	SetMyEntity();
	if (!myEntity) return;
	TCompTransform* transform = myEntity->get<TCompTransform>();

	VEC3 front = transform->getFront();
	VEC3 position = transform->getPosition();
	TCompCharacterController *cc = myEntity->get<TCompCharacterController>();
	float dt = getDeltaTime();
	cc->AddMovement(VEC3(front.x*movement_speed*dt, 0.0f, front.z*movement_speed*dt));
}