#include "mcv_platform.h"

#include "gui_action_text.h"
#include "components/entity.h"
#include "components/comp_text.h"
#include "components/entity_parser.h"
#include "app_modules/lang_manager/lang_manager.h"

using namespace std;

CGuiActionText::CGuiActionText(float x, float y) {
	assert(x >= 0.0f);
	assert(x < 1.0f);
	assert(y >= 0.0f);
	assert(y < 1.0f);
	posx = x;
	posy = y;
	sizeFont = 0.2f;
}

void CGuiActionText::setState(eAction new_action) {
	action = new_action;
}
eAction CGuiActionText::getState() {
	return action;
}

void CGuiActionText::update(float dt) {
	// Nothing animated at the moment
}

void CGuiActionText::render() {
	string text;
	switch (action) {
	case eAction::NONE:
		text = "";// lang_manager->getText("actions", "none");
		break;
	case eAction::OVERCHARGE:
		text = getActionText("overcharge");
		break;
	case eAction::POSSESS:
		text = getActionText("possess");
		break;
	case eAction::RECHARGE:
		text = getActionText("recharge");
		break;
	case eAction::RECHARGE_DRONE:
		text = getActionText("recharge_drone");
		break;
	case eAction::REPAIR_DRONE:
		text = getActionText("repair_drone");
		break;
	case eAction::ACTIVATE:
		text = getActionText("activate");
		break;
	case eAction::PUT:
		text = getActionText("put");
		break;
	case eAction::EXAMINATE:
		text = getActionText("examinate");
		break;
	case eAction::DESTROY:
		text = getActionText("destroy");
		break;
	case eAction::CREATE_MAGNETIC_BOMB:
		text = getActionText("create_magnetic_bomb");
		break;
	case eAction::CREATING:
		text = getActionText("creating");
		break;
	case eAction::TRAVEL_WIRE:
		text = getActionText("travel_wire");
		break;
	case eAction::DREAM:
		text = getActionText("dream");
		break;
	case eAction::LEAVE:
		text = getActionText("leave");
		break;
	}
	if (text != "" && text != last_text) {
		CHandle h = createPrefab("ui/text");
		GET_COMP(t, h, TCompText);
		t->setup(std::string("action_text"), std::string(text), posx, posy, std::string("#FFFFFFFF"), sizeFont, std::string("#FFFFFFFF"), 0.0f, 0.0f);
	}
	else if (text == "" && text != last_text) {
		getHandleManager<TCompText>()->each([](TCompText * mess) {
			mess->forceTTLZero();
		}
		);
	}
	last_text = text;
}

std::string CGuiActionText::getActionText(std::string action)
{
	return lang_manager->getText(CLangManagerModule::toEntry(action), "actions");
}