#ifndef INC_THROW_BOMB_H_
#define	INC_THROW_BOMB_H_

#include "components\comp_base.h"
#include "logic\aicontroller.h"

#include "handle\handle.h"

class CEntity;
class TCompTransform;
class TCompPhysics;

class CThrowBomb : public aicontroller, public TCompBase {
	____TIMER_DECLARE_(t_explode);
	//float t_explode = 2.5f;
	//float t_waiting = 0.0f;
	float rad_squared = 4.f;

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;
	TCompTransform * transform;
	TCompPhysics * physics;
	PxRigidDynamic * rd;
	float lmax, hmax, speed;
	VEC3 initial_pos, dir_throw;
	float lcurrent, hcurrent;
	bool nextState = false;
public:
	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}
	bool load(MKeyValue & atts);
	void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo() override;
	void update(float elapsed);

	//States
	void Born();
	void Idle();
	void Throwing();
	void Throwed();
	void Impacted();
	void Explode();
	void Dead();

	bool countDown();
	bool checkNextState(std::string new_st);
	void initThrow();
	void throwMovement();

	void onNextState(const TMsgActivate&);
	void onThrow(const TMsgThrow&);

	bool ImpactWhenBorn();
	void SendMsg();
};

#endif