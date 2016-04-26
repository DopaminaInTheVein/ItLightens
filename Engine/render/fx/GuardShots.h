#ifndef INC_DRAW_GUARD_SHOTS_H_
#define INC_DRAW_GUARD_SHOTS_H_

#include "render/draw_utils.h"

// TODO: ShootManager debería ser otra clase aparte!
struct Shot {
	MAT44 transform;
	float reach;
	Shot(MAT44 t, float r) : transform(t), reach(r) {}
};

class ShootManager
{
public:
	static void shootLaser(VEC3 origin, CQuaternion quat, float reach);
	static void renderAll();
private:
	static std::vector<Shot> shots;
};


#endif