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
	static const uint32_t max_tag_length = 256;
	TTagID   tags[max_tags];
	char tags_str[max_tags][max_tag_length];
	//std::set<std::string> tags_str;
	int nextTag = 0;

	TCompTags() {
		initTags();
	}
	void initTags()
	{
		for (uint32_t i = 0; i < max_tags; ++i) {
			tags[i] = 0x00;
			//auto word_i = tags_str[i];
			sprintf(tags_str[i], "");
		}
	}

	bool load(MKeyValue& atts) {
		auto all_tags = atts.getString("tags", "");
		split(all_tags, [this](const std::string& word) {
			auto tag_id = getID(word.c_str());
			tags_manager.registerTag(word);
			sprintf(tags_str[nextTag], "%s", word.c_str());
			tags[nextTag++] = tag_id;
		});
		return true;
	}

	bool save(std::ofstream& os, MKeyValue& atts) {
		std::string tags_save = "";
		for (std::string t : tags_str) {
			if (tags_save == "") tags_save = t;
			else {
				tags_save += std::string(" ");
				tags_save += std::string(t);
			}
		}
		tags_save.erase(tags_save.find_last_not_of(" \n\r\t") + 1);
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
		int i = 0;
		for (auto str : tags_str) {
			if (str != "")
				ImGui::Text("%d: %s", i++, str);
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
					sprintf(tags_str[i], "%s", msg.tag.c_str());
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
					sprintf(tags_str[i], "%s", tags_str[nextTag]);
					tags[nextTag] = 0x00; // if i == nextTag first assignation dont do anything
					sprintf(tags_str[nextTag], "");
					break;
				}
			}
		}
	}

	~TCompTags()
	{
		initTags();
	}
};

#endif
