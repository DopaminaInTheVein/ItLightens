#include "mcv_platform.h"
#include "fx_motion_blur.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderMotionBlur::renderInMenu() {
	//nothing
}

void TRenderMotionBlur::ApplyFX() {
	if (enabled) {
		//TODO
	}
}

void TRenderMotionBlur::init() {
	enabled = true;
	//tech = Resources.get("hatching.tech")->as<CRenderTechnique>();

	//TODO
}