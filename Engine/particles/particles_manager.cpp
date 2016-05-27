#include "mcv_platform.h"
#include "particles_manager.h"

#include "render/render.h"
#include "render/render_instanced.h"
#include "render/draw_utils.h"
#include "render/mesh.h"
#include "components\entity.h"
#include "resources/resources_manager.h"
#include "components\comp_name.h"

#include "app_modules\imgui\module_imgui.h"

CParticlesManager *g_particlesManager;

bool CParticlesManager::start()
{
  m_pTechniqueParticles = Resources.get("particles.tech")->as<CRenderTechnique>();
  m_particleEditor = false;
  m_pNewParticleSystem = nullptr;

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
#ifdef _DEBUG
	if (io->keys[VK_F10].becomesPressed()) {
		m_particleEditor = !m_particleEditor;
	}
#endif
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

void CParticlesManager::RenderParticlesEditor()
{
	ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiSetCond_FirstUseEver);
	if (m_particleEditor) {
		static std::string file = "NOT_LOAD";
		static std::string new_file = "default_particles";
		ImGui::Begin("ParticleEditor", &m_particleEditor);

		if (ImGui::SmallButton("New Particles System")) {

		}
		
		ImGui::SameLine();
		if (ImGui::SmallButton("Load Particles System")) {
			file = CImGuiModule::getFilePath();
		}

		if (file != "NOT_LOAD") {
			ImGui::Text("File loaded from %s\n", file.c_str());
		}



		

		if (m_pNewParticleSystem) {

			ImGui::Separator();

			m_pNewParticleSystem->renderInMenu();

			ImGui::Separator();

			if (ImGui::SmallButton("Save Particles System")) {
				//TODO: save particles system to file
			}
		}

		ImGui::End();
	}
}

void CParticlesManager::renderInMenu()
{
  ImGui::Text("num particles system = %d\n", m_Particles.size());

  ImGui::Separator();

  if (ImGui::TreeNode("Particles systems list")) {
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