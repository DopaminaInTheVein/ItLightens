#ifndef INC_COMPONENT_LIFE_H_
#define INC_COMPONENT_LIFE_H_

#include "comp_base.h"
#include "comp_msgs.h"
#include "utils/XMLParser.h"

// ------------------------------------
struct TCompLife : public TCompBase {
	float currentlife;
	float maxlife;
	TCompLife() : maxlife(100.f) {
		dbg("constructor of TCompLife\n");
	}
	~TCompLife() {
		dbg("destructor of TCompLife\n");
	}

	bool load(MKeyValue& atts) {
		currentlife = maxlife = atts.getFloat("points", 100.0f);
		return true;
	}

	void onCreate(const TMsgEntityCreated&) {
		dbg("TCompLife on TMsgEntityCreated\n");
	}
	void onDamage(const TMsgDamage& msg) {
		currentlife -= msg.points;
		if (currentlife < 0) {
			dbg("Me he muerto\n");
		}
	}
};

#endif
