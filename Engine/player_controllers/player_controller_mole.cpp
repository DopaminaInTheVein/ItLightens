#include "mcv_platform.h"
#include "player_controller_mole.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"
#include "app_modules\io\io.h"
#include "app_modules\logic_manager\logic_manager.h"
#include "components\comp_msgs.h"

#include "ui\ui_interface.h"
#include "components/comp_charactercontroller.h"
#include "components\comp_physics.h"

map<string, statehandler> player_controller_mole::statemap = {};

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_MOLE(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_MOLE_P(state) SET_ANIM_STATE_P(animController, state)

//State names
#define ST_MOLE_GRAB "grabbedBox"
#define ST_MOLE_GRAB_TURN "turnToGrab"
#define ST_MOLE_GRABBING_1 "grabbing1"
#define ST_MOLE_GRABBING_2 "grabbing2"
#define ST_MOLE_UNGRAB "leaveBox"
#define ST_MOLE_DESTROY "destroyWall"

void player_controller_mole::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_mole")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields_base = readIniAtrData(file_ini, "controller_base");

			assignValueToVar(player_max_speed, fields_base);
			assignValueToVar(player_rotation_speed, fields_base);
			assignValueToVar(jimpulse, fields_base);
			assignValueToVar(left_stick_sensibility, fields_base);
			assignValueToVar(camera_max_height, fields_base);
			assignValueToVar(camera_min_height, fields_base);

			map<std::string, float> fields_mole = readIniAtrData(file_ini, "controller_mole");

			assignValueToVar(grab_box_energy, fields_mole);
			assignValueToVar(destroy_wall_energy, fields_mole);
		}
	}
}

#define AddStMole(name, func) AddState(name, (statehandler)&player_controller_mole::func)
void player_controller_mole::Init() {
	getUpdateInfoBase(CHandle(this).getOwner());

	// read main attributes from file
	readIniFileAttr();
	mole_max_speed = player_max_speed;

	om = getHandleManager<player_controller_mole>();	//player

	if (statemap.empty()) {
		//States from controller base and poss controller
		addBasicStates();
		addPossStates();
		AddStMole(ST_MOLE_GRAB_TURN, TurnToGrab);
		AddStMole(ST_MOLE_GRABBING_1, GrabbingBox1);
		AddStMole(ST_MOLE_GRABBING_2, GrabbingBox2);
		AddStMole(ST_MOLE_GRAB, GrabbedBox);
		AddStMole(ST_MOLE_UNGRAB, LeaveBox);
		AddStMole(ST_MOLE_DESTROY, DestroyWall);
	}

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
	SET_ANIM_MOLE(AST_IDLE);
}

bool player_controller_mole::getUpdateInfo()
{
	if (!CPlayerBase::getUpdateInfo()) return false;
	myParent = CHandle(this).getOwner();
	animController = GETH_MY(SkelControllerMole);
	return true;
}

void player_controller_mole::myUpdate()
{
	Debug->DrawLine(transform->getPosition(), transform->getFront(), 1.f);
}

void player_controller_mole::UpdateInputActions() {
	if (io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed()) {
		if (boxGrabbed.isValid()) {
			logic_manager->throwEvent(logic_manager->OnLeaveBox, "");
			ChangeState(ST_MOLE_UNGRAB);
		}
		else {
			if (this->nearToBox()) {
				//ChangePose(pose_box_route);
				logic_manager->throwEvent(logic_manager->OnPickupBox, "");
				ChangeState(ST_MOLE_GRAB_TURN);
			}
			else if (this->nearToWall()) {
				//ChangePose(pose_wall_route);
				logic_manager->throwEvent(logic_manager->OnBreakWall, "");
				ChangeState(ST_MOLE_DESTROY);
			}
		}
	}
}

void player_controller_mole::UpdateMovingWithOther() {
	if (boxGrabbed.isValid()) {
		GET_COMP(box_t, boxGrabbed, TCompTransform);
		GET_COMP(box_p, boxGrabbed, TCompPhysics);

		float yawPlayer = transform->getYaw();
		VEC3 posPlayer = transform->getPosition();
		VEC3 posBox = posPlayer + transform->getFront() * grabOffset.dist;
		posBox.y += grabOffset.y;
		box_t->setYaw(yawPlayer + grabOffset.yaw);
		box_p->setPosition(posBox, box_t->getRotation());
	}
}

void player_controller_mole::UpdateUnpossess() {
	CHandle h = CHandle(this);
	tags_manager.removeTag(h.getOwner(), getID("player"));

	if (boxGrabbed.isValid()) {
		LeaveBox();
	}
}

void player_controller_mole::GrabbedBox() {
	if (!SBB::readBool(selectedBox)) {
		SBB::postBool(selectedBox, true);
	}

	GET_COMP(box_t, boxNear, TCompTransform);
	GET_COMP(box_p, boxNear, TCompPhysics);

	VEC3 posPlayer = transform->getPosition();
	VEC3 posBox = box_t->getPosition();
	posBox.y += 1.f;
	box_p->setKinematic(true);
	box_p->setPosition(posBox, box_t->getRotation());
	grabOffset.dist = realDistXZ(posPlayer, posBox);
	grabOffset.y = posBox.y - posPlayer.y;
	grabOffset.yaw = transform->getDeltaYawToAimTo(posBox);
	box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, true);

	energyDecreasal(5.0f);
	TMsgDamage dmg;
	dmg.modif = 0.5f;
	myEntity->sendMsg(dmg);
	boxGrabbed = boxNear;
	mole_max_speed /= 2;
	//ChangePose(pose_idle_route);
	ChangeState("idle");
}

