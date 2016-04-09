#include "mcv_platform.h"
#include "entity.h"
#include "comp_name.h"
#include "imgui/imgui.h"
#include "comp_tags.h"

const char* CEntity::getName() const {
  TCompName* c = get<TCompName>();
  if (c)
    return c->name;
  return "unnamed";
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

// -----------------------------
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
        if (ImGui::TreeNode( CHandleManager::getByType( i )->getName())) {
          h.renderInMenu();
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