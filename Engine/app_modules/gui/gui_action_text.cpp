#include "mcv_platform.h"

#include "gui_action_text.h"
#include "imgui/imgui_internal.h"

using namespace std;

CGuiActionText::CGuiActionText(Rect r) {
	rect = r;
	sizeFont = 0.025f;
}

void CGuiActionText::setState(eAction new_action) {
	action = new_action;
}

void CGuiActionText::update(float dt) {
	// Nothing animated at the moment
}

void CGuiActionText::render() {
	string text;
	switch (action) {
	case eAction::NONE:
		text = "";
		break;
	case eAction::OVERCHARGE:
		text = "ACTION: Overcharge";
		break;
	case eAction::POSSESS:
		text = "ACTION: Possess";
		break;
	case eAction::RECHARGE:
		text = "ACTION: Recharge";
		break;
	case eAction::RECHARGE_DRONE:
		text = "ACTION: Recharge Drone";
		break;
	case eAction::REPAIR_DRONE:
		text = "ACTION: Repair Drone";
		break;
	case eAction::ACTIVATE:
		text = "ACTION: Activate";
		break;
	case eAction::PUT:
		text = "ACTION: Put";
		break;
	case eAction::EXAMINATE:
		text = "ACTION: Examinate";
		break;
	case eAction::DESTROY:
		text = "ACTION: Destroy";
		break;
	case eAction::CREATE_MAGNETIC_BOMB:
		text = "ACTION: Create Bomb";
		break;
	case eAction::CREATING:
		text = "Creating...";
		break;
	case eAction::TRAVEL_WIRE:
		text = "ACTION: Pass through the wire";
		break;
	case eAction::DREAM:
		text = "ACTION: Enter to dream";
		break;
	case eAction::LEAVE:
		text = "ACTION: Leave";
		break;
	}

	if (text != "") {
		GUI::drawText(Pixel(rect.x, rect.y),
			GImGui->Font, sizeFont,
			GUI::IM_WHITE, text.c_str());
	}
}