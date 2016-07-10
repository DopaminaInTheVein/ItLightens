#ifndef INC_COMPONENT_ROOM_H_
#define INC_COMPONENT_ROOM_H_

#include "utils/XMLParser.h"
#include "comp_base.h"

struct TCompRoom : public TCompBase {
	int name;
	bool load(MKeyValue& atts) {
		name = atts.getInt("name", -1);
		return true;
	}

	bool setName(int newName) {
		name = newName;
		return true;
	}

	void renderInMenu() {
		ImGui::Text(std::to_string(name).c_str());
	}
};

#endif
