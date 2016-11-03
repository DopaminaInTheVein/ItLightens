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
	bool		selected = false;
	static bool		debug_render;
	VEC4 color;
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	void render() const;
	void renderInMenu();
	void uploadShaderCtes();
	void activate();
	void reloadLightmap(std::string) {} //Do nothing

	//Editor
	TCompLightPoint* original = nullptr;
	void start_editing();
	void cancel_editing();
	float* getShadowIntensityPointer() { return nullptr; }
	float* getNearPointer() { return &in_radius; }
	float* getFarPointer() { return &out_radius; }
	float* getFovPointer() { return nullptr; }
	~TCompLightPoint();
};
#endif
