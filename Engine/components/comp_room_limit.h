#ifndef INC_COMP_ROOM_LIMIT_H_
#define	INC_COMP_ROOM_LIMIT_H_

#include "comp_trigger.h"
#include "comp_msgs.h"

struct TCompRoomLimit : public TTrigger {
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated& msg);
	void init();

	void mUpdate(float dt);
	void onTriggerExit(const TMsgTriggerOut& msg) {}
	void onTriggerEnter(const TMsgTriggerIn& msg);
	void onTriggerInside(const TMsgTriggerIn& msg) {}
};

#endif