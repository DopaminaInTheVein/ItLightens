#include "mcv_platform.h"
#include "ai_workbench.h"


#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"

#include "components\entity_tags.h"

int workbench_controller::id_curr_max_wb = 0;

map<string, statehandler> workbench_controller::statemap = {};
map<int, string> workbench_controller::out = {};

void workbench_controller::Init() {
	om = getHandleManager<workbench_controller>();	//list handle beacon in game

	id_wb = ++workbench_controller::id_curr_max_wb;
	full_name = "workbench_" + to_string(id_wb);

	if (statemap.empty()) {
		AddState("idle", (statehandler)&workbench_controller::Idle);
		AddState("inactive", (statehandler)&workbench_controller::Inactive);
		AddState("busy", (statehandler)&workbench_controller::Busy);
	}

	SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?

	out[INACTIVE] = "INACTIVE";
	out[INACTIVE_TAKEN] = "INACTIVE_TAKEN";
	out[BUSY] = "BUSY";

	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	SBB::postInt(full_name, INACTIVE);		//init my state on the board
	SBB::postVEC3(full_name, curr_pos);

	//Init mesages data
	msg_empty.pos = curr_pos;
	msg_empty.name = full_name;
	msg_taken.name = full_name;

	ChangeState("idle");
}

void workbench_controller::Idle()
{
	//Nothing to do
	//if (GetAsyncKeyState('Q') != 0)
	ChangeState("inactive");
}

void workbench_controller::Inactive()
{

	if (SBB::readInt(full_name) == INACTIVE) SendMessageEmpty();
	//nothing to do, check sbb. Should go system of events
	if (SBB::readInt(full_name) == BUSY) {
		ChangeState("busy");
	}
}

void workbench_controller::Busy()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	//nothing to do, check sbb. Should go system of events
	if (SBB::readInt(full_name) == INACTIVE) {
		SendMessageEmpty();
		ChangeState("inactive");
	}
}


void workbench_controller::renderInMenu()
{
	ImGui::Text("Node : %s", out[SBB::readInt(full_name)].c_str());
}

void workbench_controller::SetHandleMeInit()
{
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
}

void workbench_controller::SetMyEntity() {
	myEntity = myParent;
}

void workbench_controller::SendMessageEmpty() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	msg_empty.pos = curr_pos;

	VHandles hs = tags_manager.getHandlesByTag(getID("AI_cientifico"));
	for (CEntity *e : hs)
		e->sendMsg(msg_empty);
}

void workbench_controller::SendMessageTaken() {

	VHandles hs = tags_manager.getHandlesByTag(getID("AI_cientifico"));
	for (CEntity *e : hs)
		e->sendMsg(msg_taken);
}