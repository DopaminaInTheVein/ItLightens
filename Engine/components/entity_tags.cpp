#include "mcv_platform.h"
#include "entity_tags.h"
#include "entity.h"
#include "comp_msgs.h"

TTagsManager tags_manager;
using namespace std;

void TTagsManager::renderInMenu() {
	for (auto it = begin(); it != end(); ++it) {
		TTagID tag_id = it->first;
		const char* tag_name = getNameOfTag(tag_id);
		if (ImGui::TreeNode((const void*)tag_id, "%s %08x", tag_name, tag_id)) {
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
ClHandle findByName(const VHandles& handles, const char* entity_name) {
	for (auto h : handles) {
		CEntity* e = h;
		if (e->hasName(entity_name))
			return h;
	}
	return ClHandle();
}

void TTagsManager::removeAllTags(ClHandle h)
{
	for (auto& it_m : tags_manager) {
		it_m.second.erase(std::remove(it_m.second.begin(), it_m.second.end(), h), it_m.second.end());
	}
}

void TTagsManager::removeTag(ClHandle h, TTagID tag_id)
{
	for (auto& it_m : tags_manager) {
		if (it_m.first == tag_id) {
			it_m.second.erase(std::remove(it_m.second.begin(), it_m.second.end(), h), it_m.second.end());
		}
	}
}

void TTagsManager::getTagFromHandle(ClHandle h_match, std::vector<TTagID>& out)
{
	for (auto& it_m : tags_manager) {
		for (ClHandle h : it_m.second) {
			if (h == h_match)
				out.push_back(it_m.first);
		}
	}
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
ClHandle TTagsManager::getFirstHavingTag(TTagID tag_id) const {
	ClHandle result;
	auto h = getHandlesByTag(tag_id);
	return h.size() > 0 ? h[0] : ClHandle();
}

ClHandle TTagsManager::getFirstHavingTag(std::string tag_id) const {
	return getFirstHavingTag(getID(tag_id.c_str()));
}

// ---------------------------------------
void TTagsManager::addTag(ClHandle h, TTagID tag_id) {
	auto it = find(tag_id);
	if (it == end()) {
		insert(std::pair<TTagID, VHandles>(tag_id, VHandles(1, h)));
	}
	else {
		it->second.push_back(h);
	}
}

// ---------------------------------------
const VHandles& TTagsManager::getHandlesByTag(TTagID tag_id) const {
	auto it = find(tag_id);
	if (it == end()) {
		static VHandles empty_set;
		return empty_set;
	}
	//auto result = it->second;
	return it->second;
}
// ---------------------------------------
ClHandle TTagsManager::getHandleByTagAndName(const char* tag, const char* name) const {
	VHandles targets = tags_manager.getHandlesByTag(getID(tag));
	return findByName(targets, name);
}

const VHandles& TTagsManager::getHandlesByTag(std::string tag_id) const {
	return getHandlesByTag(getID(tag_id.c_str()));
}

// ---------------------------------------