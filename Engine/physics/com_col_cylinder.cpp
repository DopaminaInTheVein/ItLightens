#include "mcv_platform.h"
#include "physics.h"
#include "comp_col_cylinder.h"

#define EPSILON_ANGLE_RAD 0.001f

bool TCompColCillinder::load(MKeyValue& atts) {
	radius = atts.getFloat("radius", 0);
	height = atts.getFloat("height", 0);
	position = atts.getPoint("pos");
	std::string type_str = atts.getString("type", "");
	if (type_str.compare("player") == 0)		type = COL_TAG_PLAYER;
	else if (type_str.compare("enemy") == 0)	type = COL_TAG_ENEMY;
	else if (type_str.compare("object") == 0)	type = COL_TAG_OBJECT;

	return true;
}

void TCompColCillinder::renderInMenu() {
}

// RayCast (sólo el más cercano)
void TCompColCillinder::rayCast() {
	ray_cast_halfway* rHalfWay = &Physics::RayCastHalfWay;
	ray_cast_query* rQuery = &rHalfWay->query;

	// Filtro tipo
	if (type & rQuery->types == 0) return;

	//Calculo Raycast vs. Cylinder
	//(Suponemos Cilidor alineado en eje Y)
	//p = Origen del rayo
	//v = Direccion del rayo normalizado
	//q = Centro de la base inferior del cilindro
	//radius = Radio del cilindro
	//alfa = Angulo entre {PQ} y {tangente de la base del cilindro que pasa por P}
	//beta = Angulo entre {v} y {PQ}
	//ColisionXZ = abs(alfa) > abs(beta)

	CEntity* eMe = CHandle(this).getOwner();
	TCompTransform* tMe = eMe->get<TCompTransform>();
	VEC3 p = rQuery->position;
	VEC3 q = tMe->getPosition() + position;
	VEC3 v = rQuery->direction;

	//Calculo Distancia (Si está más lejos que la colision encontrada paramos)
	//Debug height
	//VEC3 offset = VEC3(0.2, 0, 0.2);
	//Debug->DrawLine(p, q, GREEN);
	//Debug->DrawLine(q + offset, q + VEC3(0, height, 0) + offset, GREEN);
	float distancePQ = realDistXZ(q, p);
	if (distancePQ > rQuery->maxDistance) return;

	//Calculo alfa
	float alfa = atan2f(radius, distancePQ);

	//Calculo beta
	VEC3 vXZ = VEC3(v.x, 0, v.z);
	vXZ.Normalize();
	VEC3 u = VEC3(q.x - p.x, 0, q.z - p.z); //PQxz
	u.Normalize();
	float cosBeta = u.x * vXZ.x + u.z * vXZ.z;
	float beta;
	if (abs(1 - cosBeta) < EPSILON_ANGLE_RAD) beta = 0;
	else beta = acos(cosBeta);

	//Colisiona en XZ, comprobar pitch vs. altura
	if (abs(alfa) > abs(beta)) {
		VEC3 s = q - (u * radius); //Point Base Outside
		VEC3 vBottomOutside = VEC3(s.x - p.x, q.y - p.y, s.z - p.z);
		VEC3 vTopOutside = VEC3(s.x - p.x, height + q.y - p.y, s.z - p.z);
		vTopOutside.Normalize();
		vBottomOutside.Normalize();
		if (vTopOutside.y > v.y && vBottomOutside.y < v.y) {
			// Si colisiona, guardamos resultado
			rHalfWay->posCollision = q;
			rHalfWay->handle = CHandle(this).getOwner();
			rQuery->maxDistance = realDist(p, q);
		}
	}
}