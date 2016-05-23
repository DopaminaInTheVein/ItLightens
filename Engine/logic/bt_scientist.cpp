#include "mcv_platform.h"
#include "bt_scientist.h"
#include "components\comp_charactercontroller.h"
#include "components\entity_parser.h"
#include "app_modules\logic_manager\logic_manager.h"
#include "ai_beacon.h"
#include "ai_workbench.h"

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
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_cientifico")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields = readIniAtrData(file_ini, "bt_scientist");

			assignValueToVar(move_speed, fields);
			assignValueToVar(rot_speed, fields);
			assignValueToVar(square_range_action, fields);
			assignValueToVar(d_epsilon, fields);
			assignValueToVar(d_beacon_simple, fields);
			assignValueToVar(waiting_time, fields);
			assignValueToVar(t_addBeacon, fields);
			assignValueToVar(t_createBeacon, fields);
			assignValueToVar(t_removeBeacon, fields);
			assignValueToVar(t_waitInPos, fields);

		}
	}
}

void bt_scientist::Init() {
	//read main attributes from file
	readIniFileAttr();

	om = getHandleManager<bt_scientist>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	if (tree.empty()) {
		addBtPossStates();
		addChild("possessable", "scientist", PRIORITY, (btcondition)&bt_scientist::npcAvailable, NULL);
		/*addChild("speedy", "dash_start", DECORATOR_LUA, (btcondition)&bt_scientist::dashReady, logic_manager->OnDash, "PARAMETROS");
		addChild("dash_start", "dash", RANDOM, NULL, NULL);
		addChild("dash", "dashNextPoint", ACTION, NULL, (btaction)&bt_scientist::actionDashPoint);
		addChild("dash", "dashNewPoint", ACTION, NULL, (btaction)&bt_scientist::actionDashNewPoint);
		addChild("dash", "dashPlayer", ACTION, NULL, (btaction)&bt_scientist::actionDashPlayer);
		addChild("speedy", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "nextWpt", ACTION, NULL, (btaction)&bt_scientist::actionNextWpt);
		addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_scientist::actionSeekWpt);*/
	}

	// transforms for the speedy and the player
	SetMyEntity();

	out[IDLE] = "IDLE";
	out[CREATE_BEACON] = "CREATE_BEACON";
	out[ADD_BEACON] = "ADD_BEACON";
	out[REMOVE_BEACON] = "REMOVE_BEACON";
	out[WANDER] = "WANDER";

	SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?

	curkpt = 0;
}

void bt_scientist::update(float elapsed) {
	// If we become possessed, reset the tree and stop all actions
	if (possessing)
		setCurrent(NULL);
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

	zmin = atts.getFloat("zmin", 0.0f);
	zmax = atts.getFloat("zmax", 0.0f);

	return true;
}

// conditions

//TODO

// actions
int bt_scientist::LookForObj() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	if (beacon_to_go_name != "") {
		//ChangeState("seekWB");
	}
	else if (keyPoints.size() > 0) {
		//ChangeState("nextKpt");
	}

	return OK;
}

