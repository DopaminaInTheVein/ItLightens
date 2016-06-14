#ifndef	INC_EMITTER_H_
#define INC_EMITTER_H_

#include "ParticlesSystem.h"
#include "ParticlesEmitter.h"

struct TEmitter
{
	TEmitter() : emitter(NULL), isEnabled(false), particleSystem(NULL) {}
	void update(float dt){}
	class CParticlesEmitter* emitter;
	bool isEnabled;
	CParticleSystem* particleSystem;
};

#endif