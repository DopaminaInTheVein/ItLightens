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
		text = lang_manager->getText("actions", "none");
		break;
	case eAction::OVERCHARGE:
		text = lang_manager->getText("actions", "overcharge");
		break;
	case eAction::POSSESS:
		text = lang_manager->getText("actions", "possess");
		break;
	case eAction::RECHARGE:
		text = lang_manager->getText("actions", "recharge");
		break;
	case eAction::RECHARGE_DRONE:
		text = lang_manager->getText("actions", "recharge_drone");
		break;
	case eAction::REPAIR_DRONE:
		text = lang_manager->getText("actions", "repair_drone");
		break;
	case eAction::ACTIVATE:
		text = lang_manager->getText("actions", "activate");
		break;
	case eAction::PUT:
		text = lang_manager->getText("actions", "put");
		break;
	case eAction::EXAMINATE:
		text = lang_manager->getText("actions", "examinate");
		break;
	case eAction::DESTROY:
		text = lang_manager->getText("actions", "destroy");
		break;
	case eAction::CREATE_MAGNETIC_BOMB:
		text = lang_manager->getText("actions", "create_magnetic_bomb");
		break;
	case eAction::CREATING:
		text = lang_manager->getText("actions", "creating");
		break;
	case eAction::TRAVEL_WIRE:
		text = lang_manager->getText("actions", "travel_wire");
		break;
	case eAction::DREAM:
		text = lang_manager->getText("actions", "dream");
		break;
	case eAction::LEAVE:
		text = lang_manager->getText("actions", "leave");
		break;
	}
	if (text != "" && text != last_text) {
		CHandle h = createPrefab("ui/text");
		GET_COMP(t, h, TCompText);
		t->setup(std::string("action_text"), std::string(text), posx, posy, std::string("#FFFFFFFF"), 0.2f, std::string("#FFFFFFFF"), 0.0f, 0.0f);
	}
	else if (text == "" && text != last_text) {
		getHandleManager<TCompText>()->each([](TCompText * mess) {
			mess->forceTTLZero();
		}
		);
	}
	last_text = text;
}