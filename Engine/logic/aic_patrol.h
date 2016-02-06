#ifndef _AIC_PATROL
#define _AIC_PATROL

#include "aicontroller.h"

class aic_patrol : public aicontroller
{
public:

	vector<VEC3> wpts;
	int curwpt;
	float distance_combat;
	int idle_war_wait;
	int combat_wait;

	void IdleState();
	void SeekWptState();
	void NextWptState();
	void ChaseState();
	void IdleWarState();
	void CombatState();
	void OLeftState();
	void ORightState();

	void Init(TEntity* new_entity);

	void Orbit(float angle);
};

#endif