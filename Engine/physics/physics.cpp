#include "mcv_platform.h"
#include "physics.h"
#include "colliders.h"
#include "handle/handle_manager.h"
#include "comp_col_cylinder.h"

ray_cast_halfway Physics::RayCastHalfWay;

ray_cast_result Physics::calcRayCast(const ray_cast_query& query) {
	ray_cast_result res;
	RayCastHalfWay = ray_cast_halfway(query);
	RayCastHalfWay.query.direction.Normalize();
	getHandleManager<TCompColCillinder>()->onAll(&TCompColCillinder::rayCast);
	getHandleManager<boxCollider>()->onAll(&boxCollider::rayCast);
	CHandle firstCollider = RayCastHalfWay.handle;
	if (firstCollider.isValid()) {
		res.firstCollider = firstCollider;
		res.positionCollision = RayCastHalfWay.posCollision;
	}
	return res;
}