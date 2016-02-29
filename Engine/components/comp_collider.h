#ifndef INC_COMPONENT_NAME_H_
#define INC_COMPONENT_NAME_H_

#include "utils/XMLParser.h"
#include "comp_base.h"

struct TCompCollider : public TCompBase {
	float radius;
	float height;
	VEC3  position;

	bool load(MKeyValue& atts) {
		radius = atts.getFloat("radius", 0);
		height = atts.getFloat("height", 0);
		position = atts.getPoint("pos");
		return true;
	}

	void renderInMenu() {
	}
};

#endif
