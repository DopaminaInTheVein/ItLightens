#include "mcv_platform.h"
#include "particles_manager.h"

#include "render/render.h"
#include "render/render_instanced.h"
#include "render/draw_utils.h"
#include "render/mesh.h"
#include "resources/resources_manager.h"

CParticlesManager g_particlesManager;

bool CParticlesManager::start()
{
	m_pTechniqueParticles = Resources.get("particles.tech")->as<CRenderTechnique>();
	return true;
}

void CParticlesManager::update(float dt)
{
	for (auto& particles : m_Particles) {
		particles.update(dt);
	}
}

void CParticlesManager::renderParticles()
{

	CTraceScoped scope("ParticlesManager: RenderParticles");
	activateWorldMatrix(MAT44::Identity);
	m_pTechniqueParticles->activate();
	activateBlend(BLENDCFG_COMBINATIVE);
	activateZ(ZCFG_TEST_BUT_NO_WRITE);

	for (auto& particles : m_Particles) {
		particles.renderParticles();
	}

	activateZ(ZCFG_DEFAULT);
	activateBlend(BLENDCFG_DEFAULT);
}

void CParticlesManager::AddParticlesSystem(const CParticleSystem & particle_system)
{
	m_Particles.push_back(particle_system);
}

void CParticlesManager::DeleteParticleSytem(CParticleSystem & particle_system)
{
}
