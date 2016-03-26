#ifndef INC_COMP_GENERATOR_H_
#define	INC_COMP_GENERATOR_H_

#include "comp_trigger.h"
#include "comp_msgs.h"

struct TCompGenerator : public TTrigger {
	float rad;
	VEC3 org;
	CHandle player;

	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated& msg);
	void init();
	void CanRec(bool new_range);

	void mUpdate(float dt);
	void onTriggerInside(const TMsgTriggerIn& msg) {} //will do nothing, particles effect, ui message or something
	void onTriggerEnter(const TMsgTriggerIn& msg);
	void onTriggerExit(const TMsgTriggerOut& msg);
};

#endif