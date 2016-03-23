#include "mcv_platform.h"
#include "ai_scientific.h"
#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "ai_workbench.h"
#include "components\comp_charactercontroller.h"

#include "ai_beacon.h"

#include "utils/XMLParser.h"

map<string, ai_scientific::KptTipo> ai_scientific::kptTypes = {
	{ "seek", KptTipo::Seek }
	,{ "look", KptTipo::Look }
};

void ai_scientific::Init()
{
	om = getHandleManager<ai_scientific>();	//list handle scientific in game

	DeleteState("idle");

	//list states
	AddState("idle", (statehandler)&ai_scientific::Idle);
	AddState("seekWB", (statehandler)&ai_scientific::SeekWorkbench);
	AddState("moveToPos", (statehandler)&ai_scientific::MoveToPos);
	AddState("aimToPos", (statehandler)&ai_scientific::AimToPos);
	AddState("createBeacon", (statehandler)&ai_scientific::CreateBeaconFromWB);
	AddState("addBeacon", (statehandler)&ai_scientific::AddBeacon);
	AddState("removeBeacon", (statehandler)&ai_scientific::RemoveBeacon);
	AddState("lookForObj", (statehandler)&ai_scientific::LookForObj);
	AddState("waitInPos", (statehandler)&ai_scientific::WaitInPos);
	AddState("nextKpt", (statehandler)&ai_scientific::NextKpt);

	out[IDLE] = "IDLE";
	out[CREATE_BEACON] = "CREATE_BEACON";
	out[ADD_BEACON] = "ADD_BEACON";
	out[REMOVE_BEACON] = "REMOVE_BEACON";
	out[WANDER] = "WANDER";

	SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?

	curkpt = 0;

	ChangeState("idle"); //init Node
}

void ai_scientific::Idle() {
	//Nothing to do
	//if (GetAsyncKeyState('Q') != 0) {
	ChangeState("lookForObj");
	//}
}

void ai_scientific::LookForObj()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	if (beacon_to_go_name != "") {
		ChangeState("seekWB");
	}
	else if (keyPoints.size() > 0) {
		ChangeState("nextKpt");
	}
}

void ai_scientific::NextKpt() {
	actual_action = WANDER;
	obj_position = keyPoints[curkpt].pos;
	t_waitInPos = 0;
	ChangeState("aimToPos");
}

void ai_scientific::SeekWorkbench()
{
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
				ChangeState("aimToPos");
				return;
			}
		}
	}

	ChangeState("nextKpt");
	//ChangeState("lookForObj");
}

void ai_scientific::AimToPos() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
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
			ChangeState("moveToPos");
		}
		else {
			ChangeState("waitInPos");
		}
	}
}

void ai_scientific::MoveToPos()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();
	TCompCharacterController *cc = myEntity->get<TCompCharacterController>();

	VEC3 target = obj_position;
	float delta_time = getDeltaTime();


	cc->AddMovement(me_transform->getFront(), move_speed*getDeltaTime());
	VEC3 new_pos = cc->getPosition() - VEC3(0, cc->GetRadius() + cc->GetHeight(), 0);


	float dist_square = simpleDistXZ(new_pos, target);

	if (dist_square < d_epsilon) {		//TODO: System to read action to do: add beacon, remove beacon, create beacon from WB
		if (actual_action == CREATE_BEACON)
			ChangeState("createBeacon");
		else if (actual_action == ADD_BEACON)
			ChangeState("addBeacon");
		else if (actual_action == REMOVE_BEACON)
			ChangeState("removeBeacon");
		else if (actual_action == WANDER) {
			ChangeState("waitInPos");
			//ChangeState("lookForObj");
		}
	}
}

void ai_scientific::WaitInPos() {
	if (t_waitInPos > keyPoints[curkpt].time) {
		ChangeState("lookForObj");
		curkpt = (curkpt + 1) % keyPoints.size();
	}
	else {
		t_waitInPos += getDeltaTime();
	}
}

void ai_scientific::CreateBeaconFromWB()
{
	waiting_time += getDeltaTime();
	if (waiting_time > t_createBeacon) {		//go to beacon
		obj_position = beacon_to_go;
		actual_action = ADD_BEACON;
		SBB::postInt(wb_to_go_name.c_str(), workbench_controller::INACTIVE);
		wb_to_go_name = "";
		waiting_time = 0.0f;
		ChangeState("aimToPos");
	}
}

void ai_scientific::AddBeacon()
{
	waiting_time += getDeltaTime();
	if (waiting_time > t_addBeacon) {		//go to new action
		SBB::postInt(beacon_to_go_name, beacon_controller::SONAR);
		waiting_time = 0.0f;
		beacon_to_go_name = "";
		actual_action = IDLE;
		ChangeState("lookForObj");
	}
}

