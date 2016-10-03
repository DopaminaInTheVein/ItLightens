#include "mcv_platform.h"
#include "fx_depth_fog.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderDepthFog::renderInMenu() {
	//nothing
}

void TRenderDepthFog::ApplyFX() {
	if (enabled) {
		activateBlend(BLENDCFG_COMBINATIVE);

		//DoF
		tech->activate();

		drawFullScreen(render_deferred->GetFinalTexture(), tech);
	}
}

void TRenderDepthFog::init() {
	enabled = true;
	tech = Resources.get("depth_field.tech")->as<CRenderTechnique>();

	
}