#ifndef	INC_PARTICLES_MANAGER_H_
#define INC_PARTICLES_MANAGER_H_

#include "app_modules\app_module.h"
#include "ParticlesSystem.h"

class CParticlesManager : public IAppModule {
  std::vector<CParticleSystem*>		m_Particles;

  const CRenderTechnique*			m_pTechniqueParticles;
  bool								m_particleEditor;

  CParticleSystem*					m_pNewParticleSystem;

public:

  virtual bool start();
  virtual void stop() {
	  for (auto& p : m_Particles) {
		  p->stop();
	  }
	  //if (m_pNewParticleSystem) m_pNewParticleSystem->stop();
    m_Particles.clear();
  }
  virtual void update(float dt);

  //call render for each particle
  void renderParticles();

  //Add particlesSystem to the list of particles
  void AddParticlesSystem(CParticleSystem* particle_system);

  //delete specific particles system
  void DeleteParticleSytem(CParticleSystem* particle_system);


  bool* GetParticleEditorState() {
	  return &m_particleEditor;
  }

  void LoadParticlesSystems() {}

  //particles editor
  void RenderParticlesEditor();

  //debug info
  void renderInMenu();

  //only to render particle system editor
  virtual void render() {
	  if (m_particleEditor)
		  RenderParticlesEditor();
  }
  //name for module
  const char* getName() const {
    return "particles";
  }
};

extern CParticlesManager *g_particlesManager;

#endif