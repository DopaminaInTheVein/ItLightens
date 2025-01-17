#include "mcv_platform.h"
#include "entity.h"
#include "comp_name.h"
#include "imgui/imgui.h"
#include "comp_tags.h"
#include "app_modules/imgui/module_imgui.h"
const char* CEntity::getName() const {
	TCompName* c = get<TCompName>();
	if (c)
		return c->name;
	return HANDLE_UNNAMED;
}

void CEntity::setName(const char* new_name) {
	TCompName* c = get<TCompName>();
	if (c) {
		assert(strlen(new_name) < 64);
		strcpy(c->name, new_name);
	}
}

bool CEntity::hasName(const char* new_name) const {
	return strcmp(getName(), new_name) == 0;
}

bool CEntity::hasTag(std::string tag)
{
	TCompTags *tags = get<TCompTags>();
	if (tags) return tags->hasTag(getID(tag.c_str()));
	return false;
}

//------------------------------
bool CEntity::save(std::ofstream& os, MKeyValue& atts) {
	if (id >= 0) atts.put("id", id);
	atts.put("reload", need_reload);
	atts.put("permanent", permanent);
	atts.writeStartElement(os, "entity");

	for (uint32_t i = 0; i < CHandle::max_types; ++i) {
		auto hm = CHandleManager::getByType(i);
		if (comps[i].isValid()) {
			comps[i].save(os, atts);
			atts.writeSingle(os, hm->getName());
		}
	}
	atts.writeEndElement(os, "entity");

	return true;
}

// -----------------------------
void CEntity::renderInMenu() {
	const char* my_name = "unknown..";

	TCompName* c_name = get<TCompName>();
	if (c_name)
		my_name = c_name->name;

	//Check filter
	std::string str = std::string(my_name);
	int found = str.find(CImGuiModule::getFilterText()); //TODO read filter
	if (found == std::string::npos) return;

	//Show entity
	ImGui::PushID(this);
	if (ImGui::TreeNode(my_name)) {
		IMGUI_SHOW_BOOL(permanent);
		IMGUI_SHOW_BOOL(need_reload);
		// For each component type defined in the game ( comp type 0 is invalid... )
		for (uint32_t i = 1; i < CHandleManager::getNumDefinedTypes(); ++i) {
			CHandle h = comps[i];
			if (h.isValid()) {
				// Open a tree node with the name of the component
				if (ImGui::TreeNode(CHandleManager::getByType(i)->getName())) {
					h.renderInMenu();
#ifndef NDEBUG
					ImGui::Checkbox("Debug", h.getDebug());
#endif
					if (ImGui::Button("Destroy Comp.")) h.destroy();
					ImGui::TreePop();
				}
			}
		}
		CHandle h(this);
		if (ImGui::Button("Destroy"))
			h.destroy();

		ImGui::SameLine();
		ImGui::Text("Idx:%04x Age:%04x", h.getExternalIndex(), h.getAge());

		ImGui::TreePop();
	}
	ImGui::PopID();
}