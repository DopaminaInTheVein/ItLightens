#include "mcv_platform.h"
#include "particles_manager.h"

#include "render/render.h"
#include "render/render_instanced.h"
#include "render/draw_utils.h"
#include "render/mesh.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "resources/resources_manager.h"
#include "components\comp_name.h"

#include "app_modules\imgui\module_imgui.h"
#include "components\entity_parser.h"

#define SECURE_PART(handle, code) if (handle.isValid()) { ((CParticleSystem*)handle)##code;}

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
	for (auto particles : m_Particles) {
		SECURE_PART(particles, ->update(dt));
		if (particles.isValid()) {
			((CParticleSystem*)particles)->update(dt);
		}
	}
#ifndef FINAL_BUILD
	if (controller->isParticleEditorActivationPressed()) {
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
		SECURE_PART(particles, ->renderParticles());
		//particles->renderParticles();
	}

	activateZ(ZCFG_ALL_DISABLED);
	activateBlend(BLENDCFG_DEFAULT);
}

void CParticlesManager::AddParticlesSystem(CParticleSystem * particle_system)
{
	CHandle to_add = CHandle(particle_system);
	if (to_add.isValid()) m_Particles.push_back(to_add);
}
void CParticlesManager::stop() {
	if (m_Particles.size() > 0) {
		for (int i = 0; i < m_Particles.size(); i++) {
			SECURE_PART(m_Particles[i], ->stop());
		}
		//if (m_pNewParticleSystem) m_pNewParticleSystem->stop();
		m_Particles.clear();
	}
}
void CParticlesManager::DeleteParticleSytem(CParticleSystem * particle_system)
{
	CHandle to_remove = CHandle(particle_system);
	if (to_remove.isValid()) removeFromVector(m_Particles, to_remove);
}

void CParticlesManager::RenderParticlesEditor()
{
	//local variables for editor
	static char name_file[64] = "default_name";

	static int time_autosave = 0;
	static int num_autosave = 3;

	static std::string file = "NOT_LOAD";
	static std::string new_file = "default_particles";

	ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiSetCond_FirstUseEver);
	if (m_particleEditor) {
		ImGui::Begin("ParticleEditor", &m_particleEditor);

		if (ImGui::SmallButton("New Particles System")) {
			CEntity* e_particles = spawnPrefab("particles_default");
			if (!e_particles) {
				fatal("Particles couldnt be created");
				return;
			}
			m_pNewParticleSystem = e_particles->get<CParticleSystem>();
			m_pNewParticleSystem->init();
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("Load Particles System")) {
			file = CImGuiModule::getFilePath();
			auto p = file.find_last_of("\\");		//get last part from path
			file = file.substr(p + 1);

			if (file != "") {		//valid entry
				file = "data/particles/" + file;
			}

			bool is_ok = m_pNewParticleSystem->loadFromFile(file);
			if (!is_ok) {
				file = "NOT_LOAD";
			}
		}

		if (file != "NOT_LOAD") {
			ImGui::Text("File loaded from %s\n", file.c_str());
		}

		//if new particles system, menu to edit & save particles system
		if (m_pNewParticleSystem) {
			ImGui::Separator();

			m_pNewParticleSystem->renderInMenu();

			ImGui::Separator();

			ImGui::Text("\n");
			ImGui::Text("SAVE DATA");
			ImGui::Text("\n");

			ImGui::InputText("file name: ", name_file, 64);

			ImGui::SameLine();
			if (ImGui::SmallButton("Save Particles System")) {
				//CApp::get().loadMode(CApp::ePARTICLES_EDITOR);
				m_pNewParticleSystem->saveToFile(name_file);
			}

			ImGui::Text("Time in minutes. If time is less than 1, autosave desactivated\nNumber autosaves, default 3");
			ImGui::DragInt("autosave(min): ", &time_autosave);
			ImGui::DragInt("number autosaves: ", &num_autosave);
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
				SECURE_PART(p, ->renderInMenu());
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}