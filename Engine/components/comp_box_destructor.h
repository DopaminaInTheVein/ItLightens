#ifndef	INC_BOX_DESTRUCTOR_H_
#define INC_BOX_DESTRUCTOR_H_

#include "comp_trigger.h"
#include "comp_msgs.h"

struct TCompBoxDestructor : public TTrigger {



	void mUpdate(float dt) {
	}
	void onTriggerInside(const TMsgTriggerIn& msg) {}
	void onTriggerEnter(const TMsgTriggerIn& msg) {
		if (msg.other.isValid()) {
			auto h = msg.other;
			if (h.hasTag("box")) {
				h.destroy();
			}
				
		}
	}
	void onTriggerExit(const TMsgTriggerOut& msg) {}
};


#endif