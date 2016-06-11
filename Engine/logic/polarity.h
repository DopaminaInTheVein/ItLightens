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
	VEC3 offset;
	pols polarity;

	PolarityForce(float d, VEC3 dP, VEC3 offs, pols p) :
		distance(d),
		deltaPos(dP),
		offset(offs),
		polarity(p) {}
	PolarityForce() :
		distance(100.f),
		deltaPos(VEC3(0, 100, 0)),
		offset(VEC3(0.f, 0.f, 0.f)),
		polarity(NEUTRAL) {}
};

#endif