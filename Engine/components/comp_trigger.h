#ifndef	INC_COMP_TRIGGER_H_
#define INC_COMP_TRIGGER_H_

#include "comp_base.h"

struct TTrigger : public TCompBase {
	bool triggered = false;
	TMsgTriggerIn last_msg_in;
	TMsgTriggerOut last_msg_out;

	void update(float elapsed) {
		if (triggered) onTriggerInside(last_msg_in);
		mUpdate(elapsed);
	}

	void onTriggerEnterCall(const TMsgTriggerIn& msg) {
		Debug->LogRaw("OnTriggerEnter\n");
		last_msg_in = msg;
		triggered = true;
		onTriggerEnter(last_msg_in);
	}

	void onTriggerExitCall(const TMsgTriggerOut& msg) {
		Debug->LogRaw("OnTriggerExit\n");
		last_msg_out = msg;
		triggered = false;
		onTriggerExit(last_msg_out);
	}

	virtual void mUpdate(float dt) = 0;
	virtual void onTriggerInside(const TMsgTriggerIn& msg) = 0;
	virtual void onTriggerEnter(const TMsgTriggerIn& msg) = 0;
	virtual void onTriggerExit(const TMsgTriggerOut& msg) = 0;
};

#endif