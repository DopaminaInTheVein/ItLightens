#include "mcv_platform.h"
#include "GuardShots.h"
#include "resources/resources_manager.h"

std::vector<Shot> ShootManager::shots = std::vector<Shot>();
void ShootManager::shootLaser(VEC3 origin, CQuaternion quat, float reach)
{
	PROFILE_FUNCTION("ShootManager: shootLaser");
	MAT44 mat = MAT44::Identity;
	mat = MAT44::CreateScale(.1f, .1f, reach);
	mat *= MAT44::CreateFromQuaternion(quat);
	//mat *= MAT44::CreateLookAt(origin, dest, VEC3(0,1,0));
	mat.Translation(origin);
	shots.push_back(Shot(mat, reach));
}

void ShootManager::renderAll()
{
	PROFILE_FUNCTION("ShootManager: shootLaser");
	auto tech = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	tech->activate();

	const CMesh * unit_sphere = Resources.get("meshes/fx/laser.mesh")->as<CMesh>();
	unit_sphere->activate();

	for (Shot shot : shots) {
		activateWorldMatrix(shot.transform);
		unit_sphere->render();
	}
	shots.clear();
}
