#include "mcv_platform.h"
#include "comp_trigger_lua.h"
#include "components/entity.h"

using namespace std;

void TTriggerLua::onTriggerEnter(const TMsgTriggerIn& msg) {
	executeTrigger(logic_manager->OnEnter);//, msg.other);
}

void TTriggerLua::onTriggerExit(const TMsgTriggerOut& msg) {
	executeTrigger(logic_manager->OnLeave);//, msg.other);
}

void TTriggerLua::onTriggerAction() {
	executeTrigger(logic_manager->OnAction);//, msg.other);
}

void TTriggerLua::onTriggerInside(const TMsgTriggerIn& msg) {
	if (mActionable) {
		Gui->setActionAvailable(mAction);
		if (io->keys['E'].becomesPressed() || io->mouse.left.becomesPressed()) {
			mActionable = false;
			onTriggerAction();
		}
	}
}
void TTriggerLua::onSetActionable(const TMsgSetActivable& msg) {
	mActionable = msg.activable;
}

void TTriggerLua::setActionable(bool actionable) {
	mActionable = actionable;
}

void TTriggerLua::executeTrigger(CLogicManagerModule::EVENT logicEvent) { //, CHandle handle) {
	CEntity* eMe = CHandle(this).getOwner();
	logic_manager->throwEvent(logicEvent, string(eMe->getName()), CHandle(this).getOwner());
}

bool TTriggerLua::load(MKeyValue& atts) {
	string action = atts.getString("action", "");
	
	//Action
	if (action == "activate") {
		mAction = ACTIVATE;
	} else {
		mAction = NONE;
	}
	mActionable = (mAction != NONE);

	return true;
}