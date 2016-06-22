#ifndef INC_COMP_SKELETON_IK_H_
#define INC_COMP_SKELETON_IK_H_

#include "components/comp_base.h"

struct TCompSkeletonIK : public TCompBase {
	float     amount;

	struct TBoneMod {
		VEC3  normal;
		int   bone_id;
	};

	std::vector<TBoneMod>  mods;
	void  solveBone(TBoneMod* bm);

	TCompSkeletonIK() : amount(1.0f) {}
	bool load(MKeyValue &atts);
	void update(float elapsed);
	void onCreate(const TMsgEntityCreated&);
};

#endif
