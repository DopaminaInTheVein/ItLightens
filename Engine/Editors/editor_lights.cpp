#include "mcv_platform.h"
#include "editor_lights.h"

#include <fstream>

#include "utils/utils.h"

#include "handle\handle_manager.h"
#include "components\entity.h"
#include "components\comp_name.h"
#include "components\comp_transform.h"
#include "components\comp_culling.h"
#include "components\comp_room.h"

#include "components\entity_parser.h"

//lights includes
#include "components\comp_light_point.h"
#include "components\comp_light_dir.h"
#include "components\comp_light_dir_shadows.h"
#include "components\comp_light_dir_shadows_dyn.h"

static int id_name = 0;	//id used to create new unique lights

#define EACH__LIGHT__(a, b) \
a##<TCompLightPoint>##b \
a##<TCompLightDir>##b \
a##<TCompLightDirShadows>##b \
a##<TCompLightDirShadowsDynamic>##b

#define EACH_LIGHT_TYPE__(a) \
a##<TCompLightPoint>(TypeLight::POINT); \
a##<TCompLightDir>(TypeLight::DIR); \
a##<TCompLightDirShadows>(TypeLight::DIR_SHADOWS); \
a##<TCompLightDirShadowsDynamic>(TypeLight::DIR_SHADOWS_DYN); \

void CEditorLights::RenderInMenu()
{
	ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiSetCond_FirstUseEver);
	if (m_activated_editor) {
		ImGui::Begin("LightsEditor", &m_activated_editor);
		RenderGeneral();
		RenderNewLight();
		RenderAllLights();
		RenderMultiEdit();
		ImGui::End();
	}
}

void CEditorLights::RenderGeneral()
{
	ImGui::Text("VIEW OPTIONS");
	ImGui::Separator();
	if (ImGui::Checkbox("show axis", &m_show_axis)) {
		EACH__LIGHT__(SetRenderDebug, (m_show_axis);)
	}
	ImGui::Separator();
	ImGui::Text("LOAD / SAVE");
	ImGui::Separator();
	static char filename[64] = "";
	auto current_scene = CApp::get().getCurrentRealLevel();
	if (last_scene_name != current_scene) {
		last_scene_name = current_scene;
		sprintf(filename, "%s", current_scene.c_str());
	}

	ImGui::Text("bin/data/scenes/");
	ImGui::SameLine();
	ImGui::PushItemWidth(160);
	ImGui::InputText("_lights.xml", filename, 64);
	ImGui::PopItemWidth();
	//To load a specific file
	if (ImGui::SmallButton("Load")) {
		EACH__LIGHT__(DestroyLights, (););
		delete_lights = true;
		CApp::get().loadLights(filename + std::string("_lights"));
		LoadLights();
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Save")) {
		SaveLights(filename);
	}
	ImGui::Separator();
}

