#ifndef INC_BOX_PLACER_COMPONENT_H_
#define	INC_BOX_PLACER_COMPONENT_H_

#include "components/comp_base.h"
#include "handle/handle.h"

struct TCompBoxPlacer : public TCompBase {
	std::string lua_event_in;
	std::string lua_event_out;
	bool box_placed;
	CHandle box;
	float radius;

	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	void init();
	//void onCreate(const TMsgEntityCreated&);
	void update(float elapsed);

	//Aux
	bool checkBox();
};

#endif