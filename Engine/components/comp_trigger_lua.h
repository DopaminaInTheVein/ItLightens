#ifndef	INC_COMP_TRIGGER_LUA
#define INC_COMP_TRIGGER_LUA

#include "comp_trigger.h"

class TTriggerLua : public TTrigger {
	void mUpdate(float dt) {} //not needed
	void onTriggerInside(const TMsgTriggerIn& msg) {}	//not needed
	void onTriggerEnter(const TMsgTriggerIn& msg);
	void onTriggerExit(const TMsgTriggerOut& msg);
	void executeTrigger(const char* nameEvent); //, CHandle handle);
};

#endif