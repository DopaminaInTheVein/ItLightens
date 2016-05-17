#ifndef INC_MAGNET_DOOR_H_
#define INC_MAGNET_DOOR_H_

#include "components/components.h"
#include "geometry/geometry.h"
#include "logic/polarity.h"


class magnet_door : public TCompBase {
	enum eMagnetismBehaviour {
		MB_OPENING,
		MB_CLOSING,
		MB_NONE
	};

	enum eCinematicState {
		OPENED,
		CLOSED,
		OPENING,
		CLOSING
	};

	eMagnetismBehaviour prevBehaviour; //Magnetism Behaviour predomines. If MB_NONE --> openedLogic
	bool locked;
	bool moving;	//false false = opened, false true = opening
					//true false = closed, true true = closing 
	eMagnetismBehaviour magneticBehaviour, prevMagneticBehaviour;
	bool prevMoving;
	bool prevLocked;

	eCinematicState cinematicState, prevCinematicState;

	VEC3 targetOpened, targetClosed;
	pols polarity;
	float speedOpening;
	float speedClosing;

	//Update info
	TCompTransform * transform;
	TCompPhysics * physics;
	TCompTransform * playerTransform;
	pols playerPolarity;

	//Private Funcs
	bool getUpdateInfo();

public:
	void update(float elapsed);
	void onSetPolarity(const TMsgSetPolarity&);
	void onSetLocked(const TMsgSetLocked&);
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
};
#endif
