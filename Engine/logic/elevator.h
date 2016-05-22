#ifndef INC_ELEVATOR_H_
#define INC_ELEVATOR_H_

#include "components/components.h"
#include "geometry/geometry.h"
#include "logic/polarity.h"


class elevator : public TCompBase {
	
	VEC3 targetUp, targetDown;
	float speedUp;
	float speedDown;
	float epsilonTarget;

	//Update info
	CHandle myEntity;
	TCompTransform * transform;
	TCompPhysics * physics;

	//Private Funcs
	bool getUpdateInfo();
	void updateCinematicState();
	void updateMove();
	void notifyNewState();

public:
	void update(float elapsed);
	//void onElevatorAction(const TMsgElevator&);
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
};
#endif
