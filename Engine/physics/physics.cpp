#include "mcv_platform.h"
#include "physics.h"
#include "handle/handle_manager.h"
#include "comp_col_cylinder.h"

ray_cast_halfway Physics::RayCastHalfWay;

ray_cast_result Physics::calcRayCast(const ray_cast_query& query) {
	ray_cast_result res;
	RayCastHalfWay = ray_cast_halfway(query);
	getHandleManager<TCompColCillinder>()->onAll(&TCompColCillinder::rayCast);
	CHandle firstCollider = RayCastHalfWay.handle;
	if (firstCollider.isValid()) {
		res.firstCollider = CHandle(firstCollider).getOwner(); //Devolvemos directamente handle a entity
		res.positionCollision = RayCastHalfWay.posCollision;
	}
	return res;
}