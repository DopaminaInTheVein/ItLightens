#include "mcv_platform.h"
#include "comp_trigger_lua.h"
#include "components/entity.h"
#include "app_modules/logic_manager/logic_manager.h"

using namespace std;

void TTriggerLua::onTriggerEnter(const TMsgTriggerIn& msg) {
	executeTrigger("onEnter");//, msg.other);
}

void TTriggerLua::onTriggerExit(const TMsgTriggerOut& msg) {
	executeTrigger("onExit");//, msg.other);
}

void TTriggerLua::executeTrigger(const char* nameEvent) { //, CHandle handle) {
	CEntity* eMe = CHandle(this).getOwner();
	logic_manager->throwEvent(logic_manager->OnEnter, string(eMe->getName()));
}