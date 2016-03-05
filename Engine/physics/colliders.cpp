#include "mcv_platform.h"
#include "colliders.h"
#include "handle\handle.h"
#include "utils/XMLParser.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "physics.h"

using namespace std;

//####################################################
//COLLIDER:
//####################################################

//Sphere
//---------------------------------------------------------
bool sphereCollider::load(MKeyValue & atts)
{
	r = atts.getFloat("radius", 1.0f);
	strcpy(tag, atts.getString("tag", "").c_str());
	return true;
}

void sphereCollider::onCreate(const TMsgEntityCreated &)
{
	CHandle me = CHandle(this);
	CHandle parent = me.getOwner();
	CEntity *p_e = parent;
	p_e->setCollisionType(CEntity::SPHERE);
}

//transfrom already created, we can use Transform
VEC3 sphereCollider::getCenter() const
{
	VEC3 c;
	CHandle me = CHandle(this);
	CHandle parent = me.getOwner();
	CEntity *p_e = parent;

	TCompTransform * t = p_e->get<TCompTransform>();

	VEC3 org = t->getPosition();

	c = org + VEC3(0, r, 0);
	return c;
}
//---------------------------------------------------------

//Box
//---------------------------------------------------------
bool boxCollider::load(MKeyValue & atts)
{
	relative_p1 = atts.getPoint("p1");
	relative_p2 = atts.getPoint("p2");

	strcpy(tag, atts.getString("tag", "").c_str());
	types = atts.getInt("types", COL_TAG_WALL);
	return true;
}

void boxCollider::onCreate(const TMsgEntityCreated &)
{
	CHandle me = CHandle(this);
	CHandle parent = me.getOwner();
	CEntity *p_e = parent;
	p_e->setCollisionType(CEntity::BOX);
}

VEC3 boxCollider::getPMIN() const
{
	VEC3 p;
	CHandle me = CHandle(this);
	if (me.isValid()) {
		CHandle parent = me.getOwner();
		if (parent.isValid()) {
			CEntity *p_e = parent;

			TCompTransform * t = p_e->get<TCompTransform>();
			CQuaternion rot = t->getRotation();
			rot.Inverse(rot);
			CQuaternion rot2 = rot;
			rot.Conjugate();
			CQuaternion aux = CQuaternion(relative_p2.x, relative_p2.y, relative_p2.z, 0);
			aux = rot2 * aux;
			aux = aux * rot;
			VEC3 orientedP2 = VEC3(aux.x, aux.y, aux.z);
			VEC3 org = t->getPosition();
			p = org + orientedP2;
			return p;
		}
	}
	return VEC3(0,0,0);
}

VEC3 boxCollider::getPMAX() const
{
	VEC3 p;
	CHandle me = CHandle(this);
	if (me.isValid()) {
		CHandle parent = me.getOwner();
		if (parent.isValid()) {
			CEntity *p_e = parent;
			TCompTransform * t = p_e->get<TCompTransform>();
			CQuaternion rot = t->getRotation();
			rot.Inverse(rot);
			CQuaternion rot2 = rot;
			rot.Conjugate();
			CQuaternion aux = CQuaternion(relative_p1.x, relative_p1.y, relative_p1.z, 0);
			aux = rot2 * aux;
			aux = aux * rot;
			VEC3 orientedP1 = VEC3(aux.x, aux.y, aux.z);
			VEC3 org = t->getPosition();

			p = org + orientedP1;
			return p;

		}
	}
	return VEC3(0, 0, 0);
}

// RayCast (sólo el más cercano)
void boxCollider::rayCast() {
	ray_cast_halfway* rHalfWay = &Physics::RayCastHalfWay;
	ray_cast_query* rQuery = &rHalfWay->query;

	if (rQuery->types & types == 0) return;

	VEC3 min = getPMIN(), max = getPMAX();
	VEC3 rOrig = rQuery->position;
	VEC3 rDir = rQuery->direction;
	float tmin = (min.x - rOrig.x) / rDir.x;
	float tmax = (max.x - rOrig.x) / rDir.x;

	if (tmin > tmax) swap(tmin, tmax);

	float tymin = (min.y - rOrig.y) / rDir.y;
	float tymax = (max.y - rOrig.y) / rDir.y;

	if (tymin > tymax) swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax)) return;

	if (tymin > tmin) tmin = tymin;

	if (tymax < tmax) tmax = tymax;

	float tzmin = (min.z - rOrig.z) / rDir.z;
	float tzmax = (max.z - rOrig.z) / rDir.z;

	if (tzmin > tzmax) swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax)) return;

	if (tzmin > tmin) tmin = tzmin;

	if (tzmax < tmax) tmax = tzmax;

	if (tmin > 0 && tmin < rQuery->maxDistance) {
		// Si colisiona, guardamos resultado
		rQuery->maxDistance = tmin;
		rHalfWay->posCollision = rOrig + rDir * tmin;
		rHalfWay->handle = CHandle(this).getOwner();
	}
}

//---------------------------------------------------------