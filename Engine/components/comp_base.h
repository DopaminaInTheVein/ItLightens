#ifndef INC_COMPONENT_BASE_H_
#define INC_COMPONENT_BASE_H_

#include "utils\XMLParser.h"
#include "comp_msgs.h"
class MKeyValue;
class CEntity;

struct TCompBase {
	CEntity * compBaseEntity;
	void render() {}
	void init() {}
	virtual bool getUpdateInfoBase(CHandle parent);
	virtual bool getUpdateInfo() { return true; }
	void update(float elapsed) {
		(void)(elapsed);
	}

	void fixedUpdate(float elapsed) {
		(void)(elapsed);
	}
	bool load(MKeyValue& atts) {
		(void)(atts);
		return true;
	}

	bool save(std::ofstream& os, MKeyValue& atts) { return false; }

	void renderInMenu() {}

	static void reloadAttr() {};
};

#endif
