#ifndef INC_THROW_BOMB_H_
#define	INC_THROW_BOMB_H_

#include "components\comp_base.h"
#include "logic\aicontroller.h"

#include "handle\handle.h"

class CEntity;
class TCompTransform;
class TCompPhysics;

class CThrowBomb : public TCompBase {
	float t_explode = 2.5f;
	float t_waiting = 0.0f;
	float rad_squared = 4.f;

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;
	TCompTransform * transform;
	TCompPhysics * physics;
	PxRigidDynamic * rd;
	float front_offset, height_offset;
	float lmax, hmax, speed;
	VEC3 initial_pos, final_pos;
	float lcurrent, hcurrent;
	bool impact = false;
	bool exploded = false;
public:
	bool load(MKeyValue & atts);
	void Init(float lmax, float hmax);
	void CountDown();
	void Explode();

	void UpdatePosition();
	bool getUpdateInfo() override;
	void update(float elapsed);
	void onImpact(const TMsgActivate&);
	bool ImpactWhenBorn();
	void SendMsg();
};

#endif