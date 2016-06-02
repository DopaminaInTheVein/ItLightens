#include "mcv_platform.h"
#include "entity_tags.h"
#include "entity.h"
#include "comp_msgs.h"

TTagsManager tags_manager;

void TTagsManager::renderInMenu() {
  for (auto it = begin(); it != end(); ++it) {
    TTagID tag_id = it->first;
    const char* tag_name = getNameOfTag(tag_id);
    if (ImGui::TreeNode((const void*) tag_id, "%s %08x", tag_name, tag_id)) {
      for (auto h : it->second) {
        ImGui::PushID(h.asUnsigned());
        h.renderInMenu();
        ImGui::PopID();
      }
      ImGui::TreePop();
    }
  }
}

// ---------------------------------------
CHandle findByName(const VHandles& handles, const char* entity_name) {
  for (auto h : handles) {
    CEntity* e = h;
    if (e->hasName(entity_name))
      return h;
  }
  return CHandle();
}

// ---------------------------------------
const char* TTagsManager::getNameOfTag(TTagID tag_id) const {
  auto it = name_of_tag.find(tag_id);
  if (it != name_of_tag.end())
    return it->second.c_str();
  return "unknown";
}

void TTagsManager::registerTag(const std::string& tag_name) {
  auto tag_id = getID(tag_name.c_str());
  name_of_tag[tag_id] = tag_name;
}

// ---------------------------------------
void TTagsManager::addTag(CHandle h, TTagID tag_id) {
  auto it = find(tag_id);
  if (it == end()) {
    insert(std::pair<TTagID, VHandles>(tag_id, VHandles(1,h)));
  }
  else {
    it->second.push_back(h);
  }
}

// ---------------------------------------
CHandle TTagsManager::getFirstHavingTag(TTagID tag_id) const {
  auto h = getHandlesByTag(tag_id);
  if (h.empty())
    return CHandle();
  return h[0];
}

// ---------------------------------------
const VHandles& TTagsManager::getHandlesByTag(TTagID tag_id) const {
  auto it = find(tag_id);
  if (it == end()) {
    static VHandles empty_set;
    return empty_set;
  }
  return it->second;
}
