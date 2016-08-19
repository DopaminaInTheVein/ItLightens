#ifndef INC_COMPONENT_ROOM_H_
#define INC_COMPONENT_ROOM_H_

#include "utils/XMLParser.h"
#include "comp_base.h"
#include <sstream>
#include <set>

struct TCompRoom : public TCompBase {
	static std::set<int> all_rooms;
public:
	std::vector<int> name;
	char rooms_raw[32];

	void init();

	bool load(MKeyValue& atts);

	bool save(std::ofstream& os, MKeyValue& atts);

	bool setName(std::vector<int> newName);

	void renderInMenu();
};

#endif
