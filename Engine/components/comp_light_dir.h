#ifndef INC_COMPONENT_LIGHT_DIR_H_
#define INC_COMPONENT_LIGHT_DIR_H_

#include "comp_base.h"
#include "comp_camera.h"
#include "render/shader_cte.h"
#include "constants/ctes_light.h"

// ------------------------------------
struct TCompLightDir : public TCompCamera {

	VEC4 color;
	bool load(MKeyValue& atts);
	void renderInMenu();
	void uploadShaderCtes();
  void activate();
  void update(float dt);
};

#endif