void player_controller_mole::DestroyWall() {
	energyDecreasal(10.0f);
	vector<CHandle> handles = SBB::readHandlesVector("wptsBreakableWall");
	handles.erase(handles.begin() + selectedWallToBreaki);
	getHandleManager<CEntity>()->destroyHandle(getEntityWallHandle(selectedWallToBreaki));
	SBB::postHandlesVector("wptsBreakableWall", handles);
	//ChangePose(pose_idle_route);
	ChangeState("idle");
}

void player_controller_mole::LeaveBox() {
	GET_COMP(box_t, boxGrabbed, TCompTransform);
	VEC3 posboxIni = box_t->getPosition();
	CEntity* p = myParent;
	TCompTransform* p_t = p->get<TCompTransform>();
	VEC3 posbox;
	posbox.x = posboxIni.x + p_t->getFront().x * 3;
	posbox.y = posboxIni.y - 2;
	posbox.z = posboxIni.z + p_t->getFront().z * 3;
	float angle = 0.0f;
	//TODO PHYSX OBJECT
	/*while (!box_t->setPosition(posbox)) {
		angle += 0.1;
		posbox.x = posboxIni.x + p_t->getFront().x * sin(angle) * 3;
		posbox.z = posboxIni.z + p_t->getFront().z * cos(angle) * 3;
	}*/
	GET_COMP(box_p, boxGrabbed, TCompPhysics);
	box_p->setKinematic(false);
	box_p->setPosition(posbox, box_t->getRotation());
	box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);

	SBB::postBool(selectedBox, false);
	boxGrabbed = CHandle();
	mole_max_speed *= 2;
	//ChangePose(pose_idle_route);
	ChangeState("idle");
}

bool player_controller_mole::nearToWall() {
	bool found = false;
	if (SBB::readHandlesVector("wptsBreakableWall").size() > 0) {
		float distMaxx = 6.0f;
		float distMaxz = 10.5f;
		for (int i = 0; !found && i < SBB::readHandlesVector("wptsBreakableWall").size(); i++) {
			CEntity * entTransform = this->getEntityWallHandle(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowptx = fabsf(fabsf(wpt.x) - fabsf(getEntityTransform()->getPosition().x));
			float disttowptz = fabsf(fabsf(wpt.z) - fabsf(getEntityTransform()->getPosition().z));
			if (disttowptx < distMaxx && disttowptz < distMaxz) {
				distMaxx = disttowptx;
				distMaxz = disttowptz;
				selectedWallToBreaki = i;
				found = true;
			}
		}
	}
	return found;
}
bool player_controller_mole::nearToBox() {
	bool found = false;
	float distMax = 2.0f;
	float higher = -999.9f;
	string key_final = "";
	for (auto h : TCompBox::all_boxes) {
		if (!h.isValid()) continue;
		GET_COMP(tBox, h, TCompTransform);
		string key = ((CEntity*)h)->getName();
		VEC3 wpt = tBox->getPosition();
		float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
		if (disttowpt < distMax + 2 && wpt.y >= higher) {
			distMax = disttowpt;
			higher = wpt.y;
			boxNear = h;
			selectedBox = key;
			found = true;
		}
	}
	return found;
}

void player_controller_mole::InitControlState() {
	CHandle h = CHandle(this);
	tags_manager.addTag(h.getOwner(), getID("player"));
	ChangeState("idle");
}

void player_controller_mole::update_msgs()
{
	ui.addTextInstructions("Left Shift            -> Exit possession State");
	ui.addTextInstructions("Click Left Mouse      -> Grab/Throw near Box or Break Wall");
}

void player_controller_mole::TurnToGrab()
{
	bool faced = turnTo(GETH_COMP(boxNear, TCompTransform));
	if (faced) {
		ChangeState(ST_MOLE_GRABBING_1);
	}
}

void player_controller_mole::GrabbingBox1()
{
	//Player bring hands to the box (static yet)
	//IK stuff

	//Provisional
	static float t_grab1 = 1.f;
	t_grab1 -= getDeltaTime();
	if (t_grab1 < 0) {
		t_grab1 = 1.f;
		ChangeState(ST_MOLE_GRABBING_2);
	}
}

void player_controller_mole::GrabbingBox2()
{
	//Player return to idlebox pose
	//End IK Stuff progressively
	//Box track player hands

	//Provisional
	static float t_grab2 = 1.5f;
	t_grab2 -= getDeltaTime();
	if (t_grab2 < 0) {
		t_grab2 = 1.5f;
		ChangeState(ST_MOLE_GRAB);
	}
}

void player_controller_mole::ChangeCommonState(std::string st)
{
	if (st == "moving") {
		SET_ANIM_MOLE(AST_MOVE);
	}
	else if (st == "running") {
		SET_ANIM_MOLE(AST_RUN);
	}
	else if (st == "jumping") {
		SET_ANIM_MOLE(AST_JUMP);
	}
	else if (st == "idle") {
		SET_ANIM_MOLE(AST_IDLE);
	}
}