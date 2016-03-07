#ifndef INC_COMP_WIRE_H_
#define	INC_COMP_WIRE_H_

#include "comp_base.h"
#include "comp_msgs.h"

struct TCompWire : public TCompBase {
	float half_lenght;
	VEC3 direction;
	VEC3 org;
	CHandle player;
	bool send = false;
	
	bool load(MKeyValue& atts);
	void update(float dt);
	void onCreate(const TMsgEntityCreated& msg);
	void init();
	void CanPass();
};

#endif