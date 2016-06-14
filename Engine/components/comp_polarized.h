#ifndef	INC_POLARIZED_H_
#define INC_POLARIZED_H_

#include "comp_base.h"
#include "logic/polarity.h"

struct TMsgPolarize;
class CHandle;

struct TCompPolarized : public TCompBase {
	enum type {
		FIXED = 0,
		FREE,
	};

	VEC3				origin;
	bool				send = false;
	bool				moving = false;
	VEC3				last_position;

	TMsgPolarize		msg_in;
	TMsgPolarize		msg_out;
	CHandle				player_h;

	float			dist_effect_squared_free = 25.f;
	float			dist_effect_squared_fixed = 4.f;
	float			dist_near = 5.f;

	PolarityForce	force;

	float			mThresholdMass = 2.5f;
	float			mEpsilonMove = 0.01f;

	int				mType = FIXED;
	int				mPlayer_state = NEUTRAL;

	void init();
	bool getUpdateInfo() override;
	void update(float elapsed);

	bool load(MKeyValue& atts);

	void onCreate(const TMsgEntityCreated&);

	void onPolarize(const TMsgPlayerPolarize& msg);

	void sendMessagePlayer(const TMsgPolarize& msg);

	//Get Force to Player
	PolarityForce getForce();
};

#endif