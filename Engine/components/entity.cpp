#include "mcv_platform.h"
#include "entity.h"
#include "comp_name.h"
#include "imgui/imgui.h"

void CEntity::renderInMenu() {
	const char* my_name = "unknown..";

	TCompName* c_name = get<TCompName>();
	if (c_name)
		my_name = c_name->name;

	ImGui::PushID(this);
	if (ImGui::TreeNode(my_name)) {
		// For each component type defined in the game ( comp type 0 is invalid... )
		for (uint32_t i = 1; i < CHandleManager::getNumDefinedTypes(); ++i) {
			CHandle h = comps[i];
			if (h.isValid()) {
				// Open a tree node with the name of the component
				if (ImGui::TreeNode(CHandleManager::getByType(i)->getName())) {
					h.renderInMenu();
					ImGui::TreePop();
				}
			}
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}