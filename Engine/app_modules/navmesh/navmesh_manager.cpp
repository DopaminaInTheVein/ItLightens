#include "mcv_platform.h"
#include "navmesh_manager.h"

#include "recast/navmesh.h"
#include "recast/navmesh_query.h"
#include "logic/sbb.h"
#include "components/entity.h"
#include "components/entity_parser.h"
#include "components/comp_transform.h"
#include "components/comp_physics.h"

std::string CNavmeshManager::getPathNavmesh(std::string level_name)
{
	return "data/navmeshes/" + level_name + ".data";
}

void CNavmeshManager::recalcNavmesh(std::string level_name) {
	// GENERATE NAVMESH
	CNavmesh nav = SBB::readNavmesh();
	nav.build(getPathNavmesh(level_name));
	SBB::postNavmesh(nav);
	SBB::postBool(level_name, true);
}

void CNavmeshManager::readNavmesh(std::string level_name) {
	// GENERATE NAVMESH
	CNavmesh nav = SBB::readNavmesh();
	bool recalc = !nav.reload(getPathNavmesh(level_name));
	if (recalc) {
		recalcNavmesh(level_name);
	}
	else {
		SBB::postNavmesh(nav);
		SBB::postBool("navmesh", true);
	}
}

void CNavmeshManager::initNavmesh(std::string level_name)
{
	SBB::postBool("navmesh", false);
	std::string salaloc = getPathNavmesh(level_name);

	// GENERATE NAVMESH
	CNavmesh nav;
	nav.m_input.clearInput();
	dbg(" GENERANDO NAVMESH INPUT...\n");
	for (CHandle han : CEntityParser::getCollisionables()) {
		CEntity * e = han;
		dbg("Navmesh, collisionable: %s\n", e->getName());
		if (e) {
			TCompTransform * trans = e->get<TCompTransform>();
			TCompPhysics * p = e->get<TCompPhysics>();
			if (!p) continue;
			auto actor = p->getActor();
			if (!actor) continue;
			PxBounds3 bounds = actor->getWorldBounds();
			VEC3 min, max;
			min.x = bounds.minimum.x;
			min.y = bounds.minimum.y;
			min.z = bounds.minimum.z;
			max.x = bounds.maximum.x;
			max.y = bounds.maximum.y;
			max.z = bounds.maximum.z;

			auto rb = p->getActor()->isRigidStatic();
			if (rb) {
				int nBShapes = rb->getNbShapes();
				PxShape **ptr;
				ptr = new PxShape*[nBShapes];
				rb->getShapes(ptr, 1);
				for (int i = 0; i < nBShapes; i++) {
					PxTriangleMeshGeometry meshGeom;
					assert(ptr[i]);
					if (ptr[i]->getTriangleMeshGeometry(meshGeom)) {
						nav.m_input.addInput(meshGeom.triangleMesh, PhysxConversion::PxVec3ToVec3(rb->getGlobalPose().p), min, max, trans->getRotation());
					}
				}
			}
			else {
				nav.m_input.addInput(min, max);
			}
		}
	}
	nav.m_input.computeBoundaries();
	SBB::postNavmesh(nav);
	std::ifstream is(salaloc.c_str());
	bool recalc = !is.is_open();
	is.close();
	SBB::postBool(level_name, false);
	//------------------------------
	if (!recalc) {
		readNavmesh(level_name);
	}
	else {
		recalcNavmesh(level_name);
	}
}