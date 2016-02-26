#include "mcv_platform.h"
#include "player_mole_controller.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_name.h"

#include "components\entity_tags.h"

#include "logic\ai_mole.h"

void player_mole_controller::Init() {
	player_controller::Init();

	AddState("grabBox", (statehandler)&player_mole_controller::GrabBox);
	AddState("leaveBox", (statehandler)&player_mole_controller::LeaveBox);
}

void player_mole_controller::GrabBox() {
	if (SBB::readHandlesVector("wptsBoxes").size() > 0) {
		float distMax = 15.0f;
		string key_final = "";
		bool found = false;
		for (int i = 0; i < SBB::readHandlesVector("wptsBoxes").size(); i++) {
			CEntity * entTransform = this->getEntityPointer(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
			string key = nameBox->name;
			if (disttowpt < distMax) {
				selectedBox = i;
				distMax = disttowpt;
				key_final = key;
				found = true;
			}
		}
		if (found) {
			if (SBB::readBool(key_final)) {
				ai_mole * mole = SBB::readMole(key_final);
				mole->ChangeState("idle");
				// quitar la mole que la tenia
			}
			else {
				SBB::postBool(key_final, true);
			}
		}
	}
}
void player_mole_controller::LeaveBox() {
}

// Decides which state to go next
string player_mole_controller::ParseInput() {
	dt = getDeltaTime();

	if (!ImGui::GetIO().WantCaptureKeyboard) {
		if (Input.IsLeftPressed()) {
			return "moveleft";
		}
		if (Input.IsRightPressed()) {
			return "moveright";
		}
		if (Input.IsUpPressed()) {
			return "moveup";
		}
		if (Input.IsDownPressed()) {
			return "movedown";
		}
		if (Input.IsSpacePressed()) {
			return "jump";
		}
		if (Input.IsLeftClickPressed()) {
			if (boxGrabbed) {
				boxGrabbed = false;
				return "leaveBox";
			}
			else {
				boxGrabbed = true;
				return "grabBox";
			}
		}
		if (Input.IsRightClickPressed()) {
			return "possess";
		}
		if (Input.IsOrientLeftPressed())
		{
			return "orientleft";
		}
		if (Input.IsOrientRightPressed())
		{
			return "orientright";
		}
	}
	else {
		Input.Unacquire();
	}

	return "idle";
}