#include "mcv_platform.h"
#include "editor_lights.h"

#include <fstream>

#include "handle\handle_manager.h"
#include "components\entity.h"
#include "components\comp_name.h"
#include "components\comp_transform.h"
#include "components\comp_culling.h"
#include "components\comp_room.h"

#include "components\entity_parser.h"

//lights includes
#include "components\comp_light_dir.h"
#include "components\comp_light_dir_shadows.h"
#include "components\comp_light_point.h"

static int id_name = 0;	//id used to create new unique lights

void CEditorLights::update(float dt) {
	if (controller->isEditorLightsButtonPressed()) {
		m_activated_editor = !m_activated_editor;
	}
}

//Load all the lights on the scene at game start
bool CEditorLights::LoadLights()
{
	CEditorLights* me = this;

	//shadow lights
	getHandleManager<TCompLightPoint>()->each([me](TCompLightPoint* c) {
		CHandle h_light = CHandle(c);
		me->AddLightToSave(h_light, TypeLight::POINT);
	});

	//dir lights
	getHandleManager<TCompLightDir>()->each([me](TCompLightDir* c) {
		CHandle h_light = CHandle(c);
		me->AddLightToSave(h_light, TypeLight::DIR);
	});

	//shadow lights
	getHandleManager<TCompLightDirShadows>()->each([me](TCompLightDirShadows* c) {
		CHandle h_light = CHandle(c);
		me->AddLightToSave(h_light, TypeLight::DIR_SHADOWS);
	});

	return true;
}

bool CEditorLights::SaveLights(std::string fileName)
{
	if (fileName == DEFAULT_LIGHTS) {
		//read level name as lightfilename

		CApp& app = CApp::get();
		fileName = app.getCurrentRealLevel();
		fileName += "_lights";
	}

	std::string full_path = "data/scenes/" + fileName + ".xml";

	MKeyValue atts;

	//file buffer
	std::ofstream os(full_path.c_str());

	atts.writeStartElement(os, "entities");

	for (int idx = 0; idx < m_Lights.size(); idx++) {
		CHandle h_owner = m_Lights[idx].getOwner();
		GET_COMP(lp, h_owner, TCompLightPoint);
		GET_COMP(ld, h_owner, TCompLightDir);
		GET_COMP(lds, h_owner, TCompLightDirShadows);
		if (lp || ld || lds) {
			((CEntity *)(h_owner))->save(os, atts);
		}
	}

	atts.writeEndElement(os, "entities");
	os.close();
	return true;
}

bool CEditorLights::AddLightToSave(CHandle h, TypeLight type)
{
	if (!h.isValid()) return false;
	RemoveLight(h, m_LigthsTemp, m_TypesTemp);
	m_Lights.push_back(h);
	m_Types.push_back(type);
	return true;
}

