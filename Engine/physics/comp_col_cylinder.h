#ifndef INC_COMPONENT_COLLIDER_H_
#define INC_COMPONENT_COLLIDER_H_

#include "utils/XMLParser.h"
#include "components/comp_base.h"
#include "components/entity.h"
#include "components/comp_transform.h"
#include "physics.h"

struct TCompColCillinder : public TCompBase {
	float radius;
	float height;
	VEC3  position;
	char type;

	bool load(MKeyValue& atts);

	void renderInMenu();

	// RayCast (sólo el más cercano)
	void rayCast();
};

#endif
