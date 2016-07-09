#ifndef INC_COMPONENT_ROOM_H_
#define INC_COMPONENT_ROOM_H_

#include "utils/XMLParser.h"
#include "comp_base.h"

struct TCompRoom : public TCompBase {
	static const size_t max_name_length = 64;
	char name[max_name_length];
	bool load(MKeyValue& atts) {
		strcpy(name, atts.getString("name", "none").c_str());
		return true;
	}

	bool setName(std::string newName) {
		strcpy(name, newName.c_str());
		return true;
	}

	void renderInMenu() {
		ImGui::Text(name);
	}
};

#endif
