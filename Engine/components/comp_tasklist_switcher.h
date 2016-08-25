#ifndef INC_COMP_TASKLIST_SWITCH_H_
#define	INC_COMP_TASKLIST_SWITCH_H_

#include "comp_trigger.h"
#include "comp_msgs.h"

struct TasklistSwitch : public TTrigger {
	int task;

	bool load(MKeyValue& atts);
	bool save(std::ofstream& ofs, MKeyValue& atts);
	void onCreate(const TMsgEntityCreated& msg);
	void init();

	void mUpdate(float dt);
	void onTriggerExit(const TMsgTriggerOut& msg) {}
	void onTriggerEnter(const TMsgTriggerIn& msg);
	void onTriggerInside(const TMsgTriggerIn& msg) {}
};

#endif