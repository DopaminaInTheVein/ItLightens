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
		CS_OPENED,
		CS_CLOSED,
		CS_OPENING,
		CS_CLOSING
	};

	bool locked;
	eMagnetismBehaviour magneticBehaviour;
	eCinematicState cinematicState, prevCinematicState;

	VEC3 targetOpened, targetClosed;
	pols polarity;
	float speedOpening;
	float speedClosing;
	float distPolarity;
	float epsilonTarget;

	//Update info
	CHandle myEntity;
	TCompTransform * transform;
	TCompPhysics * physics;
	TCompTransform * playerTransform;
	pols playerPolarity;

	//Private Funcs
	void updateMagneticBehaviour();
	void updateCinematicState();
	void updateMove();
	void notifyNewState();

public:
	bool getUpdateInfo() override;
	void update(float elapsed);
	void onSetPolarity(const TMsgSetPolarity&);
	void onSetLocked(const TMsgSetLocked&);
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
};
#endif
