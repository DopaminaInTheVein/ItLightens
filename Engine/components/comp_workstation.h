#ifndef INC_WORKSTATION_COMPONENT_H_
#define	INC_WORKSTATION_COMPONENT_H_

#include "comp_base.h"
#include "entity.h"
#include "comp_transform.h"

struct TCompWorkstation : public TCompBase {

	CHandle mParent;	
	VEC3 position;
	std::string anim_name;

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
		return true;
	}

	VEC3 getPosition() {
		return position;
	}

	std::string getAnimation() {
		return anim_name;
	}
};

#endif