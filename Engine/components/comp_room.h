#ifndef INC_COMPONENT_ROOM_H_
#define INC_COMPONENT_ROOM_H_

#include "utils/XMLParser.h"
#include "comp_base.h"
#include <sstream>

struct TCompRoom : public TCompBase {
	std::vector<int> name;

	bool load(MKeyValue& atts) {
		std::string to_parse = atts.getString("name", "-1");
		std::stringstream ss(to_parse);
		std::string number;
		while (std::getline(ss, number, '/')) {
			int value = std::stoi(number);
			if (std::find(name.begin(), name.end(), value) == name.end()) {
				name.push_back(value);
			}
		}
		return true;
	}

	bool setName(std::vector<int> newName) {
		name = newName;
		return true;
	}

	void renderInMenu() {
		std::string all_rooms = std::to_string(name[0]);
		for (int i = 1; i < name.size(); i++) {
			all_rooms += " " + std::to_string(name[i]);
		}
		ImGui::Text(all_rooms.c_str());
	}
};

#endif
