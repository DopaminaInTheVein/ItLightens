#include "mcv_platform.h"
#include "simple_physx.h"
#include "handle\handle.h"
#include "components\entity_tags.h"

#include "components\entity.h"
#include "components\comp_transform.h"

#include "components\entity_tags.h"
#include "colliders.h"



bool CSimplePhysx::isMovementValid(CHandle h, std::string tag)
{

	vector<string> objToCollide = relations[tag];
	for (auto objs : objToCollide) {
		VHandles objs_h = tags_manager.getHandlesByTag(getID(objs.c_str()));
		for (auto obj : objs_h) {
			if (isColliding(h, obj))
				return false;

		}
	}
	return true;
}

void CSimplePhysx::test()
{
	CHandle h1 = tags_manager.getFirstHavingTag(getID("coll1"));
	CHandle h2 = tags_manager.getFirstHavingTag(getID("coll2"));

	CEntity *e1 = h1;
	CEntity *e2 = h2;

	TCompTransform *t1 = e1->get<TCompTransform>();
	TCompTransform *t2 = e2->get<TCompTransform>();

	VEC3 p1 = VEC3(1.5,0,-0.5);
	VEC3 p2 = VEC3(2.5,1,-1.5);



	VEC3 c = VEC3(1,1,0);
	float r = 1;

	
	p1 = VEC3(t2->getPosition().x-0.5f, t2->getPosition().y,t2->getPosition().z - 0.5f);
	p2 = VEC3(t2->getPosition().x + 0.5f, t2->getPosition().y+1, t2->getPosition().z + 0.5f);


}

bool CSimplePhysx::isColliding(CHandle own, CHandle other)
{
	CEntity *e1 = own;
	CEntity *e2 = other;
	if (e1 && e2) {
		if (e1->getCollisionType() == CEntity::SPHERE && e2->getCollisionType() == CEntity::SPHERE) {
			sphereCollider *sc1 = e1->get<sphereCollider>();
			sphereCollider *sc2 = e2->get<sphereCollider>();

			return SphereVsSphere(sc1->getRadius(), sc1->getCenter(), sc2->getRadius(), sc2->getCenter());
		}

		if (e1->getCollisionType() == CEntity::SPHERE && e2->getCollisionType() == CEntity::BOX) {
			sphereCollider	*sc1 = e1->get<sphereCollider>();
			boxCollider		*bc2 = e2->get<boxCollider>();

			return SphereVsBox(sc1->getRadius(), sc1->getCenter(), bc2->getPMIN(), bc2->getPMAX());
		}
	}
	return false;
}

bool CSimplePhysx::SphereVsSphere(float r1, VEC3 c1, float r2, VEC3 c2)
{
	float simple_d = simpleDist(c1, c2);
	bool ret = false;
	if (simple_d < squared(r1 + r2)) {		//fast test for collion, may be wrong
		simple_d = realDist(c1, c2);	

		if (simple_d < (r1 + r2))	//if real collision
			ret = true;
	}

	return ret;
}


bool CSimplePhysx::SphereVsBox(float r, VEC3 c, VEC3 pos_min, VEC3 pos_max)
{
	float real_x_min;
	float real_x_max;
	float real_y_min;
	float real_y_max;
	float real_z_min;
	float real_z_max;
	

	//get real points min & max:
	if (pos_min.y <= pos_max.y) {
		real_y_min = pos_min.y;
		real_y_max = pos_max.y;
	}
	else {
		real_y_max = pos_min.y;
		real_y_min = pos_max.y;
	}
	
	if (pos_min.z <= pos_max.z) {
		real_z_min = pos_min.z;
		real_z_max = pos_max.z;
	}
	else {
		real_z_min = pos_max.z;
		real_z_max = pos_min.z;
	}

	if (pos_min.x <= pos_max.x) {
		real_x_min = pos_min.x;
		real_x_max = pos_max.x;
	}
	else {
		real_x_min = pos_max.x;
		real_x_max = pos_min.x;
	}

	float dist_squared = r * r;
	if (c.x < real_x_min) dist_squared -= squared(c.x - real_x_min);
	else if (c.x > real_x_max) dist_squared -= squared(c.x - real_x_max);
	if (c.y < real_y_min) dist_squared -= squared(c.y - real_y_min);
	else if (c.y > real_y_max) dist_squared -= squared(c.y - real_y_max);
	if (c.z < real_z_min) dist_squared -= squared(c.z - real_z_min);
	else if (c.z > real_z_max) dist_squared -= squared(c.z - real_z_max);
	
	bool ret =  dist_squared > 0;

	return ret;
}