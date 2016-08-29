#ifndef INC_COMP_TMX_ANIMATOR_H_
#define	INC_COMP_TMX_ANIMATOR_H_

#include "comp_base.h"

class TCompTransform;

struct TCompTransformAnimator : public TCompBase {
	float yaw_speed;
	float roll_speed;
	float pitch_speed;
	TCompTransform * tmx;

	bool load(MKeyValue& atts);
	bool getUpdateInfo();
	void update(float dt);
};

#endif