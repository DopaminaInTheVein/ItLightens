#include "mcv_platform.h"
#include "colliders.h"
#include "handle\handle.h"
#include "utils/XMLParser.h"
#include "components\entity.h"
#include "components\comp_transform.h"


//####################################################
//COLLIDER:
//####################################################

//Sphere
//---------------------------------------------------------
bool sphereCollider::load(MKeyValue & atts)
{
	r = atts.getFloat("radius",1.0f);
	strcpy(tag, atts.getString("tag","").c_str());
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

	TCompTransform * t =  p_e->get<TCompTransform>();

	VEC3 org = t->getPosition();

	c = org + VEC3(0,r,0);
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
	int i = 0;
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
	CHandle parent = me.getOwner();
	CEntity *p_e = parent;

	TCompTransform * t = p_e->get<TCompTransform>();
	CQuaternion rot = t->getRotation();
	rot.Inverse(rot);
	CQuaternion rot2 = rot;
	rot.Conjugate();
	CQuaternion aux = CQuaternion(relative_p2.x,relative_p2.y,relative_p2.z,0);
	aux = rot2 * aux;
	aux = aux * rot;
	VEC3 orientedP2 = VEC3(aux.x,aux.y,aux.z);
	VEC3 org = t->getPosition();
	p = org + orientedP2;
	return p;
}


VEC3 boxCollider::getPMAX() const
{
	VEC3 p;
	CHandle me = CHandle(this);
	CHandle parent = me.getOwner();
	CEntity *p_e = parent;

	TCompTransform * t = p_e->get<TCompTransform>();
	CQuaternion rot = t->getRotation();
	rot.Inverse(rot);
	CQuaternion rot2 = rot;
	rot.Conjugate();
	CQuaternion aux = CQuaternion(relative_p1.x, relative_p1.y, relative_p1.z,0);
	aux = rot2 * aux;
	aux = aux * rot;
	VEC3 orientedP1 = VEC3(aux.x, aux.y, aux.z);
	VEC3 org = t->getPosition();

	p = org + orientedP1;
	return p;
}
//---------------------------------------------------------