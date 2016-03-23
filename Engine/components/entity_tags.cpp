#include "mcv_platform.h"
#include "entity_tags.h"

TTagsManager tags_manager;

// ---------------------------------------
void TTagsManager::addTag(CHandle h, TTagID tag_id) {
	auto it = find(tag_id);
	if (it == end()) {
		insert(std::pair<TTagID, VHandles>(tag_id, VHandles(1, h)));
	}
	else {
		it->second.push_back(h);
	}
}

void TTagsManager::removeTag(CHandle h, TTagID tag_id)
{
	for (auto& it_m : tags_manager) {
		if (it_m.first == tag_id) {
			it_m.second.erase(std::remove(it_m.second.begin(), it_m.second.end(), h), it_m.second.end());
		}
	}
}

// ---------------------------------------
CHandle TTagsManager::getFirstHavingTag(TTagID tag_id) const {
	auto h = getHandlesByTag(tag_id);
	if (h.empty())
		return CHandle();
	return h[0];
}

void TTagsManager::getTagFromHandle(CHandle h_match, std::vector<TTagID>& out)
{
	for (auto& it_m : tags_manager) {
		for (CHandle h : it_m.second) {
			if (h == h_match)
				out.push_back(it_m.first);
		}
	}
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

// ---------------------------------------