#include "mcv_platform.h"
#include "player_controller_mole.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_name.h"
#include "app_modules\io\io.h"
#include "components\entity_tags.h"

#include "ui\ui_interface.h"
#include "logic\ai_mole.h"

void player_controller_mole::Init() {
	om = getHandleManager<player_controller_mole>();	//player

	AddState("grabBox", (statehandler)&player_controller_mole::GrabBox);
	AddState("leaveBox", (statehandler)&player_controller_mole::LeaveBox);
	AddState("destroyWall", (statehandler)&player_controller_mole::DestroyWall);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	//Mallas
	pose_run = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_jump = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_box = getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_wall = getHandleManager<TCompRenderStaticMesh>()->createHandle();

	CEntity* myEntity = myParent;
	pose_idle = myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	actual_render = pose_run;

	pose_idle.setOwner(myEntity);
	pose_run.setOwner(myEntity);
	pose_jump.setOwner(myEntity);
	pose_box.setOwner(myEntity);
	pose_wall.setOwner(myEntity);

	TCompRenderStaticMesh *mesh;

	mesh = pose_idle;
	mesh->static_mesh = Resources.get("static_meshes/mole.static_mesh")->as<CStaticMesh>();

	mesh = pose_jump;
	mesh->static_mesh = Resources.get("static_meshes/mole_jump.static_mesh")->as<CStaticMesh>();

	mesh = pose_run;
	mesh->static_mesh = Resources.get("static_meshes/mole_run.static_mesh")->as<CStaticMesh>();

	mesh = pose_box;
	mesh->static_mesh = Resources.get("static_meshes/mole_box.static_mesh")->as<CStaticMesh>();

	mesh = pose_wall;
	mesh->static_mesh = Resources.get("static_meshes/mole_wall.static_mesh")->as<CStaticMesh>();

	actual_render->registerToRender();

	ChangeState("idle");
}
void player_controller_mole::UpdateInputActions() {
	energyDecreasal(getDeltaTime()*0.5f);
	if (state == "moving")
		ChangePose(pose_run);
	else if (state == "jumping")
		ChangePose(pose_jump);

	if (io->mouse.left.becomesReleased() || io->joystick.button_X.becomesReleased()) {
		if (boxGrabbed) {
			ChangePose(pose_idle);
			ChangeState("leaveBox");
		}
		else {
			if (this->nearToBox()) {
				ChangePose(pose_box);
				ChangeState("grabBox");
			}
			else if (this->nearToWall()) {
				ChangePose(pose_wall);
				ChangeState("destroyWall");
			}
		}
	}
}

void player_controller_mole::UpdateMovingWithOther() {
	if (boxGrabbed) {
		ChangePose(pose_box);
		energyDecreasal(getDeltaTime()*0.5f);
		CEntity* box = SBB::readHandlesVector("wptsBoxes")[selectedBoxi];
		TCompTransform* box_t = box->get<TCompTransform>();
		SetMyEntity();
		CEntity* p = myParent;
		TCompTransform* p_t = p->get<TCompTransform>();
		VEC3 posPlayer = p_t->getPosition();
		posPlayer.y += 2;
		box_t->setPosition(posPlayer);
	}
}

void player_controller_mole::UpdateUnpossess() {
	if (boxGrabbed) {
		ChangePose(pose_idle);
		LeaveBox();
	}
}

void player_controller_mole::GrabBox() {
	if (SBB::readBool(selectedBox)) {
		ai_mole * mole = SBB::readMole(selectedBox);
		ChangePose(pose_idle);
		mole->ChangeState("idle");
	}
	else {
		SBB::postBool(selectedBox, true);
	}
	CEntity* box = SBB::readHandlesVector("wptsBoxes")[selectedBoxi];
	TCompTransform* box_t = box->get<TCompTransform>();
	SetMyEntity();
	CEntity* p = myParent;
	TCompTransform* p_t = p->get<TCompTransform>();
	VEC3 posPlayer = p_t->getPosition();
	posPlayer.y += 2;
	box_t->setPosition(posPlayer);

	energyDecreasal(5.0f);
	boxGrabbed = true;
	player_max_speed /= 2;
	ChangePose(pose_idle);
	ChangeState("idle");
}

void player_controller_mole::DestroyWall() {
	energyDecreasal(10.0f);
	vector<CHandle> handles = SBB::readHandlesVector("wptsBreakableWall");
	handles.erase(handles.begin() + selectedWallToBreaki);
	getHandleManager<CEntity>()->destroyHandle(getEntityWallHandle(selectedWallToBreaki));
	SBB::postHandlesVector("wptsBreakableWall", handles);
	ChangePose(pose_idle);
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
	while (!box_t->executeMovement(posbox)) {
		angle += 0.1;
		posbox.x = posboxIni.x + p_t->getFront().x * sin(angle) * 3;
		posbox.z = posboxIni.z + p_t->getFront().z * cos(angle) * 3;
	}
	SBB::postBool(selectedBox, false);
	boxGrabbed = false;
	player_max_speed *= 2;
	ChangePose(pose_idle);
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
	ChangeState("idle");
}

void player_controller_mole::update_msgs()
{
	ui.addTextInstructions("Left Shift            -> Exit possession State");
	ui.addTextInstructions("Click Left Mouse      -> Grab/Throw near Box or Break Wall");
}

//Cambio de malla
void player_controller_mole::ChangePose(CHandle new_pos_h)
{
	TCompRenderStaticMesh *new_pose = new_pos_h;
	if (new_pose == actual_render) return;
	actual_render->unregisterFromRender();
	actual_render = new_pose;
	actual_render->registerToRender();
}