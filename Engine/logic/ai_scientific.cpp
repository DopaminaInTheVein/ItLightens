#include "mcv_platform.h"
#include "ai_scientific.h"
#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"

#include "ai_beacon.h"

void ai_scientific::Init()
{
	om = getHandleManager<ai_scientific>();	//list handle scientific in game

	//list states
	AddState("idle", (statehandler)&ai_scientific::Idle);
	AddState("seekWB", (statehandler)&ai_scientific::SeekWorkbench);
	AddState("moveToPos", (statehandler)&ai_scientific::MoveToPos);
	AddState("aimToPos", (statehandler)&ai_scientific::AimToPos);
	AddState("createBeacon", (statehandler)&ai_scientific::CreateBeaconFromWB);
	AddState("addBeacon", (statehandler)&ai_scientific::AddBeacon);
	AddState("removeBeacon", (statehandler)&ai_scientific::RemoveBeacon);
	AddState("lookForObj", (statehandler)&ai_scientific::LookForObj);

	out[IDLE] = "IDLE";
	out[CREATE_BEACON] = "CREATE_BEACON";
	out[ADD_BEACON] = "ADD_BEACON";
	out[REMOVE_BEACON] = "REMOVE_BEACON";
	out[WANDER] = "WANDER";

	//TODO: search positions from WorkBench components in game
	wbs.resize(2);
	wbs[0] = VEC3(10.0f, 0.0f, 10.0f);
	wbs[1] = VEC3(-10.0f, 0.0f, -10.0f);

	SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?

	ChangeState("idle"); //init Node
}

void ai_scientific::Idle() {
	//Nothing to do
	if (GetAsyncKeyState('Q') != 0) {
		ChangeState("lookForObj");
	}
}

void ai_scientific::LookForObj()
{
	VEC3 pos_wander = VEC3(rand() % 20 - 10, 0.0f, rand() % 20 - 10);
	actual_action = WANDER;
	obj_position = pos_wander;
	ChangeState("aimToPos");
}

void ai_scientific::SeekWorkbench()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	//look for closest wb
	float min_d = 10000.0f;
	float square_d = 0.0f;
	for (auto &wb : wbs) {
		square_d = simpleDistXZ(curr_pos, wb);
		if (square_d < min_d) {
			obj_position = wb;
			min_d = square_d;
		}
	}

	ChangeState("aimToPos");
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
		ChangeState("moveToPos");
	}
}

void ai_scientific::MoveToPos()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	VEC3 target = obj_position;
	float delta_time = getDeltaTime();

	VEC3 new_pos = curr_pos;
	new_pos.x += delta_time*move_speed*me_transform->getFront().x;
	new_pos.y += delta_time*move_speed*me_transform->getFront().y;
	new_pos.z += delta_time*move_speed*me_transform->getFront().z;

	me_transform->setPosition(new_pos);

	float dist_square = simpleDistXZ(new_pos, target);

	if (dist_square < d_epsilon) {		//TODO: System to read action to do: add beacon, remove beacon, create beacon from WB
		if (actual_action == CREATE_BEACON)
			ChangeState("createBeacon");
		else if (actual_action == ADD_BEACON)
			ChangeState("addBeacon");
		else if (actual_action == REMOVE_BEACON)
			ChangeState("removeBeacon");
		else if (actual_action == WANDER)
			ChangeState("lookForObj");
	}
}

void ai_scientific::CreateBeaconFromWB()
{
	waiting_time += getDeltaTime();
	if (waiting_time > t_createBeacon) {		//go to beacon
		obj_position = beacon_to_go;
		actual_action = ADD_BEACON;
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
			SBB::postInt(msg.name_beacon, beacon_controller::TO_REMOVE_TAKEN);
			obj_position = beacon_to_go = msg.pos_beacon;
			beacon_to_go_name = msg.name_beacon;
			actual_action = REMOVE_BEACON;
			ChangeState("aimToPos");
		}
	}
}

void ai_scientific::onEmptyBeacon(const TMsgBeaconEmpty & msg)
{
	if (actual_action == IDLE || actual_action == WANDER) {
		if (SBB::readInt(msg.name_beacon) != beacon_controller::INACTIVE_TAKEN) {
			beacon_to_go = msg.pos_beacon;
			beacon_to_go_name = msg.name_beacon;
			actual_action = CREATE_BEACON;
			SBB::postInt(msg.name_beacon, beacon_controller::INACTIVE_TAKEN);	//disable beacon for other bots
			ChangeState("seekWB");
		}
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