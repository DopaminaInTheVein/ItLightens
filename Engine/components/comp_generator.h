#ifndef INC_COMP_GENERATOR_H_
#define	INC_COMP_GENERATOR_H_

#include "comp_base.h"
#include "comp_msgs.h"

struct TCompGenerator : public TCompBase {
	float rad;
	VEC3 org;
	CHandle player;

	bool load(MKeyValue& atts);
	void update(float dt);
	void onCreate(const TMsgEntityCreated& msg);
	void init();
	void CanRec();
};

#endif