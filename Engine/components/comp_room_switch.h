#ifndef INC_COMP_ROOM_SWITCH_H_
#define	INC_COMP_ROOM_SWITCH_H_

#include "comp_trigger.h"
#include "comp_msgs.h"

struct TCompRoomSwitch : public TTrigger {
	std::vector<int> room_back;
	std::vector<int> room_front;

	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated& msg);
	void init();

	void mUpdate(float dt);
	void onTriggerExit(const TMsgTriggerOut& msg);
	void onTriggerEnter(const TMsgTriggerIn& msg) {}
	void onTriggerInside(const TMsgTriggerIn& msg) {}
};

#endif