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
	case eAction::ACTIVATE:
		text = "ACTION: Activate";
		break;
	case eAction::PUT:
		text = "ACTION: Put";
		break;
	}

	if (text != "") {
		GUI::drawText(Pixel(rect.x, rect.y),
			GImGui->Font, sizeFont,
			GUI::IM_WHITE, text.c_str());
	}
}