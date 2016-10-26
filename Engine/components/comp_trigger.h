#ifndef	INC_COMP_TRIGGER_H_
#define INC_COMP_TRIGGER_H_

#include "comp_base.h"
#include "player_controllers/player_controller_mole.h"
#include "player_controllers/player_controller_cientifico.h"

struct TTrigger : public TCompBase {
	bool triggered = false;
	TMsgTriggerIn last_msg_in;
	TMsgTriggerOut last_msg_out;

	// possession management
	bool possessing = false;
	CHandle pos_handle;

	void update(float elapsed) {
		if (triggered) onTriggerInside(last_msg_in);
		mUpdate(elapsed);
	}

	void onTriggerEnterCall(const TMsgTriggerIn& msg) {
		PROFILE_FUNCTION("Trigger Enter Call");
		CHandle hEnter = msg.other;
		if (hEnter.isValid()) {
			dbg("Entro en trigger!\n");
			if (hEnter.hasTag("player")) {
				CEntity* eEnter = hEnter;
				player_controller_mole* cont_mole = eEnter->get<player_controller_mole>();
				player_controller_cientifico* cont_cientifico = eEnter->get<player_controller_cientifico>();
				// if we are possessing, we activate the flags
				if (cont_mole || cont_cientifico) {
					possessing = true;
					pos_handle = hEnter;
				}
				// if we are not possessing, but we entered possessing, exit the possession
				else if (possessing) {
					possessing = false;
					TMsgTriggerOut out_msg;
					out_msg.other = pos_handle;
					onTriggerExit(out_msg); 
				}
				Debug->LogRaw("OnTriggerEnter\n");
				last_msg_in = msg;
				triggered = true;
				onTriggerEnter(last_msg_in);
			}
		}
	}

	void onTriggerExitCall(const TMsgTriggerOut& msg) {
		PROFILE_FUNCTION("Trigger Exit Call");
		CHandle hExit = msg.other;
		if (hExit.isValid()) {
			dbg("Salgo de trigger!\n");
			if (hExit.hasTag("player")) {
				Debug->LogRaw("OnTriggerExit\n");
				last_msg_out = msg;
				triggered = false;
				onTriggerExit(last_msg_out);
			}
		}
	}

	virtual void mUpdate(float dt) = 0;
	virtual void onTriggerInside(const TMsgTriggerIn& msg) = 0;
	virtual void onTriggerEnter(const TMsgTriggerIn& msg) = 0;
	virtual void onTriggerExit(const TMsgTriggerOut& msg) = 0;
};

#endif