#include "mcv_platform.h"
#include "GuardShots.h"
#include "resources/resources_manager.h"

std::vector<Shot> ShootManager::shots = std::vector<Shot>();
void ShootManager::shootLaser(const VEC3& origin, const VEC3& reach)
{
	PROFILE_FUNCTION("ShootManager: shootLaser");
	/*MAT44 mat = MAT44::Identity;
	mat = MAT44::CreateScale(.1f, .1f, reach);
	mat *= MAT44::CreateFromQuaternion(quat);
	//mat *= MAT44::CreateLookAt(origin, dest, VEC3(0,1,0));
	mat.Translation(origin);*/
	shots.push_back(Shot(origin, reach));
}

void ShootManager::renderAll()
{
	PROFILE_FUNCTION("ShootManager: shootLaser");

	//const CMesh * unit_sphere = Resources.get("meshes/fx/laser.mesh")->as<CMesh>();
	//unit_sphere->activate();

	

	//Shot new_shot = Shot(MAT44::Identity, 1);
	//shots.push_back(new_shot);

	for (Shot shot : shots) {
		//activateWorldMatrix(shot.transform);
		//unit_sphere->render();

		drawLine(shot.origin, shot.end, VEC4(1, 0, 0, 1));
	}
	//shots.clear();
}