LightTemplate
void CEditorLights::DestroyLights()
{
	getHandleManager<TLight>()->each([](TLight * e) {
		if (CHandle(e).isValid()) {
			CHandle(e).getOwner().destroy();
		}
	});
}
void CEditorLights::RenderNewLight()
{
	static bool rooms_selected[ROOMS_SIZE] = { 0 };
	if (ImGui::CollapsingHeader("New Light")) {
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
		if (ImGui::Button("Add new dir shadows light dyn")) {
			AddLightToEngine(TypeLight::DIR_SHADOWS_DYN, rooms_selected);
		}
	}
}
void CEditorLights::RenderAllLights()
{
	if (ImGui::CollapsingHeader("All Lights")) {
		if (ImGui::SmallButton("Reload Lights")) {
			LoadLights();
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
	}
}
void CEditorLights::RenderMultiEdit()
{
	if (ImGui::CollapsingHeader("MultiEdit")) {
		if (multi_editing == IDLE) {
			if (ImGui::Button("Edit Selected")) {
				for (auto lhandle : m_Lights) StartEditLight(lhandle);
				for (auto lhandle : m_LightsTemp) StartEditLight(lhandle);
				multi_editing = EDITING;
			}
			if (ImGui::Button("Destroy Selected")) {
				DestroySelected(); //peta el imgui por alguna razon
			}
			if (ImGui::Button("Hide Selected")) {
				HideSelected(); //peta el imgui por alguna razon
			}
		}
		else if (multi_editing == EDITING) {
			if (ImGui::Button("Apply")) {
				multi_editing = IDLE;
				multi_edit_light = EditLight();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				multi_editing = IDLE;
				for (auto lhandle : m_Lights) CancelEditLight(lhandle);
				for (auto lhandle : m_LightsTemp) CancelEditLight(lhandle);
				multi_edit_light = EditLight();
			}
			multi_edit_light.renderInMenu();
		}
	}
}

void CEditorLights::DestroySelected()
{
	for (auto hlight : m_Lights) {
		if (IsSelected(hlight)) {
			if (hlight.isValid()) hlight.getOwner().destroy();
		}
	}
	for (auto hlight : m_LightsTemp) {
		if (IsSelected(hlight))  hlight.destroy();//if (hlight.isValid()) hlight.getOwner().destroy();
	}
}
void CEditorLights::HideSelected()
{
	for (auto hlight : m_Lights) {
		if (IsSelected(hlight)) {
			EACH__LIGHT__(HideLight, (hlight););
		}
	}
}

void CEditorLights::update(float dt) {
	if (multi_editing == EDITING) {
		for (auto hlight : m_Lights) UpdateEditingLight(hlight);
		for (auto hlight : m_LightsTemp) UpdateEditingLight(hlight);
	}
	if (controller->isEditorLightsButtonPressed()) {
		m_activated_editor = !m_activated_editor;
	}
}

//Load all the lights on the scene at game start
bool CEditorLights::LoadLights()
{
	m_Lights.clear();
	m_Types.clear();

	EACH_LIGHT_TYPE__(LoadLights);
	return true;
}
LightTemplate
void CEditorLights::LoadLights(TypeLight tl)
{
	CEditorLights* me = this;
	getHandleManager<TLight>()->each([me, tl](TLight* c) {
		CHandle h_light = CHandle(c);
		me->AddLightToSave(h_light, tl);
	});
}

bool CEditorLights::SaveLights(std::string fileName)
{
	if (fileName == DEFAULT_LIGHTS) {
		//read level name as lightfilename

		CApp& app = CApp::get();
		fileName = app.getCurrentRealLevel();
		fileName += "_lights";
	}

	std::string full_path = "data/scenes/" + fileName + "_lights.xml";

	MKeyValue atts;

	//file buffer
	std::ofstream os(full_path.c_str());

	atts.writeStartElement(os, "entities");

	for (auto hlight : m_Lights) {
		EACH__LIGHT__(SaveLight, (hlight, os, atts););
	}
	atts.writeEndElement(os, "entities");
	os.close();
	return true;
}

LightTemplate
bool CEditorLights::SaveLight(CHandle hlight, std::ofstream& ofs, MKeyValue& atts) {
	CHandle owner = hlight.isValid() ? hlight.getOwner() : CHandle();
	if (owner.isValid()) {
		GET_COMP(l, owner, TLight);
		if (l) return ((CEntity *)(owner))->save(ofs, atts);
	}
	return false;
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
	else if (type == TypeLight::DIR_SHADOWS_DYN) {
		h = spawnPrefab("light_dir_shadows_dyn_default");
		CEntity* e_owner = h;
		h = e_owner->get<TCompLightDirShadowsDynamic>();
		if (!h.isValid()) return false;
		TCompName* name = e_owner->get<TCompName>();
		std::string sname = "light_dir_shadow_dyn";
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
	for (int i = 0; i < ROOMS_SIZE; i++) {
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
	return false EACH__LIGHT__(|| HideLight, (e_owner));
}
LightTemplate
bool CEditorLights::HideLight(CEntity* e)
{
	TLight* l = e->get<TLight>();
	if (l) l->enabled = !l->enabled;
	return l;
}
LightTemplate
bool CEditorLights::HideLight(CHandle h)
{
	TLight* l = h;
	if (l) l->enabled = false;
	return l;
}

//void CEditorLights::SetRenderDebug(bool value, std::vector<CHandle> v_lights, std::vector<TypeLight> v_types) {
//	for (auto hlight : v_lights) {
//		EACH__LIGHT__(SetRenderDebug, (value););
//	}
//}
LightTemplate
void CEditorLights::SetRenderDebug(bool value) {
	TLight::debug_render = value;
}

LightTemplate
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
		if (ImGui::TreeNodeCheck(room_title, list.rcheck + room, r_changed[room], multi_editing == IDLE)) {
			//Every Light
			for (int idx = 0; idx < lights.size(); ++idx) {
				if (TCompRoom::SameRoom(lights[idx].getOwner(), room)) {
					RenderLight(lights[idx], types[idx], temporal);
				}
			}
			ImGui::TreePop();
		}
	}
	// Desmarcamos las puestas a false
	for (int room = 0; room < ROOMS_SIZE; room++) {
		if (r_changed[room]) {
			for (auto hlight : lights) {
				if (!hlight.isValid()) continue;
				if (TCompRoom::SameRoom(hlight.getOwner(), room)) {
					SetSelected(hlight, list.rcheck[room]);
				}
			}
		}
	}

	// Actualizamos Check de las rooms
	for (int room = 0; room < ROOMS_SIZE; room++) {
		bool found = false;
		for (auto hlight : lights) {
			if (!hlight.isValid()) continue;
			if (TCompRoom::SameRoom(hlight.getOwner(), room)) {
				if (IsSelected(hlight)) {
					found = true;
					break;
				}
			}
		}
		list.rcheck[room] = found;
	}
}

void CEditorLights::RenderLight(CHandle& h_light, TypeLight& type, bool temporal)
{
	//Specific Light
	if (type == TypeLight::POINT) RenderLight<TCompLightPoint>(h_light, type, temporal);
	else if (type == TypeLight::DIR)	RenderLight<TCompLightDir>(h_light, type, temporal);
	else if (type == TypeLight::DIR_SHADOWS) RenderLight<TCompLightDirShadows>(h_light, type, temporal);
	else if (type == TypeLight::DIR_SHADOWS_DYN) RenderLight<TCompLightDirShadowsDynamic>(h_light, type, temporal);
}

template <typename TLight>
void CEditorLights::RenderLight(CHandle& h_light, TypeLight& type, bool temporal)
{
	CHandle h_owner = h_light.getOwner();
	bool changed_selection = false;
	GET_COMP(name, h_owner, TCompName);
	GET_COMP(light, h_owner, TLight);
	GET_COMP(trans, h_owner, TCompTransform);
	if (light && name && trans) {
		ImGui::PushID(light);
		if (ImGui::TreeNodeCheck(GET_NAME(h_owner), &light->selected, changed_selection, multi_editing == IDLE)) {
			CHandle h_owner = h_light.getOwner();
			if (name) name->renderInMenu();
			if (trans)trans->renderInMenu();
			renderLightComp(light);
			if (temporal)
				RenderTemporalLight(h_light, type, light->enabled);
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}

void CEditorLights::RenderTemporalLight(CHandle& light, TypeLight& type, bool& enabled)
{
	ImGui::Checkbox("hide", &enabled);
	if (ImGui::SmallButton("Add as permanent")) {
		AddLightToSave(light, type);
	}
}

void CEditorLights::StartEditLight(CHandle hlight)
{
	if (!hlight.isValid()) return;
	CHandle lowner = hlight.getOwner();
	if (!lowner.isValid()) return;
	EACH__LIGHT__(StartEditLight, (lowner););
}
LightTemplate
void CEditorLights::StartEditLight(CHandle lowner)
{
	GET_COMP(l, lowner, TLight);
	if (l && l->selected) l->start_editing();
}

void CEditorLights::CancelEditLight(CHandle hlight)
{
	if (!hlight.isValid()) return;
	CHandle lowner = hlight.getOwner();
	if (!lowner.isValid()) return;
	EACH__LIGHT__(CancelEditLight, (lowner););
}
LightTemplate
void CEditorLights::CancelEditLight(CHandle lowner)
{
	GET_COMP(l, lowner, TLight);
	if (l && l->selected) l->cancel_editing();
}

void CEditorLights::SetSelected(CHandle hlight, bool sel)
{
	if (!hlight.isValid()) return;
	CHandle lowner = hlight.getOwner();
	if (!lowner.isValid()) return;
	EACH__LIGHT__(SetSelected, (lowner, sel););
}

LightTemplate
void CEditorLights::SetSelected(CHandle lowner, bool sel)
{
	GET_COMP(l, lowner, TLight);
	if (l) l->selected = sel;
}

bool CEditorLights::IsSelected(CHandle hlight)
{
	if (!hlight.isValid()) return false;
	CHandle lowner = hlight.getOwner();
	if (!lowner.isValid()) return false;
	return false EACH__LIGHT__(|| IsSelected, (lowner));
}
LightTemplate
bool CEditorLights::IsSelected(CHandle lowner)
{
	GET_COMP(l, lowner, TLight);
	if (l)  return l->selected;
}

// -------------------------------------------------- //
// EDIT LIGHT
// -------------------------------------------------- //
char CEditorLights::EditLight::mode_names[EditMode::SIZE][20] = { "Offset(+)", "Proportional(*)", "Replace(=)" };
void CEditorLights::EditLight::renderInMenu()
{
	pIntensity.renderInMenu();
	pRed.renderInMenu();
	pGreen.renderInMenu();
	pBlue.renderInMenu();
	pNear.renderInMenu();
	pFar.renderInMenu();
	pFov.renderInMenu();
}
void CEditorLights::EditLight::LightParam::renderInMenu()
{
	ImGui::PushID(this);
	ImGui::Text(name.c_str());

	ImGui::PushID(&v);
	ImGui::PushItemWidth(320);
	ImGui::DragFloat("", &v, vspeed, mode == PROP ? 0.f : -rmax, mode == PROP ? 5.f : rmax);
	ImGui::PopItemWidth();
	ImGui::PopID();

	ImGui::SameLine();
	ImGui::PushID(&vspeed);
	ImGui::PushItemWidth(100);
	if (ImGui::InputFloat("", &vspeed, 0.01f, 0.01f, 2) && !changed_by_user) {
		changed_by_user = true;
		if (vspeed < 0.01f) vspeed = 0.01f;
	}
	ImGui::PopItemWidth();
	ImGui::PopID();

	for (int i = 0; i < EditMode::SIZE; i++) {
		if (ImGui::Checkbox(mode_names[i], vmode + i) && !changed_by_user) {
			changed_by_user = true;
			if (vmode[i]) {
				for (int i2 = 0; i2 < EditMode::SIZE; i2++) {
					if (i2 != i) vmode[i2] = false;
				}
				mode = (EditMode)i;
				switch (mode) {
				case OFFSET:
					v = 0.f;
					break;
				case PROP:
					v = 1.f;
					break;
				case REPLACE:
					break;
				}
			}
			else {
				vmode[i] = true;
			}
		}
		if (i < EditMode::SIZE - 1) ImGui::SameLine();
	}

	ImGui::Separator();
	ImGui::PopID();
	changed_by_user = false;
}

void CEditorLights::UpdateEditingLight(CHandle hlight)
{
	if (!hlight.isValid()) return;
	CHandle lowner = hlight.getOwner();
	if (!lowner.isValid()) return;
	EACH__LIGHT__(multi_edit_light.updateLight, (lowner););
}

template <typename TLight>
void CEditorLights::EditLight::updateLight(CHandle lowner)
{
	GET_COMP(light, lowner, TLight);
	if (!light || !light->selected || !light->original) return;
	pIntensity.update(&light->original->color.w, &light->color.w);
	pRed.update(&light->original->color.x, &light->color.x);
	pGreen.update(&light->original->color.y, &light->color.y);
	pBlue.update(&light->original->color.z, &light->color.z);
	pNear.update(light->original->getNearPointer(), light->getNearPointer());
	pFar.update(light->original->getFarPointer(), light->getFarPointer());
	pFov.update(light->original->getFovPointer(), light->getFovPointer());
}

void CEditorLights::EditLight::LightParam::update(float *orig, float * dest)
{
	if (!orig || !dest) return;
	ToDisplay(orig);
	ToDisplay(dest);

	switch (mode) {
	case OFFSET:
		*dest = *orig + v;
		break;
	case PROP:
		*dest = *orig * v;
		break;
	case REPLACE:
		*dest = v;
		break;
	}
	*dest = clamp(*dest, rmin, rmax);
	ToIntern(orig);
	ToIntern(dest);
}