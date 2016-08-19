#ifndef INC_COMPONENT_NAME_H_
#define INC_COMPONENT_NAME_H_

#include "utils/XMLParser.h"
#include "comp_base.h"

struct TCompName : public TCompBase {
	static const size_t max_name_length = 64;
	char name[max_name_length];
	bool load(MKeyValue& atts) {
		setName(atts.getString("name", "none").c_str());
		return true;
	}
	bool save(std::ofstream& os, MKeyValue& atts) {
		atts.put("name", std::string(name));
		return true;
	}

	void renderInMenu() {
		ImGui::Text(name);
	}

	void setName(std::string name_s) {
		strcpy(name, name_s.c_str());
	}
};

#endif
