#ifndef INC_COMP_TMX_ANIMATOR_H_
#define	INC_COMP_TMX_ANIMATOR_H_

#include "comp_base.h"

#define TMX_ANIMATOR_NULL_VAL -1000.f

class TCompTransform;

struct TCompTransformAnimator : public TCompBase {
	float yaw_speed;
	float roll_speed;
	float pitch_speed;
	float roll_target = TMX_ANIMATOR_NULL_VAL;
	float roll_target_speed = TMX_ANIMATOR_NULL_VAL;
	TCompTransform * tmx;

	bool load(MKeyValue& atts);
	bool getUpdateInfo();
	void update(float dt);
	void SetRollTarget(float new_roll, float time);
};

#endif