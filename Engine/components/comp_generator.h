#ifndef INC_COMP_GENERATOR_H_
#define	INC_COMP_GENERATOR_H_

#include "comp_trigger.h"
#include "comp_msgs.h"

struct TCompGenerator : public TTrigger {
	float life_recover;
	float rad;
	VEC3 org;
	ClHandle player;
	ClHandle mesh;
	static VHandles all_generators;

	____TIMER_DECLARE_(timeReuse);

	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated& msg);
	void init();
	void CanRec(bool new_range);
	void setUsable(bool);

	bool isUsable();
	float use();
	void mUpdate(float dt);
	void onTriggerInside(const TMsgTriggerIn& msg) {} //will do nothing, particles effect, ui message or something
	void onTriggerEnter(const TMsgTriggerIn& msg);
	void onTriggerExit(const TMsgTriggerOut& msg);
	~TCompGenerator();
};

#endif