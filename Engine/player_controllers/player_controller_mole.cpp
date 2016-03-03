#include "mcv_platform.h"
#include "player_controller_mole.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_name.h"

#include "components\entity_tags.h"

#include "logic\ai_mole.h"

void player_controller_mole::Init() {
	om = getHandleManager<player_controller_mole>();	//player

	DeleteState("jumping");
	DeleteState("falling");
	DeleteState("moving");

	AddState("grabBox", (statehandler)&player_controller_mole::GrabBox);
	AddState("leaveBox", (statehandler)&player_controller_mole::LeaveBox);
	AddState("destroyWall", (statehandler)&player_controller_mole::DestroyWall);
	AddState("moving", (statehandler)&player_controller_mole::Moving);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
}
void player_controller_mole::UpdateInputActions() {
	energyDecreasal(getDeltaTime()*0.5f);
	if (Input.IsLeftClickReleased()) {
		if (boxGrabbed) {
			ChangeState("leaveBox");
		}
		else {
			if (this->nearToBox()) {
				ChangeState("grabBox");
			}
			else if (this->nearToWall()) {
				ChangeState("destroyWall");
			}
		}
	}
}

void player_controller_mole::Moving()
{
	UpdateDirection();
	energyDecreasal(getDeltaTime()*0.5f);
	if (boxGrabbed) {
		energyDecreasal(getDeltaTime()*0.5f);
		CEntity* box = SBB::readHandlesVector("wptsBoxes")[selectedBoxi];
		TCompTransform* box_t = box->get<TCompTransform>();
		SetMyEntity();
		CEntity* p = myParent;
		TCompTransform* p_t = p->get<TCompTransform>();

		box_t->setPosition(p_t->getPosition());
	}

	if (!UpdateMovDirection()) ChangeState("idle");
}

void player_controller_mole::GrabBox() {
	if (SBB::readBool(selectedBox)) {
		ai_mole * mole = SBB::readMole(selectedBox);
		mole->ChangeState("idle");
	}
	else {
		SBB::postBool(selectedBox, true);
	}
	energyDecreasal(5.0f);
	boxGrabbed = true;
	player_max_speed /= 2;
	ChangeState("idle");
}

void player_controller_mole::DestroyWall() {
	energyDecreasal(10.0f);
	vector<CHandle> handles = SBB::readHandlesVector("wptsBreakableWall");
	handles.erase(handles.begin() + selectedWallToBreaki);
	getHandleManager<CEntity>()->destroyHandle(getEntityWallHandle(selectedWallToBreaki));
	SBB::postHandlesVector("wptsBreakableWall", handles);
	ChangeState("idle");
}

void player_controller_mole::LeaveBox() {
	SBB::postBool(selectedBox, false);
	boxGrabbed = false;
	player_max_speed *= 2;
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
		string key_final = "";
		for (int i = 0; i < SBB::readHandlesVector("wptsBoxes").size(); i++) {
			CEntity * entTransform = this->getEntityBoxPointer(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
			string key = nameBox->name;
			if (disttowpt < distMax) {
				distMax = disttowpt;
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