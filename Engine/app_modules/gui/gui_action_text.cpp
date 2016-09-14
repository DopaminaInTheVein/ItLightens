#include "mcv_platform.h"

#include "gui_action_text.h"
#include "components/entity.h"
#include "components/comp_text.h"

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
	if (text != "" && text != last_text) {
		auto hm = CHandleManager::getByName("entity");
		CHandle new_hp = hm->createHandle();
		CEntity* entity = new_hp;

		auto hm1 = CHandleManager::getByName("name");
		CHandle new_hn = hm1->createHandle();
		MKeyValue atts1;
		atts1.put("name", "helpText");
		new_hn.load(atts1);
		entity->add(new_hn);

		auto hm3 = CHandleManager::getByName("helper_text");
		CHandle new_hl = hm3->createHandle();
		MKeyValue atts3;
		atts3["text"] = text;
		atts3["pos_x"] = std::to_string(posx);
		atts3["pos_y"] = std::to_string(posy);
		new_hl.load(atts3);
		entity->add(new_hl);

		//Add tag talk text
		TMsgSetTag msg;
		msg.add = true;
		msg.tag = "talk_text";
		new_hp.sendMsg(msg);
	}
	else if (text == "" && text != last_text) {
		getHandleManager<TCompText>()->each([](TCompText * mess) {
			mess->forceTTLZero();
		}
		);
	}
	last_text = text;
}