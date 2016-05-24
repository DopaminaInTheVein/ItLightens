#include "mcv_platform.h"
#include "particles_manager.h"

#include "render/render.h"
#include "render/render_instanced.h"
#include "render/draw_utils.h"
#include "render/mesh.h"
#include "components\entity.h"
#include "resources/resources_manager.h"
#include "components\comp_name.h"

CParticlesManager *g_particlesManager;

bool CParticlesManager::start()
{
  m_pTechniqueParticles = Resources.get("particles.tech")->as<CRenderTechnique>();

  /*auto hs = tags_manager.getHandlesByTag("particles");

  for (CEntity* e : hs) {
    CParticleSystem* ps = e->get<CParticleSystem>();
    if (ps)
      m_Particles.push_back(ps);
  }*/

  return true;
}

void CParticlesManager::update(float dt)
{
  for (auto& particles : m_Particles) {
    particles->update(dt);
  }
}

void CParticlesManager::renderParticles()
{
  CTraceScoped scope("ParticlesManager: RenderParticles");
  activateWorldMatrix(MAT44::Identity);
  m_pTechniqueParticles->activate();      //default technique for particles
  activateBlend(BLENDCFG_COMBINATIVE);
  activateZ(ZCFG_TEST_BUT_NO_WRITE);

  for (auto& particles : m_Particles) {
    particles->renderParticles();
  }

  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);
}

void CParticlesManager::AddParticlesSystem(CParticleSystem * particle_system)
{
  m_Particles.push_back(particle_system);
}

void CParticlesManager::DeleteParticleSytem(CParticleSystem * particle_system)
{
}

void CParticlesManager::renderInMenu()
{
  ImGui::Text("num particles system = %d\n", m_Particles.size());

  ImGui::Separator();

  ImGui::Text("Add particles, only temporal particles");
  if (ImGui::SmallButton("Add particles(doesnt work yet)")) {
    //nothing
  }

  ImGui::Separator();

  if (ImGui::TreeNode("Particles systems")) {
    ImGui::Text("all particles systems...");
    for (auto& p : m_Particles) {
      std::string name;
      CHandle h = CHandle(p).getOwner();

      if (h.isValid()) {  //handle valid, search for name
        CEntity * e = h;
        TCompName* name_component = e->get<TCompName>();
        name = name_component->name;
      }
      else {    //handle invalid, default name
        name = "default name";
      }
      if (ImGui::TreeNode(name.c_str())) {
        p->renderInMenu();
        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }
}