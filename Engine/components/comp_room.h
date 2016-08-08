#ifndef INC_COMPONENT_ROOM_H_
#define INC_COMPONENT_ROOM_H_

#include "utils/XMLParser.h"
#include "comp_base.h"
#include <sstream>

struct TCompRoom : public TCompBase {
	std::vector<int> name;

	void init();

	bool load(MKeyValue& atts);

	bool save(std::ofstream& os, MKeyValue& atts);

	bool setName(std::vector<int> newName);

	void renderInMenu();
};

#endif
