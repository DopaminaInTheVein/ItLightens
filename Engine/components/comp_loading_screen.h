#ifndef INC_COMPONENT_LOADING_SCREEN_H_
#define INC_COMPONENT_LOADING_SCREEN_H_

#include "comp_base.h"

class TCompLoadingScreen : public TCompBase {
	float resolution_x;
	float resolution_y;
	int loading_value = 0;
	ImGuiWindowFlags flags;
	bool added = false;

public:
	//Update info

	void update(float elapsed);
	void render() const;
	bool load(MKeyValue& atts);
};

#endif
