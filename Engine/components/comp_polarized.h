#ifndef	INC_POLARIZED_H_
#define INC_POLARIZED_H_

#include "comp_base.h"

struct TMsgPolarize;
class CHandle;

struct TCompPolarized : public TCompBase {

	enum pols {
		NEUTRAL = 0,
		MINUS,
		PLUS,
	};

	enum type {
		FIXED = 0,
		FREE,
	};

	VEC3				origin;
	bool				send					= false;

	TMsgPolarize		msg_in;
	TMsgPolarize		msg_out;
	CHandle			player_h;

	float			dist_effect_squared	= 25.0f;
	float			dist_near			= 3.0f;

	int				mPol					= NEUTRAL;
	int				mType				= FIXED;
	int				mPlayer_state		= NEUTRAL;

	void init();
	void update(float elapsed);
	bool load(MKeyValue& atts);

	void onCreate(const TMsgEntityCreated&);

	void onPolarize(const TMsgPlayerPolarize& msg);

	void sendMessagePlayer(const TMsgPolarize& msg);
};

#endif