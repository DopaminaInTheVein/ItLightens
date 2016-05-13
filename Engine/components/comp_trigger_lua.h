#ifndef	INC_COMP_TRIGGER_LUA
#define INC_COMP_TRIGGER_LUA

#include "comp_trigger.h"

#include "app_modules/gui/gui.h"
#include "app_modules/logic_manager/logic_manager.h"

class TTriggerLua : public TTrigger {
	bool actionable;
	eAction action;

	void mupdate(float dt) {} //not needed
	void onTriggerInside(const TMsgTriggerIn& msg);
	void onTriggerEnter(const TMsgTriggerIn& msg);
	void onTriggerExit(const TMsgTriggerOut& msg);
	void onTriggerAction();
	void executeTrigger(CLogicManagerModule::EVENT logicEvent); //, CHandle handle);
};

#endif