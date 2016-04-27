#ifndef INC_COMPONENT_LIGHT_FADABLE_H_
#define INC_COMPONENT_LIGHT_FADABLE_H_

#include "comp_base.h"
#include "comp_camera.h"
#include "render/shader_cte.h"
#include "constants/ctes_light.h"

// ------------------------------------
struct TCompLightFadable : public TCompBase {
	float  out_radius;
	float  in_radius;
	float  ttl;

	VEC4 color;
	bool load(MKeyValue& atts);
	void renderInMenu();
	void uploadShaderCtes();

	void update(float dt);
	void activate();
};

#endif
