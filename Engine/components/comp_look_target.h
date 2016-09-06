#ifndef INC_COMPONENT_LOOK_TARGET_H_
#define INC_COMPONENT_LOOK_TARGET_H_

#include "comp_base.h"
class TCompAbsAABB;
struct TCompLookTarget : public TCompBase {
	bool visible;
	float time_elapsed;
	float time_wait;

	CHandle culling;
	TCompAbsAABB * aabb;
	std::string lua_in;
	std::string lua_out;
	bool sense_vision_needed;
public:
	bool getUpdateInfo();
	bool load(MKeyValue& atts);
	void update(float dt);
	void renderInMenu();
};

#endif
