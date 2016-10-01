#ifndef INC_COMPONENT_FADING_GLOBE_H_
#define INC_COMPONENT_FADING_GLOBE_H_

#include "comp_base.h"

class TCompFadingGlobe : public TCompBase {
	float resolution_x;
	float resolution_y;
	ImGuiWindowFlags flags;
	float globe_width;
	float globe_height;
	bool added = false;
	// aux params for computing the correct position and size
	std::string prefab_route;
	std::string globe_name;
	float distance;
	float char_x;
	float char_y;
	float char_z;
	// position of the globe
	float screen_x;
	float screen_y;
	float screen_z;
	// globes count
	static int globes;

	float  ttl;

	bool isBehindCamera();

public:
	//Update info

	void update(float elapsed);
	void render() const;
	bool load(MKeyValue& atts);
};

#endif
