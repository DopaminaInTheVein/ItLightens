#include "mcv_platform.h"
#include "fx_outline.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderOutline::renderInMenu() {
}

void TRenderOutline::ApplyFX() {
	if (enabled) {
		shader_ctes_globals.global_color = VEC4(1, 1, 1, 1);
		shader_ctes_globals.uploadToGPU();
		activateBlend(BLENDCFG_SUBSTRACT);
		drawFullScreen(render_deferred->GetFinalTexture(), tech);
	}
}

void TRenderOutline::init() {
	enabled = true;
	tech = Resources.get("edgeDetection.tech")->as<CRenderTechnique>();
}