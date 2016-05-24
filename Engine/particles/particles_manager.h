#ifndef	INC_PARTICLES_MANAGER_H_
#define INC_PARTICLES_MANAGER_H_

#include "app_modules\app_module.h"
#include "ParticlesSystem.h"

class CParticlesManager : public IAppModule {
  std::vector<CParticleSystem*>	m_Particles;

  const CRenderTechnique*			m_pTechniqueParticles;

public:

  virtual bool start();
  virtual void stop() {
    m_Particles.clear();
  }
  virtual void update(float dt);

  //useless empty fuction for particles
  virtual void render() {} //will not be used, instead use renderParticles, called and controlled by render_deferred

  //call render for each particle
  void renderParticles();

  //Add particlesSystem to the list of particles
  void AddParticlesSystem(CParticleSystem* particle_system);

  //delete specific particles system
  void DeleteParticleSytem(CParticleSystem* particle_system);

  //debug info
  void renderInMenu();

  //name for module
  const char* getName() const {
    return "particles";
  }
};

extern CParticlesManager *g_particlesManager;

#endif