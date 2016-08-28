#ifndef INC_BOX_PLACER_COMPONENT_H_
#define	INC_BOX_PLACER_COMPONENT_H_

#include "components/comp_base.h"
#include "handle/handle.h"

struct TCompBoxPlacer : public TCompBase {
	std::string lua_event;
	bool box_placed;
	CHandle box;
	float radius;

	void update(float elapsed);
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
};

#endif