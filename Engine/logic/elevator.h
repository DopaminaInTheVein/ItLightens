#ifndef INC_ELEVATOR_H_
#define INC_ELEVATOR_H_

#include "components/components.h"
#include "geometry/geometry.h"
#include "logic/polarity.h"

class elevator : public TCompBase {
	enum eElevatorState {
		UP,
		GOING_UP,
		DOWN,
		GOING_DOWN
	};

	VEC3 targetUp, targetDown;
	float speedUp;
	float speedDown;
	float epsilonTarget;
	float lastSpeed = 0.0f;
	int state_init = -1;

	eElevatorState prevState;

	//Update info
	CHandle myEntity;
	TCompTransform * transform;
	TCompPhysics * physics;

	//Private Funcs
	void updateMove();
	void notifyNewState();

public:
	eElevatorState state;
	bool getUpdateInfo();
	void update(float elapsed);
	void onElevatorAction(const TMsgActivate&);
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
};
#endif
