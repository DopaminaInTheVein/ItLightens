#include "mcv_platform.h"
#include "ai_beacon.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"

#include "components\entity_tags.h"

int beacon_controller::id_curr_max_beacons = 0;

map<string, statehandler> beacon_controller::statemap = {};

map<int, string> beacon_controller::out = {};

void beacon_controller::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("beacon")) {
			map<std::string, float> fields = readIniFileAttrMap("ai_beacon");

			assignValueToVar(range, fields);
			assignValueToVar(rot_speed_sonar, fields);
			assignValueToVar(rot_speed_disable, fields);
			assignValueToVar(t_waiting, fields);
			assignValueToVar(t_max_sonar, fields);
			assignValueToVar(t_max_empty, fields);
			assignValueToVar(t_max_disable, fields);

		}
	}
}

void beacon_controller::Init() {

	//read main attributes from file
	readIniFileAttr();

	om = getHandleManager<beacon_controller>();	//list handle beacon in game

	id_beacon = ++beacon_controller::id_curr_max_beacons;
	full_name = "beacon_" + to_string(id_beacon);

	if (statemap.empty()) {
		AddState("idle", (statehandler)&beacon_controller::Idle);
		AddState("inactive", (statehandler)&beacon_controller::Inactive);
		AddState("activeNothing", (statehandler)&beacon_controller::ActiveNothing);
		AddState("activeSonar", (statehandler)&beacon_controller::ActiveSonar);
		AddState("waitToRemove", (statehandler)&beacon_controller::WaitToRemoveSonar);
		AddState("waitToremoveNothing", (statehandler)&beacon_controller::WaitToRemoveNothing);
		AddState("waitInactive", (statehandler)&beacon_controller::WaitInactive);
	}

	SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?

	out[INACTIVE] = "INACTIVE";
	out[INACTIVE_TAKEN] = "INACTIVE_TAKEN";
	out[SONAR] = "SONAR";
	out[BUSY] = "BUSY";
	out[TO_REMOVE] = "TO_REMOVE";
	out[TO_REMOVE_TAKEN] = "TO_REMOVE_TAKEN";

	SBB::postInt(full_name, INACTIVE);		//init my state on the board

											//Init mesages data
	msg_remove.pos_beacon = VEC3(0, 0, 0);	//has to be updated on frame if can be moved
	msg_remove.name_beacon = full_name;

	msg_empty.pos = VEC3(0, 0, 0);	//has to be updated on frame if can be moved
	msg_empty.name = full_name;

	ChangeState("idle");
}

void beacon_controller::Idle()
{
	//Nothing to do
	//if (GetAsyncKeyState('Q') != 0)
	ChangeState("waitInactive");
}

void beacon_controller::WaitInactive()
{
	t_waiting += getDeltaTime();
	if (t_waiting >= t_max_empty) {
		t_waiting = 0;
		ChangeState("inactive");
	}
}

void beacon_controller::Inactive()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();
	if (SBB::readInt(full_name) == INACTIVE) SendMessageEmpty();
	//nothing to do, check sbb. Should go system of events
	if (SBB::readInt(full_name) != INACTIVE && SBB::readInt(full_name) != INACTIVE_TAKEN) {
		switch (SBB::readInt(full_name)) {
		case BUSY:
			ChangeState("activeNothing");
			break;
		case SONAR:
			ChangeState("activeSonar");
			break;
		default:
			//nothing to do
			break;
		}
	}
}

void beacon_controller::ActiveSonar()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();

	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);
	me_transform->setAngles(yaw + rot_speed_sonar*getDeltaTime(), pitch);
	Debug->DrawLine(me_transform->getPosition() + VEC3(0, 1, 0), me_transform->getFront(), range, RED);
	//TMsgNoise

	VHandles hs = tags_manager.getHandlesByTag(getID("player"));

	CEntity * player = hs[hs.size() - 1];
	TCompTransform * player_transform = player->get<TCompTransform>();
	VEC3 posPlayer = player_transform->getPosition();
	VEC3 myPos = me_transform->getPosition();
	if (squaredDistY(posPlayer, myPos) < squaredDistXZ(posPlayer, myPos) * 2) { //Pitch < 30
		if (me_transform->isHalfConeVision(posPlayer, deg2rad(15.0f))) { //Cono vision
			if (squaredDist(myPos, posPlayer) < 3.0f) { //Distancia
				TMsgNoise msg;
				msg.source = posPlayer;
				for (CHandle guardHandle : tags_manager.getHandlesByTag(getID("AI_guard"))) {
					CEntity * ePoss = guardHandle;
					ePoss->sendMsg(msg);
				}
			}
		}
	}

	t_waiting += getDeltaTime();
	if (t_waiting > t_max_sonar) {		//go to new action
		t_waiting = 0.0f;

		VEC3 curr_pos = me_transform->getPosition();
		SBB::postInt(full_name, TO_REMOVE);
		SendMessageRemove();
		ChangeState("waitToRemove");		//TODO: separate from waitToRemove from activeNothing when detection
	}
}

