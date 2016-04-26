#ifndef	INC_POLARIZED_H_
#define INC_POLARIZED_H_

#include "comp_base.h"

struct TMsgPolarize;
class CHandle;

enum pols {
	NEUTRAL = 0,
	MINUS,
	PLUS,
};

struct PolarityForce {
	float distance;
	VEC3 deltaPos;
	pols polarity;
	PolarityForce(float d, VEC3 dP, pols p) :
		distance(d),
		deltaPos(dP),
		polarity(p) {}
	PolarityForce() :
		distance(100.f),
		deltaPos(VEC3(0, 100, 0)),
		polarity(NEUTRAL) {}
};

struct TCompPolarized : public TCompBase {
	enum type {
		FIXED = 0,
		FREE
	};

	VEC3				origin;
	bool				send					= false;
	bool				moving					= false;
	VEC3				last_position;

	TMsgPolarize		msg_in;
	TMsgPolarize		msg_out;
	CHandle			player_h;

	float			dist_effect_squared	= 25.0f;
	float			dist_near			= 3.0f;
	PolarityForce	force;
	//float			dist_player = 1000.f; // Update each frame
	//VEC3			deltaPos_player = VEC3(1000.f, 1000.f, 1000.f); //Update each frame
	//int				mPol				= NEUTRAL;

	float			mThresholdMass		= 2.5f;

	int				mType				= FIXED;
	int				mPlayer_state		= NEUTRAL;

	void init();
	void update(float elapsed);
	bool load(MKeyValue& atts);

	void onCreate(const TMsgEntityCreated&);

	void onPolarize(const TMsgPlayerPolarize& msg);

	void sendMessagePlayer(const TMsgPolarize& msg);

	//Get Force to Player
	PolarityForce getForce();
};

#endif