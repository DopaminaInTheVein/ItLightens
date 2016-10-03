#include "mcv_platform.h"
#include "fx_antialiasing.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderAntiAliasing::renderInMenu() {
	//nothing
}

void TRenderAntiAliasing::ApplyFX() {
	if (enabled) {
		//TODO
	}
}

void TRenderAntiAliasing::init() {
	enabled = true;
	//tech = Resources.get("hatching.tech")->as<CRenderTechnique>();

	//TODO
}