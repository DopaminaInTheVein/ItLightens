#ifndef INC_PARTICLE_DATA_H_
#define	INC_PARTICLE_DATA_H_


#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;

struct TParticleData {
	std::vector<PxU32>		indexBuffer;
	std::vector<PxVec3>		positionBuffer;
	std::vector<PxVec3>		positionInitBuffer;
	std::vector<PxVec3>		velocityBuffer;
	std::vector<PxVec3>		velocityInitBuffer;
	std::vector<float>		lifeTimeBuffer;
	std::vector<float>		maxLifeTimeBuffer;

	PxU32	maxParticles;
	PxU32	numParticles;

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

		//lifetime
		lifeTimeBuffer.resize(maxParticles);
		maxLifeTimeBuffer.resize(maxParticles);
	}

	void clear() {
		indexBuffer.clear();
		positionBuffer.clear();
		velocityBuffer.clear();
		lifeTimeBuffer.clear();
	}
};

#endif