void beacon_controller::ActiveNothing()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();

	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);
	me_transform->setAngles(yaw + rot_speed_disable*getDeltaTime(), pitch);
	Debug->DrawLine(me_transform->getPosition() + VEC3(0, 1, 0), me_transform->getFront(), range, BLUE);

	t_waiting += getDeltaTime();
	if (t_waiting > t_max_disable) {		//go to new action
		t_waiting = 0.0f;

		VEC3 curr_pos = me_transform->getPosition();
		SBB::postInt(full_name, TO_REMOVE);
		SendMessageRemove();
		ChangeState("waitToremoveNothing");
	}
}

void beacon_controller::WaitToRemoveSonar()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	//TODO: difference with active_disable, animation from active_sonar only right now
	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);
	me_transform->setAngles(yaw + rot_speed_sonar*getDeltaTime(), pitch);
	Debug->DrawLine(me_transform->getPosition() + VEC3(0, 1, 0), me_transform->getFront(), range, RED);

	VHandles hs = tags_manager.getHandlesByTag(getID("player"));

	CEntity * player = hs[hs.size() - 1];
	TCompTransform * player_transform = player->get<TCompTransform>();
	VEC3 posPlayer = player_transform->getPosition();
	VEC3 myPos = me_transform->getPosition();
	if (squaredDistY(posPlayer, myPos) < squaredDistXZ(posPlayer, myPos) * 2) { //Pitch < 30
		if (me_transform->isHalfConeVision(posPlayer, deg2rad(15.0f))) { //Cono vision
			if (squaredDist(myPos, posPlayer) < 3.0f) { //Distancia
				TMsgNoise msg;
				msg.source = posPlayer;
				for (CHandle guardHandle : tags_manager.getHandlesByTag(getID("AI_guard"))) {
					CEntity * ePoss = guardHandle;
					ePoss->sendMsg(msg);
				}
			}
		}
	}

	if(SBB::readInt(full_name) == TO_REMOVE)SendMessageRemove();

	//nothing to do, check sbb. Should go system of events
	if (SBB::readInt(full_name) != TO_REMOVE && SBB::readInt(full_name) != TO_REMOVE_TAKEN) {
		ChangeState("waitInactive");
	}
}

void beacon_controller::WaitToRemoveNothing()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	//TODO: difference with active_disable, animation from active_sonar only right now
	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);
	me_transform->setAngles(yaw + rot_speed_disable*getDeltaTime(), pitch);
	Debug->DrawLine(me_transform->getPosition() + VEC3(0, 1, 0), me_transform->getFront(), range, BLUE);
	
	if(SBB::readInt(full_name) == TO_REMOVE)SendMessageRemove();	

	//nothing to do, check sbb. Should go system of events
	if (SBB::readInt(full_name) != TO_REMOVE && SBB::readInt(full_name) != TO_REMOVE_TAKEN) {
		ChangeState("waitInactive");
	}
}

void beacon_controller::onPlayerAction(TMsgBeaconBusy & msg)
{
	//dbg("%s :\n", full_name.c_str());
	if (SBB::readInt(full_name) != SONAR) {
		SetMyEntity();
		TCompTransform *me_transform = myEntity->get<TCompTransform>();
		VEC3 curr_pos = me_transform->getPosition();

		float d = simpleDistXZ(curr_pos, msg.pos);
		//dbg("%s - %f\n",full_name.c_str(),d);
		if (d <= 5) {
			*msg.reply = true;
			SBB::postInt(full_name, BUSY);
			ChangeState("activeNothing");
			SendMessageTaken();
		}
	}
}

void beacon_controller::renderInMenu()
{
	ImGui::Text("Node : %s", out[SBB::readInt(full_name)].c_str());
	ImGui::Text("timer : %.4f", t_waiting);
}

void beacon_controller::SetHandleMeInit()
{
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
}

void beacon_controller::SetMyEntity() {
	myEntity = myParent;
}

void beacon_controller::SendMessageEmpty() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	msg_empty.pos = curr_pos;

	VHandles hs = tags_manager.getHandlesByTag(getID("AI_cientifico"));
	for (CEntity *e : hs)
		e->sendMsg(msg_empty);
}

void beacon_controller::SendMessageRemove() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	msg_remove.pos_beacon = curr_pos;

	VHandles hs = tags_manager.getHandlesByTag(getID("AI_cientifico"));
	for (CEntity *e : hs)
		e->sendMsg(msg_remove);
}

void beacon_controller::SendMessageTaken() {
	msg_taken.name = full_name;

	VHandles hs = tags_manager.getHandlesByTag(getID("AI_cientifico"));
	for (CEntity *e : hs)
		e->sendMsg(msg_taken);
}