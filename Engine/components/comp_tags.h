#ifndef INC_COMPONENT_TAGS_H_
#define INC_COMPONENT_TAGS_H_

#include "comp_base.h"
#include "entity_tags.h"
#include <sstream>
#include <set>

typedef uint32_t TTagID;

template< typename CB >
void split(const std::string& str, CB cb) {
	std::string word;
	std::istringstream iss(str, std::istringstream::in);
	while (iss >> word)
		cb(word);
}

// ----------------------------------------------------------------
struct TCompTags : public TCompBase {
	static const uint32_t max_tags = 16;
	TTagID   tags[max_tags];
	std::set<std::string> tags_str;
	int nextTag = 0;

	TCompTags() {
		for (uint32_t i = 0; i < max_tags; ++i)
			tags[i] = 0x00;
	}

	bool load(MKeyValue& atts) {
		auto all_tags = atts.getString("tags", "");
		split(all_tags, [this](const std::string& word) {
			auto tag_id = getID(word.c_str());
			tags_manager.registerTag(word);
			tags[nextTag++] = tag_id;
			tags_str.insert(word);
		});
		return true;
	}

	bool save(std::ofstream& os, MKeyValue& atts) {
		std::string tags_save = "";
		for (std::string t : tags_str) {
			if (tags_save == "") tags_save = t;
			else {
				tags_save += " ";
				tags_save += t;
			}
		}

		atts.put("tags", tags_save);
		return true;
	}

	bool created = false;
	void onCreate(const TMsgEntityCreated&) {
		if (created) return;
		created = true;
		CHandle h_entity = CHandle(this).getOwner();
		for (uint32_t i = 0; i < max_tags; ++i)
			if (tags[i])
				tags_manager.addTag(h_entity, tags[i]);
	}

	bool hasTag(TTagID tag) {
		for (auto t : tags) {
			if (t == tag)
				return true;
		}

		return false;
	}

	void renderInMenu() {
		for (uint32_t i = 0; i < max_tags; ++i) {
			if (tags[i])
				ImGui::Text("Tag %d : %s %08x", i, tags_manager.getNameOfTag(tags[i]), tags[i]);
		}
	}

	void onSetTag(const TMsgSetTag& msg) {
		CHandle h_entity = CHandle(this).getOwner();
		auto tag_id = getID(msg.tag.c_str());
		if (msg.add) {
			for (int i = 0; i < max_tags; ++i) {
				// Already found?
				if (tags[i] == tag_id)
					return;
				// Once we found an empty slot, fill it
				if (!tags[i]) {
					assert(nextTag < max_tags);
					tags[i] = tag_id;
					tags_str.insert(msg.tag);
					tags_manager.addTag(h_entity, tag_id);
					nextTag++;
					break;
				}
			}
		}
		else {
			for (int i = 0; i < max_tags; ++i) {
				// Already found?
				if (tags[i] == tag_id) {
					tags_manager.removeTag(h_entity, tag_id);
					nextTag--;
					tags[i] = tags[nextTag];
					tags[nextTag] = 0x00; // if i == nextTag first assignation dont do anything
					tags_str.erase(msg.tag);
					break;
				}
			}
		}
	}
};

#endif
