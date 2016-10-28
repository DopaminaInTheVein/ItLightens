#ifndef INC_COMPONENT_FADING_GLOBE_H_
#define INC_COMPONENT_FADING_GLOBE_H_

#include "comp_base.h"
class TCompTransform;
class TCompCameraMain;

class TCompFadingGlobe : public TCompBase {
	float resolution_x;
	float resolution_y;
	ImGuiWindowFlags flags;
	//float globe_width;
	//float globe_height;
	//bool added = false;
	CHandle globe_handle;
	CHandle camera_main;
	TCompTransform* cam_tmx;
	TCompCameraMain* cam;
	float size_world = -1.f;
	bool perenne = false;

	// aux params for computing the correct position and size
	std::string prefab_route;
	std::string globe_name;
	//float distance;
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
	float MAX_DISTANCE = -1.f;

	bool isBehindCamera();
	bool inDistance();
	void createGlobe();
	float getGlobeScale();

public:
	//Update info
	bool getUpdateInfo() override;
	std::string getGlobeName() const {
		return globe_name;
	}
	void forceTTLZero() {
		ttl = -0.1f;
	}
	void update(float elapsed);
	//void render() const;
	bool load(MKeyValue& atts);
	void setWorldSize(float new_size_world) { size_world = new_size_world; }
	float getWorldSize() { return size_world; }
};

#endif