void ai_scientific::RemoveBeacon()
{
	waiting_time += getDeltaTime();
	if (waiting_time > t_removeBeacon) {		//go to new action
		SBB::postInt(beacon_to_go_name, beacon_controller::INACTIVE);
		waiting_time = 0.0f;
		actual_action = IDLE;
		ChangeState("lookForObj");
	}
}

//Messages:
void ai_scientific::onRemoveBeacon(const TMsgBeaconToRemove& msg)
{
	if (actual_action == IDLE || actual_action == WANDER) {
		//TODO: preference for closest objectives
		if (SBB::readInt(msg.name_beacon) != beacon_controller::TO_REMOVE_TAKEN) {
			if (msg.pos_beacon.z > zmin && msg.pos_beacon.z < zmax) {
				SBB::postInt(msg.name_beacon, beacon_controller::TO_REMOVE_TAKEN);
				obj_position = beacon_to_go = msg.pos_beacon;
				beacon_to_go_name = msg.name_beacon;
				actual_action = REMOVE_BEACON;
				ChangeState("aimToPos");
			}
		}
	}
}

void ai_scientific::onEmptyBeacon(const TMsgBeaconEmpty & msg)
{
	if (actual_action == IDLE || actual_action == WANDER) {
		if (SBB::readInt(msg.name) != beacon_controller::INACTIVE_TAKEN) {
			if (msg.pos.z > zmin && msg.pos.z < zmax) {
				beacon_to_go = msg.pos;
				beacon_to_go_name = msg.name;
				actual_action = CREATE_BEACON;
				SBB::postInt(msg.name, beacon_controller::INACTIVE_TAKEN);	//disable beacon for other bots
				ChangeState("seekWB");
			}
		}
	}
}

void ai_scientific::onEmptyWB(const TMsgWBEmpty & msg)
{
}

void ai_scientific::onTakenBeacon(const TMsgBeaconTakenByPlayer & msg)
{
	if (msg.name == beacon_to_go_name) {
		waiting_time = 0.0f;
		beacon_to_go_name = "";
		actual_action = IDLE;
		ChangeState("lookForObj");
	}
}

void ai_scientific::onTakenWB(const TMsgWBTakenByPlayer & msg)
{
}

//clean all objects with actions related with NPC
void ai_scientific::CleanStates() {
	if (beacon_to_go_name != "") {
		if (SBB::readInt(beacon_to_go_name) == beacon_controller::INACTIVE_TAKEN)
			SBB::postInt(beacon_to_go_name, beacon_controller::INACTIVE);

		if (SBB::readInt(beacon_to_go_name) == beacon_controller::TO_REMOVE_TAKEN) {
			SBB::postInt(beacon_to_go_name, beacon_controller::TO_REMOVE);
		}
		beacon_to_go_name = "";
	}

	if (wb_to_go_name != "") {
		if (SBB::readInt(wb_to_go_name) == workbench_controller::INACTIVE_TAKEN)
			SBB::postInt(wb_to_go_name, workbench_controller::INACTIVE);

		if (SBB::readInt(wb_to_go_name) == workbench_controller::BUSY)
			SBB::postInt(wb_to_go_name, workbench_controller::INACTIVE);

		wb_to_go_name = "";
	}
}

void ai_scientific::onStaticBomb(const TMsgStaticBomb & msg)
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	float d = squaredDist(msg.pos, curr_pos);

	if (d < msg.r) {
		CleanStates();
		ChangeState(ST_STUNT);
	}
}

void ai_scientific::renderInMenu()
{
	ImGui::Text("Node: %s", out[actual_action].c_str());
	ImGui::Text("Timer: %.4f", waiting_time);
	ImGui::Text("Objective: %s", beacon_to_go_name.c_str());
}

void ai_scientific::SetHandleMeInit()
{
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
}

void ai_scientific::SetMyEntity() {
	myEntity = myParent;
}

const void ai_scientific::StuntState() {
	actionStunt();
	CleanStates();
	____TIMER_CHECK_DO_(timeStunt);
	stunned = false;
	ChangeState(ST_STUNT_END);
	____TIMER_CHECK_DONE_(timeStunt);
}

CEntity* ai_scientific::getMyEntity() {
	CHandle me = CHandle(this);
	return me.getOwner();
}

#define KPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "kpt%d_%s", index, nameSufix);

bool ai_scientific::load(MKeyValue& atts) {
	int n = atts.getInt("kpt_size", 0);
	keyPoints.resize(n);
	for ( int i = 0; i < n; i++) {
		KPT_ATR_NAME(atrType, "type", i);
		KPT_ATR_NAME(atrPos, "pos", i);
		KPT_ATR_NAME(atrWait, "wait", i);
		keyPoints[i] = KeyPoint(
			kptTypes[atts.getString(atrType, "seek")]
			, atts.getPoint(atrPos)
			, atts.getFloat(atrWait, 0.0f)
			);
	}

	zmin = atts.getFloat("zmin",0.0f);
	zmax = atts.getFloat("zmax",0.0f);

	return true;
}