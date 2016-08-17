#ifndef INC_COMPONENT_LIGHT_POINT_H_
#define INC_COMPONENT_LIGHT_POINT_H_

#include "comp_base.h"
#include "comp_camera.h"
#include "render/shader_cte.h"
#include "constants/ctes_light.h"

// ------------------------------------
struct TCompLightPoint : public TCompBase {
	float  out_radius;
	float  in_radius;
	float  ttl;

	bool		enabled = true;
	bool		debug_render = false;

	VEC4 color;
	bool load(MKeyValue& atts);
	void render() const;
	void renderInMenu();
	void uploadShaderCtes();
	void activate();
};

#endif
