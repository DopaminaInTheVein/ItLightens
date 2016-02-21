#ifndef INC_COMPONENT_LIFE_H_
#define INC_COMPONENT_LIFE_H_

#include "comp_base.h"
#include "comp_msgs.h"

// ------------------------------------
struct TCompLife : public TCompBase {
	float life;
	TCompLife() : life(0.f) {
		dbg("ctor of TCompLife\n");
	}
	~TCompLife() {
		dbg("dtor of TCompLife\n");
	}
	void onCreate(const TMsgEntityCreated&) {
		dbg("TCompLife on TMsgEntityCreated\n");
	}
	void onDamage(const TMsgDamage& msg) {
		life -= msg.points;
		if (life < 0) {
			dbg("Me he muerto\n");
		}
	}
};

#endif