bool CEditorLights::AddLightToEngine(TypeLight type, bool* rooms)
{
	CHandle h;

	if (type == TypeLight::POINT) {
		h = spawnPrefab("light_point_default");
		CEntity* e_owner = h;
		h = e_owner->get<TCompLightPoint>();
		if (!h.isValid()) return false;
		TCompName* name = e_owner->get<TCompName>();
		std::string sname = "point_light";
		sname += std::to_string(id_name);
		name->setName(sname);
	}
	else if (type == TypeLight::DIR) {
		h = spawnPrefab("light_dir_default");
		CEntity* e_owner = h;
		h = e_owner->get<TCompLightDir>();
		if (!h.isValid()) return false;
		TCompName* name = e_owner->get<TCompName>();
		std::string sname = "light_dir";
		sname += std::to_string(id_name);
		name->setName(sname);
	}
	else if (type == TypeLight::DIR_SHADOWS) {
		h = spawnPrefab("light_dir_shadows_default");
		CEntity* e_owner = h;
		h = e_owner->get<TCompLightDirShadows>();
		if (!h.isValid()) return false;
		TCompName* name = e_owner->get<TCompName>();
		std::string sname = "light_dir_shadow";
		sname += std::to_string(id_name);
		name->setName(sname);
	}
	else {
		//nothing to do
		return false;
	}

	//Position
	GET_COMP(tmx_light, h.getOwner(), TCompTransform);
	if (tmx_light) {
		CHandle hcam = tags_manager.getFirstHavingTag("camera_main");
		if (hcam.isValid()) {
			GET_COMP(tmx_cam, hcam, TCompTransform);
			tmx_light->setPosition(tmx_cam->getPosition() + tmx_cam->getFront());
		}
	}

	//Rooms
	std::set<int> rooms_selected = std::set<int>();
	for (int i = 0; i < 10; i++) {
		if (rooms[i]) rooms_selected.insert(i);
	}
	if (rooms_selected.size() > 0) {
		auto hm = CHandleManager::getByName("room");
		CHandle new_h = hm->createHandle();
		TCompRoom* room_comp = new_h;
		for (int r : rooms_selected) {
			room_comp->name.push_back(r);
		}
		((CEntity*)(h.getOwner()))->add(new_h);
	}

	m_LigthsTemp.push_back(h);
	m_TypesTemp.push_back(type);

	id_name++;
	return true;
}

//return the index of an object from a vector
template<class T>
int getIndex(std::vector<T> vec, T obj) {
	int idx = 0;
	for (auto it : vec) {
		if (it == obj) return idx;
		idx++;
	}
	return -1;
}

bool CEditorLights::RemoveLight(CHandle h, std::vector<CHandle>& v_lights, std::vector<TypeLight>& v_types)
{
	int idx = getIndex(v_lights, h);
	if (idx < 0) return false;
	v_lights.erase(v_lights.begin() + idx);
	v_types.erase(v_types.begin() + idx);

	return true;
}

bool CEditorLights::HideLight(CHandle h)
{
	CEntity* e_owner = h.getOwner();
	if (!e_owner) return false;

	TCompLightPoint* isPointLight = e_owner->get<TCompLightPoint>();

	if (isPointLight) {
		isPointLight->enabled = !isPointLight->enabled;
		return true;
	}

	TCompLightDir* isLightDir = e_owner->get<TCompLightDir>();

	if (isLightDir) {
		isLightDir->enabled = !isLightDir->enabled;
		return true;
	}

	TCompLightDirShadows* isLightDirShadow = e_owner->get<TCompLightDirShadows>();

	if (isLightDirShadow) {
		isLightDirShadow->enabled = !isLightDirShadow->enabled;
		return true;
	}

	return false;
}

void CEditorLights::SetRenderDebug(bool value, std::vector<CHandle> v_lights, std::vector<TypeLight> v_types) {
	for (int idx = 0; idx < v_lights.size(); idx++) {
		if (v_types[idx] == TypeLight::POINT) {
			TCompLightPoint *lp = v_lights[idx];
			if (lp) {
				lp->debug_render = value;
			}
		}
		else if (v_types[idx] == TypeLight::DIR) {
			TCompLightDir *lp = v_lights[idx];
			if (lp) {
				lp->debug_render = value;
			}
		}
		else if (v_types[idx] == TypeLight::DIR_SHADOWS) {
			TCompLightDirShadows *lp = v_lights[idx];
			if (lp) {
				lp->debug_render = value;
			}
		}
	}
}

void CEditorLights::renderLightPoint(TCompLightPoint* pl) {
	pl->renderInMenu();
	bool hidden = !pl->enabled;
	if (ImGui::Checkbox("hide", &hidden)) {
		pl->enabled = !hidden;
	}
	if (ImGui::SmallButton("Destroy")) {
		CHandle light_handle = CHandle(pl);
		RemoveLight(light_handle, m_Lights, m_Types);
		RemoveLight(light_handle, m_LigthsTemp, m_TypesTemp);
		light_handle.destroy();
	}
}

