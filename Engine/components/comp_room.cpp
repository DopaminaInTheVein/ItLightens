#include "mcv_platform.h"
#include "comp_room.h"
#include "logic/sbb.h"

std::set<int> TCompRoom::all_rooms = std::set<int>();

bool TCompRoom::load(MKeyValue& atts) {
	std::string to_parse = atts.getString("name", "-1");
	std::stringstream ss(to_parse);
	std::string number;
	strcpy(rooms_raw, to_parse.c_str());
	while (std::getline(ss, number, '/')) {
		int value = std::stoi(number);
		if (std::find(name.begin(), name.end(), value) == name.end()) {
			name.push_back(value);
			all_rooms.insert(value);
		}
	}
	return true;
}

void TCompRoom::init()
{
	if (CHandle(this).getOwner().hasTag("player")) {
		SBB::postSala(name[0]);
	}
}

bool TCompRoom::save(std::ofstream& os, MKeyValue& atts) {
	std::stringstream ss;
	for (int n : name) {
		if (ss.str() != "") ss << "/";
		ss << n;
	}
	atts.put("name", ss.str());
	return true;
}

bool TCompRoom::setName(std::vector<int> newName) {
	name = newName;
	return true;
}

void TCompRoom::renderInMenu() {
	std::string all_rooms = std::to_string(name[0]);
	for (int i = 1; i < name.size(); i++) {
		all_rooms += " " + std::to_string(name[i]);
	}
	ImGui::Text(all_rooms.c_str());
}