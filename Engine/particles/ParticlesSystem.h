#ifndef	INC_PARTICLES_SYSTEM_H_
#define INC_PARTICLES_SYSTEM_H_

#include "components\comp_base.h"
#include "ParticleData.h"

using namespace physx;

class CParticleSystem : public TCompBase {

	PxParticleSystem *			m_pParticleSystem;
	PxParticleCreationData		m_particleData;

	int							m_numParticles;

	PxVec3						m_initial_pos;
	PxVec3						m_initial_velocity;

	PxU32*						m_pParticleValidity;
	PxU32						m_validParticleRange;

	PxParticleExt::IndexPool*	m_pIndexPool;

	const CTexture*				m_pTexture;
	

	TParticleData				m_particles;

	bool StepLifeTimeParticle(unsigned idx, float max_time, float dt);
	void SetParticleInitialValues(unsigned idx);

public:

	CParticleSystem() : m_pParticleSystem(nullptr), m_pParticleValidity(nullptr) {}
	~CParticleSystem() {}

	void stop() {
		m_particles.clear();

		m_pParticleSystem->release();
	}

	void render() {}	//not used
	void renderParticles();
	void init();
	bool CreateParticles(TParticleData& particles);
	void update(float elapsed);
	bool load(MKeyValue& atts) {
		(void)(atts);
		return true;
	}

	void renderInMenu();
};

#endif