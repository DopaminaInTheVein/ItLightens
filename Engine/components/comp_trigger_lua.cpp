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
	if (actionable) {
		Gui->setActionAvailable(action);
		if (io->keys['E'].becomesPressed()) {
			onTriggerAction();
			actionable = false;
		}
	}
}


void TTriggerLua::executeTrigger(CLogicManagerModule::EVENT logicEvent) { //, CHandle handle) {
	CEntity* eMe = CHandle(this).getOwner();
	logic_manager->throwEvent(logicEvent, string(eMe->getName()));
}