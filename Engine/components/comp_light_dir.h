#ifndef INC_COMPONENT_LIGHT_DIR_H_
#define INC_COMPONENT_LIGHT_DIR_H_

#include "comp_base.h"
#include "comp_camera.h"
#include "render/shader_cte.h"
#include "constants/ctes_light.h"

// ------------------------------------
struct TCompLightDir : public TCompCamera {
	const CTexture*	light_mask;
	VEC4				color;
	bool				enabled = true;
	bool				selected = false;
	static bool			debug_render;
	static void setRenderDebug(bool b) { debug_render = b; }

	bool				load(MKeyValue& atts);
	bool				save(std::ofstream& os, MKeyValue& atts);
	void				render() const;
	void				renderInMenu();
	void				uploadShaderCtes(CEntity* e);
	void				activate();
	void				update(float dt);

	//Editor
	TCompLightDir* original = nullptr;
	void start_editing();
	void cancel_editing();
	~TCompLightDir();
};
#endif
