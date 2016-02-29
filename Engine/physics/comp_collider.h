#ifndef INC_COMPONENT_COLLIDER_H_
#define INC_COMPONENT_COLLIDER_H_

#include "utils/XMLParser.h"
#include "components/comp_base.h"

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

	void collide() {
		dbg("Hola soy collider %f %f (%f, %f, %f), ", radius, height, position.x, position.y, position.z);
	}
};

#endif
