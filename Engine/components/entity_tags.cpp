#include "mcv_platform.h"
#include "entity_tags.h"

TTagsManager tags_manager;

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

VEntities TTagsManager::getHandlesPointerByTag(TTagID tag_id) {
	VEntities entities;
	for (auto it : getHandlesByTag(tag_id)) {
		CEntity * entity = it;
		entities.push_back(entity);
	}
	return entities;
}
