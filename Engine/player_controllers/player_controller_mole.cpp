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

		AddState("grabBox", (statehandler)&player_controller_mole::GrabBox);
		AddState("leaveBox", (statehandler)&player_controller_mole::LeaveBox);
		AddState("destroyWall", (statehandler)&player_controller_mole::DestroyWall);
	}

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
	SET_ANIM_MOLE(AST_IDLE);
}

bool player_controller_mole::getUpdateInfo()
{
	if (!CPlayerBase::getUpdateInfo()) return false;
	animController = GETH_MY(SkelControllerMole);
	return true;
}

void player_controller_mole::UpdateInputActions() {
	//if (state == "moving")
	//	ChangePose(pose_run_route);
	//else if (state == "jumping" || state == "falling")
	//	ChangePose(pose_jump_route);
	//else if (state == "idle")
	//	ChangePose(pose_idle_route);

	if (io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed()) {
		if (boxGrabbed) {
			//ChangePose(pose_idle_route);
			logic_manager->throwEvent(logic_manager->OnLeaveBox, "");
			ChangeState("leaveBox");
		}
		else {
			if (this->nearToBox()) {
				//ChangePose(pose_box_route);
				logic_manager->throwEvent(logic_manager->OnPickupBox, "");
				ChangeState("grabBox");
			}
			else if (this->nearToWall()) {
				//ChangePose(pose_wall_route);
				logic_manager->throwEvent(logic_manager->OnBreakWall, "");
				ChangeState("destroyWall");
			}
		}
	}
}

void player_controller_mole::UpdateMovingWithOther() {
	if (boxGrabbed) {
		//ChangePose(pose_box_route);
		CEntity* box = SBB::readHandlesVector("wptsBoxes")[selectedBoxi];
		TCompTransform* box_t = box->get<TCompTransform>();
		TCompPhysics* box_p = box->get<TCompPhysics>();

		CEntity* p = myParent;
		TCompTransform* p_t = p->get<TCompTransform>();
		VEC3 posPlayer = p_t->getPosition();
		posPlayer.y += 4;
		box_p->setPosition(posPlayer, box_t->getRotation());
	}
}

void player_controller_mole::UpdateUnpossess() {
	CHandle h = CHandle(this);
	tags_manager.removeTag(h.getOwner(), getID("player"));

	if (boxGrabbed) {
		LeaveBox();
	}
}

void player_controller_mole::GrabBox() {
	if (SBB::readBool(selectedBox)) {
		bt_mole * mole = SBB::readMole(selectedBox);
		//mole->ChangeState("idle");
	}
	else {
		SBB::postBool(selectedBox, true);
	}
	CEntity* box = SBB::readHandlesVector("wptsBoxes")[selectedBoxi];
	TCompTransform* box_t = box->get<TCompTransform>();
	TCompPhysics* box_p = box->get<TCompPhysics>();

	CEntity* p = myParent;
	TCompTransform* p_t = p->get<TCompTransform>();
	VEC3 posPlayer = p_t->getPosition();
	posPlayer.y += 4;

	box_p->setKinematic(true);
	box_p->setPosition(posPlayer, box_t->getRotation());

	energyDecreasal(5.0f);
	TMsgDamage dmg;
	dmg.modif = 0.5f;
	myEntity->sendMsg(dmg);
	boxGrabbed = true;
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
	CEntity* box = SBB::readHandlesVector("wptsBoxes")[selectedBoxi];
	TCompTransform* box_t = box->get<TCompTransform>();
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

	TCompPhysics *box_p = box->get<TCompPhysics>();
	box_p->setKinematic(false);
	box_p->setPosition(posbox, box_t->getRotation());

	SBB::postBool(selectedBox, false);
	boxGrabbed = false;
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
	if (SBB::readHandlesVector("wptsBoxes").size() > 0) {
		float distMax = 2.0f;
		float higher = -999.9f;
		string key_final = "";
		for (int i = 0; i < SBB::readHandlesVector("wptsBoxes").size(); i++) {
			CEntity * entTransform = this->getEntityBoxPointer(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
			string key = nameBox->name;
			if (disttowpt < distMax + 2 && wpt.y >= higher) {
				distMax = disttowpt;
				higher = wpt.y;
				selectedBox = key;
				selectedBoxi = i;
				found = true;
			}
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

//Cambio de malla
//void player_controller_mole::ChangePose(string new_pose_route) {
//	mesh->unregisterFromRender();
//	MKeyValue atts_mesh;
//	atts_mesh["name"] = new_pose_route;
//	mesh->load(atts_mesh);
//	mesh->registerToRender();
//}