#ifndef INC_COMP_SKELETON_LOOKAT_H_
#define INC_COMP_SKELETON_LOOKAT_H_

#include "components/comp_base.h"

class TCompTransform;
class TCompSkeleton;

struct TCompSkeletonLookAt : public TCompBase {
	VEC3 target;
	float    amount;
	TCompSkeletonLookAt() { }
	std::vector<std::string> look_at_bones{ "spine_down", "spine_up", "neck", "head" };
	std::vector<float> look_at_amoung{ 0.2f, 0.35f, 0.2f, 1.0f };
	float max_angle = deg2rad(110.f);

	TCompTransform * my_transform;
	TCompSkeleton * my_skeleton;

	bool load(MKeyValue &atts);
	bool getUpdateInfo();
	void update(float elapsed);
	void onCreate(const TMsgEntityCreated&);

	void renderInMenu();
};

#endif
