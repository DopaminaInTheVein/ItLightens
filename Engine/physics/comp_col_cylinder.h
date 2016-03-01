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

	bool load(MKeyValue& atts) {
		radius = atts.getFloat("radius", 0);
		height = atts.getFloat("height", 0);
		position = atts.getPoint("pos");
		std::string type_str = atts.getString("type", "");
		if (type_str.compare("player") == 0)		type = COL_TAG_PLAYER;
		else if (type_str.compare("enemy") == 0)	type = COL_TAG_ENEMY;
		else if (type_str.compare("object") == 0)	type = COL_TAG_OBJECT;

		return true;
	}

	void renderInMenu() {
	}

	void rayCast() {
		ray_cast_halfway* rHalfWay = &Physics::RayCastHalfWay;
		ray_cast_query* rQuery = &rHalfWay->query;

		// Filtro tipo
		dbg("My Type: %d\n", type);
		dbg("Types: %d\n", rQuery->types);
		if (!(type & rQuery->types)) return;

		//TODO: Calcular raycast de verdad
		CEntity* eMe = CHandle(this).getOwner();
		TCompTransform* tMe = eMe->get<TCompTransform>();
		VEC3 myPos = tMe->getPosition() + position;

		//Prueba: devolver el collider más cercano
		float distance = realDist(myPos, rQuery->position);
		if (distance < rQuery->maxDistance) {
			rQuery->maxDistance = distance;
			rHalfWay->posCollision = myPos;
			Physics::RayCastHalfWay.handle = CHandle(this);
		}
	}
};

#endif
