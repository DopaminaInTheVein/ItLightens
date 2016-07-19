#ifndef INC_COMP_SKELETON_LOOKAT_H_
#define INC_COMP_SKELETON_LOOKAT_H_

#include "components/comp_base.h"

struct TCompSkeletonLookAt : public TCompBase {
	VEC3 target;
	float    amount;
	TCompSkeletonLookAt() { }
	bool load(MKeyValue &atts);
	void update(float elapsed);
	void onCreate(const TMsgEntityCreated&);
};

#endif
