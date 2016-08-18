#ifndef INC_COMPONENT_LIGHT_DIR_SHADOWS_H_
#define INC_COMPONENT_LIGHT_DIR_SHADOWS_H_

#include "comp_base.h"
#include "comp_light_dir.h"

class CRenderToTexture;

// ------------------------------------
struct TCompLightDirShadows : public TCompLightDir {
	CRenderToTexture* rt_shadows = nullptr;
	bool              enabled = true;
	int res;
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	void activate();
	void update(float dt);
	void generateShadowMap();
	void destroy();

	void setNewFov(float fov);
};

#endif