void CEditorLights::renderLightDir(TCompLightDir* pl) {
	pl->renderInMenu();
	bool hidden = !pl->enabled;
	if (ImGui::Checkbox("hide", &hidden)) {
		pl->enabled = !hidden;
	}
	if (ImGui::SmallButton("Destroy")) {
		CHandle light_handle = CHandle(pl);
		RemoveLight(light_handle, m_Lights, m_Types);
		RemoveLight(light_handle, m_LigthsTemp, m_TypesTemp);
		light_handle.destroy();
	}
}
void CEditorLights::renderLightDirShadows(TCompLightDirShadows* pl) {
	pl->renderInMenu();
	bool hidden = !pl->enabled;
	if (ImGui::Checkbox("hide", &hidden)) {
		pl->enabled = !hidden;
	}
	if (ImGui::SmallButton("Destroy")) {
		CHandle light_handle = CHandle(pl);
		RemoveLight(light_handle, m_Lights, m_Types);
		RemoveLight(light_handle, m_LigthsTemp, m_TypesTemp);
		light_handle.destroy();
	}
}

void CEditorLights::RenderInMenu()
{
	static bool rooms_selected[10] = { 0 };
	ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiSetCond_FirstUseEver);
	if (m_activated_editor) {
		ImGui::Begin("LightsEditor", &m_activated_editor);

		if (ImGui::Checkbox("show axis", &m_show_axis)) {
			SetRenderDebug(m_show_axis, m_Lights, m_Types);
			SetRenderDebug(m_show_axis, m_LigthsTemp, m_TypesTemp);
		}

		if (ImGui::SmallButton("Save Lights")) {
			SaveLights();
		}

		if (ImGui::SmallButton("Reload Lights")) {
			LoadLights();
		}

		//To load a specific file
		if (ImGui::SmallButton("Load lights file")) {
			//TODO
		}
		ImGui::Separator();
		for (int room : TCompRoom::all_rooms) {
			char text_check[64];
			sprintf(text_check, "Room %d", room);
			ImGui::Checkbox(text_check, &rooms_selected[room]);
		}
		if (ImGui::Button("Add new point light")) {
			AddLightToEngine(TypeLight::POINT, rooms_selected);
		}

		if (ImGui::Button("Add new dir light")) {
			AddLightToEngine(TypeLight::DIR, rooms_selected);
		}

		if (ImGui::Button("Add new dir shadows light")) {
			AddLightToEngine(TypeLight::DIR_SHADOWS, rooms_selected);
		}

		ImGui::Separator();
		ImGui::Text("Engine lights:");

		//permanent lights

		//lights map loop
		for (int idx = 0; idx < m_Lights.size(); ++idx)
		{
			if (m_Types[idx] == TypeLight::POINT) {
				CHandle h_owner = m_Lights[idx].getOwner();
				CEntity* e_owner = h_owner;
				if (!e_owner) continue;	//handle not valid

				TCompName* name = e_owner->get<TCompName>();
				TCompLightPoint* light_point = m_Lights[idx];
				TCompTransform* trans = e_owner->get<TCompTransform>();

				if (light_point) {
					if (ImGui::TreeNode(name->name)) {
						name->renderInMenu();
						trans->renderInMenu();
						renderLightPoint(light_point);

						ImGui::TreePop();
					}
				}
			}
			else if (m_Types[idx] == TypeLight::DIR) {
				CHandle h_owner = m_Lights[idx].getOwner();
				CEntity* e_owner = h_owner;
				if (!e_owner) continue;	//handle not valid

				TCompTransform* trans = e_owner->get<TCompTransform>();
				TCompName* name = e_owner->get<TCompName>();

				TCompLightDir* light_dir = m_Lights[idx];

				if (light_dir) {
					if (ImGui::TreeNode(name->name)) {
						name->renderInMenu();
						trans->renderInMenu();
						renderLightDir(light_dir);
						ImGui::TreePop();
					}
				}
			}
			else if (m_Types[idx] == TypeLight::DIR_SHADOWS) {
				CHandle h_owner = m_Lights[idx].getOwner();
				CEntity* e_owner = h_owner;
				if (!e_owner) continue;	//handle not valid

				TCompName* name = e_owner->get<TCompName>();

				TCompTransform* trans = e_owner->get<TCompTransform>();
				TCompLightDirShadows* light_dir_shadows = m_Lights[idx];

				if (light_dir_shadows) {
					if (ImGui::TreeNode(name->name)) {
						name->renderInMenu();
						trans->renderInMenu();
						renderLightDirShadows(light_dir_shadows);
						ImGui::TreePop();
					}
				}
			}
			else {
				//nothing
			}
		}

		//Temporal lights

		ImGui::Separator();
		ImGui::Text("Temporal lights (not saved in the engine):");

		//lights map loop
		for (int idx = 0; idx < m_LigthsTemp.size(); ++idx)
		{
			if (m_TypesTemp[idx] == TypeLight::POINT) {
				CHandle h_owner = m_LigthsTemp[idx].getOwner();
				CEntity* e_owner = h_owner;
				if (!e_owner) continue;	//handle not valid

				TCompName* name = e_owner->get<TCompName>();
				TCompLightPoint* light_point = m_LigthsTemp[idx];
				TCompTransform* trans = e_owner->get<TCompTransform>();

				if (light_point) {
					if (ImGui::TreeNode(name->name)) {
						name->renderInMenu();
						trans->renderInMenu();
						light_point->renderInMenu();
						bool hidden = !light_point->enabled;
						if (ImGui::Checkbox("hide", &hidden)) {
							light_point->enabled = !hidden;
						}
						if (ImGui::SmallButton("Add as permanent")) {
							AddLightToSave(m_LigthsTemp[idx], m_TypesTemp[idx]);
						}
						ImGui::TreePop();
					}
				}
			}
			else if (m_TypesTemp[idx] == TypeLight::DIR) {
				CHandle h_owner = m_LigthsTemp[idx].getOwner();
				CEntity* e_owner = h_owner;
				if (!e_owner) continue;	//handle not valid

				TCompTransform* trans = e_owner->get<TCompTransform>();
				TCompName* name = e_owner->get<TCompName>();

				TCompLightDir* light_dir = m_LigthsTemp[idx];

				if (light_dir) {
					if (ImGui::TreeNode(name->name)) {
						name->renderInMenu();
						trans->renderInMenu();
						light_dir->renderInMenu();
						bool hidden = !light_dir->enabled;
						if (ImGui::Checkbox("hide", &hidden)) {
							light_dir->enabled = !hidden;
						}
						if (ImGui::SmallButton("Add as permanent")) {
							AddLightToSave(m_LigthsTemp[idx], m_TypesTemp[idx]);
						}
						ImGui::TreePop();
					}
				}
			}
			else if (m_TypesTemp[idx] == TypeLight::DIR_SHADOWS) {
				CHandle h_owner = m_LigthsTemp[idx].getOwner();
				CEntity* e_owner = h_owner;
				if (!e_owner) continue;	//handle not valid

				TCompName* name = e_owner->get<TCompName>();

				TCompTransform* trans = e_owner->get<TCompTransform>();
				TCompLightDirShadows* light_dir_shadows = m_LigthsTemp[idx];

				if (light_dir_shadows) {
					if (ImGui::TreeNode(name->name)) {
						name->renderInMenu();
						trans->renderInMenu();
						light_dir_shadows->renderInMenu();
						bool hidden = !light_dir_shadows->enabled;
						if (ImGui::Checkbox("hide", &hidden)) {
							light_dir_shadows->enabled = !hidden;
						}
						if (ImGui::SmallButton("Add as permanent")) {
							AddLightToSave(m_LigthsTemp[idx], m_TypesTemp[idx]);
						}
						ImGui::TreePop();
					}
				}
			}
			else {
				//nothing
			}
		}

		ImGui::End();
	}
}