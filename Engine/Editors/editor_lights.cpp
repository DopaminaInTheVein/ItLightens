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

//CEditorLights::CEditorLights()
//{
//	//rooms_engine = rooms_temp = std::vector<bool>(ROOMS_SIZE, false);
//}

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
	RemoveLight(h, m_LightsTemp, m_TypesTemp);
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
			room_comp->addRoom(r);
		}
		((CEntity*)(h.getOwner()))->add(new_h);
	}

	m_LightsTemp.push_back(h);
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

template <typename TLight>
void CEditorLights::renderLightComp(TLight* lcomp) {
	lcomp->renderInMenu();
	bool hidden = !lcomp->enabled;
	if (ImGui::Checkbox("hide", &hidden)) {
		lcomp->enabled = !hidden;
	}
	if (ImGui::SmallButton("Destroy")) {
		CHandle light_handle = CHandle(lcomp);
		RemoveLight(light_handle, m_Lights, m_Types);
		RemoveLight(light_handle, m_LightsTemp, m_TypesTemp);
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
			SetRenderDebug(m_show_axis, m_LightsTemp, m_TypesTemp);
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

		//permanent lights
		if (ImGui::TreeNode("Engine Lights")) {
			RenderLightList(m_Lights, m_Types, false, engine_list);
			ImGui::TreePop();
		}

		//temporal lights
		if (ImGui::TreeNode("Temporal Lights")) {
			RenderLightList(m_LightsTemp, m_TypesTemp, true, temp_list);
			ImGui::TreePop();
		}

		ImGui::End();
	}
}

void CEditorLights::RenderLightList(VHandles& lights, VTypeLights& types, bool temporal, LightList& list)
{
	int m_Lights = 0; // Check
	int m_Types = 0;

	// Every room
	char room_title[] = "Room 00";
	bool r_changed[ROOMS_SIZE] = { false };
	for (int room : TCompRoom::all_rooms) {
		assert(room < ROOMS_SIZE && room < 100);
		sprintf(room_title, "Room %d", room);
		if (ImGui::TreeNodeCheck(room_title, list.rcheck + room, r_changed[room])) {
			//Every Light
			for (int idx = 0; idx < lights.size(); ++idx) {
				if (!lights[idx].isValid()) return;
				if (TCompRoom::SameRoom(lights[idx].getOwner(), room)) {
					RenderLight(lights[idx], types[idx], temporal);
				}
			}
			ImGui::TreePop();
		}
	}

	for (int i = 0; i < ROOMS_SIZE; i++) {
		if (r_changed[i]) {
			dbg("Ha cambiado room %d a %d\n", i, list.rcheck[i]);
		}
	}
}

void CEditorLights::RenderLight(CHandle& h_light, TypeLight& type, bool temporal)
{
	//Specific Light
	if (type == TypeLight::POINT) RenderLight<TCompLightPoint>(h_light, type, temporal);
	else if (type == TypeLight::DIR)	RenderLight<TCompLightDir>(h_light, type, temporal);
	else if (type == TypeLight::DIR_SHADOWS) RenderLight<TCompLightDirShadows>(h_light, type, temporal);
}

template <typename TLight>
void CEditorLights::RenderLight(CHandle& h_light, TypeLight& type, bool temporal)
{
	CHandle h_owner = h_light.getOwner();
	GET_COMP(name, h_owner, TCompName);
	GET_COMP(light, h_owner, TLight);
	GET_COMP(trans, h_owner, TCompTransform);
	if (light) {
		if (ImGui::TreeNode(GET_NAME(h_owner))) {
			if (name) name->renderInMenu();
			if (trans)trans->renderInMenu();
			renderLightComp(light);
			if (temporal)
				RenderTemporalLight(h_light, type, light->enabled);
			ImGui::TreePop();
		}
	}
}

void CEditorLights::RenderTemporalLight(CHandle& light, TypeLight& type, bool& enabled)
{
	ImGui::Checkbox("hide", &enabled);
	if (ImGui::SmallButton("Add as permanent")) {
		AddLightToSave(light, type);
	}
}