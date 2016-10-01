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
	bool perenne = false;
	// aux params for computing the correct position and size
	std::string globe_name;
	std::string prefabToUse;
	float distance;
	float char_x;
	float char_y;
	float char_z;
	// position of the globe
	float screen_x;
	float screen_y;
	float screen_z;

	float  ttl;

	bool isBehindCamera();

public:
	//Update info
	std::string getGlobeName() const {
		return globe_name;
	}
	void forceTTLZero() {
		ttl = -0.1f;
	}
	void update(float elapsed);
	void render() const;
	bool load(MKeyValue& atts);
};

#endif
