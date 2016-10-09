#ifndef INC_DRAW_GUARD_SHOTS_H_
#define INC_DRAW_GUARD_SHOTS_H_

#include "render/draw_utils.h"

// TODO: ShootManager debería ser otra clase aparte!
struct Shot {
	//MAT44 transform;
	//float reach;
	//Shot(MAT44 t, float r) : transform(t), reach(r) {}

	VEC3 origin;
	VEC3 end;

	Shot(const VEC3& src, const VEC3& dst) : origin(src), end(dst) {}
};

class ShootManager
{
public:
	//static void shootLaser(VEC3 origin, CQuaternion quat, float reach);
	static void shootLaser(const VEC3& origin, const VEC3& reach);
	static void renderAll();
//private:
	static std::vector<Shot> shots;
};


#endif