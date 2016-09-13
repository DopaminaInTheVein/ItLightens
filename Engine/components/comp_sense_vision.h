#ifndef INC_COMPONENT_SENSE_VISION_H_
#define INC_COMPONENT_SENSE_VISION_H_

#include "comp_base.h"
#include <set>

struct TCompSenseVision : public TCompBase {
public:
	std::set<CHandle> handles;
	enum eSenseVision {
		DISABLED = 0,
		ENABLED,
		DEFAULT,
	} sense_vision_mode;

	bool load(MKeyValue& atts);
	void renderInMenu();

	// Gets
	bool isSenseVisionEnabled();

	//Sets
	void registerHandle(CHandle);
	void unregisterHandle(CHandle);
	void setSenseVisionMode(eSenseVision);
	void setSenseVisionMode(bool);
	void resetVisionMode();
};

#endif
