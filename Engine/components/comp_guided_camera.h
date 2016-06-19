#ifndef INC_COMPONENT_GUIDED_CAMERA_H_
#define INC_COMPONENT_GUIDED_CAMERA_H_

#include "comp_base.h"

//Forward declaration
class TCompCameraMain;
class TCompTransform;

class TCompGuidedCamera : public TCompBase {
	int num_points;
	float velocity;
	float velocity_default;
	bool default_dirs;
	bool reversed;

	std::vector<VEC3> positions;
	std::vector<VEC3> targets;

	int curPoint = 0;
	float factor = 0.0f;
	float smoothFactor = 0.9f;
public:
	TCompGuidedCamera() {
	}
	~TCompGuidedCamera() {
	}

	bool load(MKeyValue& atts);
	void renderInMenu() {}
	void update(float dt) {}

	const int getTotalPoints() const { return num_points; }
	const float
		ocity() const { return velocity; }

	void onGuidedCamera(const TMsgGuidedCamera&);
	bool followGuide(TCompTransform*, TCompCameraMain*);
	void start(float speed = 0.f);
};

#endif