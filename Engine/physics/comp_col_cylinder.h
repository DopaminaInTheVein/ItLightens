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

	// RayCast (sólo el más cercano)
	void rayCast() {
		ray_cast_halfway* rHalfWay = &Physics::RayCastHalfWay;
		ray_cast_query* rQuery = &rHalfWay->query;

		// Filtro tipo
		dbg("My Type: %d\n", type);
		dbg("Types: %d\n", rQuery->types);
		if (!(type & rQuery->types)) return;

		//Calculo Raycast vs. Cylinder
		//(Suponemos Cilidor alineado en eje Y)
		//p = Origen del rayo						  
		//v = Direccion del rayo					  
		//q = Centro de la base inferior del cilindro 
		//radius = Radio del cilindro					  
		//alfa = Angulo entre {PQ} y {tangente de la base del cilindro que pasa por P}
		//beta = Angulo entre {v} y {PQ}
		//Colision = abs(alfa) > abs(beta)

		CEntity* eMe = CHandle(this).getOwner();
		TCompTransform* tMe = eMe->get<TCompTransform>();
		VEC3 p = rQuery->position;
		VEC3 q = tMe->getPosition() + position;
		VEC3 v = rQuery->direction;

		//Calculo Distancia (Si está más lejos que la colision encontrada paramos)
		float distancePQ = realDistXZ(q, p);
		if (distancePQ > rQuery->maxDistance) return;
		
		//Calculo alfa
		float alfa = atan2f(radius, distancePQ);

		//Calculo beta
		VEC3 u = (q - p);
		u.Normalize();
		float cosBeta = u.x * v.x + u.z * v.z;
		float beta = acos(cosBeta);

		// Si colisiona, guardamos resultado
		if (abs(alfa) > abs(beta)) {
			rHalfWay->posCollision = q;
			rHalfWay->handle = CHandle(this);
			rQuery->maxDistance = distancePQ;
		}

	}
};

#endif
