#ifndef INC_POLARITY_H_
#define INC_POLARITY_H_

#include "geometry/geometry.h"

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

#endif