int bt_scientist::SeekWorkbench() {
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
			if (wb_pos.z > zmin && wb_pos.z < zmax) {
				obj_position = wb_pos;
				wb_to_go_name = name;
				actual_action = CREATE_BEACON;
				SBB::postInt(name, workbench_controller::INACTIVE_TAKEN);
				//ChangeState("aimToPos");
				return OK;
			}
		}
	}

	//ChangeState("nextKpt");

	return OK;
}
int bt_scientist::AimToPos() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return KO;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	VEC3 target = obj_position;
	float delta_time = getDeltaTime();

	//rotate
	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);
	float new_yaw;
	float diff_yaw = me_transform->getDeltaYawToAimTo(target);

	if (diff_yaw < 0) {
		new_yaw = -1;
	}
	else if (diff_yaw > 0) {
		new_yaw = 1;
	}
	else {
		new_yaw = 0;
	}

	new_yaw = delta_time*rot_speed*new_yaw;

	if (fabs(new_yaw) > fabs(diff_yaw)) //if rot too big
		new_yaw = diff_yaw;				//aim to obj point

	me_transform->setAngles(new_yaw + yaw, pitch);

	if (new_yaw == diff_yaw) {
		if (actual_action != WANDER || keyPoints[curkpt].type == Seek) {
			//ChangeState("moveToPos");
		}
		else {
			//ChangeState("waitInPos");
		}
	}

	return OK;
}
int bt_scientist::MoveToPos() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return KO;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();
	TCompCharacterController *cc = myEntity->get<TCompCharacterController>();

	VEC3 target = obj_position;
	float delta_time = getDeltaTime();


	cc->AddMovement(me_transform->getFront(), move_speed);
	VEC3 new_pos = cc->GetFootPosition();


	float dist_square = simpleDistXZ(new_pos, target);

	if (dist_square < d_epsilon) {		//TODO: System to read action to do: add beacon, remove beacon, create beacon from WB
		if (actual_action == CREATE_BEACON) {
			//ChangeState("createBeacon");
		}
		else if (actual_action == ADD_BEACON) {
			//ChangeState("addBeacon");
		}
		else if (actual_action == REMOVE_BEACON) {
			//ChangeState("removeBeacon");
		}
		else if (actual_action == WANDER) {
			//ChangeState("waitInPos");
		}
	}

	return OK;
}
int bt_scientist::CreateBeaconFromWB() {
	waiting_time += getDeltaTime();
	if (waiting_time > t_createBeacon) {		//go to beacon
		obj_position = beacon_to_go;
		actual_action = ADD_BEACON;
		SBB::postInt(wb_to_go_name.c_str(), workbench_controller::INACTIVE);
		wb_to_go_name = "";
		waiting_time = 0.0f;
		//ChangeState("aimToPos");
	}

	return OK;
}
int bt_scientist::AddBeacon() {
	waiting_time += getDeltaTime();
	if (waiting_time > t_addBeacon) {		//go to new action
		SBB::postInt(beacon_to_go_name, beacon_controller::SONAR);
		waiting_time = 0.0f;
		beacon_to_go_name = "";
		actual_action = IDLE;
		//ChangeState("lookForObj");
	}

	return OK;
}
int bt_scientist::RemoveBeacon() {
	waiting_time += getDeltaTime();
	if (waiting_time > t_removeBeacon) {		//go to new action
		SBB::postInt(beacon_to_go_name, beacon_controller::INACTIVE);
		waiting_time = 0.0f;
		actual_action = IDLE;
		//ChangeState("lookForObj");
	}

	return OK;
}
int bt_scientist::WaitInPos() {
	if (t_waitInPos > keyPoints[curkpt].time) {
		//ChangeState("lookForObj");
		curkpt = (curkpt + 1) % keyPoints.size();
	}
	else {
		t_waitInPos += getDeltaTime();
	}

	return OK;
}
int bt_scientist::NextKpt() {
	actual_action = WANDER;
	obj_position = keyPoints[curkpt].pos;
	t_waitInPos = 0;
	//ChangeState("aimToPos");

	return OK;
}

//Messages:
void bt_scientist::onRemoveBeacon(const TMsgBeaconToRemove& msg)
{
	if (actual_action == IDLE || actual_action == WANDER) {
		//TODO: preference for closest objectives
		if (SBB::readInt(msg.name_beacon) != beacon_controller::TO_REMOVE_TAKEN) {
			if (msg.pos_beacon.z > zmin && msg.pos_beacon.z < zmax) {
				SBB::postInt(msg.name_beacon, beacon_controller::TO_REMOVE_TAKEN);
				obj_position = beacon_to_go = msg.pos_beacon;
				beacon_to_go_name = msg.name_beacon;
				actual_action = REMOVE_BEACON;
				//ChangeState("aimToPos");
			}
		}
	}
}

void bt_scientist::onEmptyBeacon(const TMsgBeaconEmpty & msg)
{
	if (actual_action == IDLE || actual_action == WANDER) {
		if (SBB::readInt(msg.name) != beacon_controller::INACTIVE_TAKEN) {
			if (msg.pos.z > zmin && msg.pos.z < zmax) {
				beacon_to_go = msg.pos;
				beacon_to_go_name = msg.name;
				actual_action = CREATE_BEACON;
				SBB::postInt(msg.name, beacon_controller::INACTIVE_TAKEN);	//disable beacon for other bots
				//ChangeState("seekWB");
			}
		}
	}
}

void bt_scientist::onEmptyWB(const TMsgWBEmpty & msg)
{
}

void bt_scientist::onTakenBeacon(const TMsgBeaconTakenByPlayer & msg)
{
	if (msg.name == beacon_to_go_name) {
		waiting_time = 0.0f;
		beacon_to_go_name = "";
		actual_action = IDLE;
		//ChangeState("lookForObj");
	}
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
		//ChangeState(ST_STUNT);
	}
}

// ImGui support
void bt_scientist::renderInMenu()
{
	ImGui::Text("Node: %s", out[actual_action].c_str());
	ImGui::Text("Timer: %.4f", waiting_time);
	ImGui::Text("Objective: %s", beacon_to_go_name.c_str());
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

