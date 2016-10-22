#ifndef INC_PARTICLE_DATA_H_
#define	INC_PARTICLE_DATA_H_

#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;

struct TParticleData {
	enum State {
		WAITING = 0,
		STARTED = 1,
		OFF = -1,
	};

	std::vector<PxU32>		indexBuffer;
	std::vector<PxVec3>		positionBuffer;
	std::vector<PxVec3>		positionInitBuffer;
	std::vector<PxVec3>		velocityBuffer;
	std::vector<PxVec3>		negativeVelocityBuffer;	//used to reset velocity
	std::vector<PxVec3>		velocityInitBuffer;

	std::vector<float>		sizeBuffer;

	std::vector<float>		currDelayStart;
	std::vector<State>		started;

	std::vector<VEC4>		colorBuffer;
	std::vector<VEC4>		colorOriginBuffer;

	std::vector<float>		lifeTimeBuffer;
	std::vector<float>		maxLifeTimeBuffer;

	PxU32	maxParticles;
	PxU32	numParticles;

	int max_frames;
	//#ifndef FINAL_BUILD
	//	int id_owner = 0;
	//#endif
	TParticleData() : maxParticles(0), numParticles(0) {}

	TParticleData(PxU32 _maxParticles)
	{
		initialize(_maxParticles);
	}

	void initialize(PxU32 _maxParticles)
	{
		maxParticles = _maxParticles;
		numParticles = 0;

		//index
		indexBuffer.resize(maxParticles);

		//positions
		positionBuffer.resize(maxParticles);
		positionInitBuffer.resize(maxParticles);

		//velocities
		velocityInitBuffer.resize(maxParticles);
		velocityBuffer.resize(maxParticles);
		negativeVelocityBuffer.resize(maxParticles);

		//lifetime
		lifeTimeBuffer.resize(maxParticles);
		maxLifeTimeBuffer.resize(maxParticles);

		//delay
		started.resize(maxParticles);
		currDelayStart.resize(maxParticles);

		//size
		sizeBuffer.resize(maxParticles);

		//color
		colorBuffer.resize(maxParticles);
		colorOriginBuffer.resize(maxParticles);
	}

	void clear() {
		indexBuffer.clear();
		positionBuffer.clear();
		velocityBuffer.clear();
		lifeTimeBuffer.clear();
		sizeBuffer.clear();
		started.clear();
		currDelayStart.clear();
		colorBuffer.clear();

		positionInitBuffer.clear();
		negativeVelocityBuffer.clear();
		velocityInitBuffer.clear();

		colorOriginBuffer.clear();

		maxLifeTimeBuffer.clear();
	}
};

#endif
