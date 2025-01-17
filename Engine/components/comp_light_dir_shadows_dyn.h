#ifndef INC_COMPONENT_LIGHT_DIR_SHADOWS_DYN_H_
#define INC_COMPONENT_LIGHT_DIR_SHADOWS_DYN_H_

#include "comp_base.h"
#include "comp_light_dir_shadows.h"

class CRenderToTexture;

// ------------------------------------
struct TCompLightDirShadowsDynamic : public TCompLightDirShadows {
	VEC3 last_position_target = VEC3(0, 0, 0);
	CHandle h_main_camera;

	float offset = 7.0f;

	bool is_inRoom = true;

	bool save(std::ofstream & os, MKeyValue & atts);
	bool load(MKeyValue & atts);
	void init();
	bool setCameraHandle();
	void update(float dt);

	void generateShadowMap();

	void destroy();

	void setNewFov(float fov_in_rads);

	void activate();

	bool isInRoom() const {
		return is_inRoom;
	}

	//Editor
	TCompLightDirShadowsDynamic* original = nullptr;
	void start_editing();
	void cancel_editing();
	~TCompLightDirShadowsDynamic();
};

#endif
