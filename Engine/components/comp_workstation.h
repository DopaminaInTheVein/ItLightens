#ifndef INC_WORKSTATION_COMPONENT_H_
#define	INC_WORKSTATION_COMPONENT_H_

#include "comp_base.h"
#include "entity.h"
#include "comp_transform.h"

struct TCompWorkstation : public TCompBase {

	CHandle mParent;	
	VEC3 position;
	std::string anim_name;
	VEC3 pos_action;
	float yaw_action;

	void init() {
		mParent = CHandle(this).getOwner();		
		CEntity *e = mParent;
		TCompTransform *t = e->get<TCompTransform>();
		position = t->getPosition();
	}

	void update(float elapsed) {
		// nothing to be done
	}

	bool load(MKeyValue& atts) {
		anim_name = atts.getString("animation", "idle");
		pos_action = atts.getPoint("posAction");
		yaw_action = atts.getFloat("yawAction", 0);
		return true;
	}

	VEC3 getPosition() {
		return position;
	}

	VEC3 getActionPosition() {
		return pos_action;
	}

	float getActionYaw() {
		return yaw_action;
	}

	std::string getAnimation() {
		return anim_name;
	}
};

#endif