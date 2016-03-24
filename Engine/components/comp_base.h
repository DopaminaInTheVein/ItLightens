#ifndef INC_COMPONENT_BASE_H_
#define INC_COMPONENT_BASE_H_

#include "utils\XMLParser.h"
#include "comp_msgs.h"

class MKeyValue;

struct TCompBase {
	void render() {}
	void init() {}
	void update(float elapsed) {
		(void)(elapsed);
	}
	bool load(MKeyValue& atts) {
		(void)(atts);
		return true;
	}

	void onTrigger(const TMsgTrigger& msg){}
	void onTriggerExit(const TMsgTrigger& msg){}
	void onTriggered(const TMsgTrigger& msg){}
	void renderInMenu() {}
};

#endif
