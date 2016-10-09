#include "mcv_platform.h"
#include "fx_antialiasing.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderAntiAliasing::renderInMenu() {
	ImGui::Checkbox("enable antialiasing", &enabled);
}


void TRenderAntiAliasing::ApplyFX() {
	if (enabled) {
		activateBlend(BLENDCFG_DEFAULT);
		Render.activateBackBuffer();
		tech->activate();

		drawFullScreen(render_deferred->GetOutputTexture(), tech);
	}
	else {
		activateBlend(BLENDCFG_DEFAULT);
		Render.activateBackBuffer();
		drawFullScreen(render_deferred->GetOutputTexture());
	}
}

void TRenderAntiAliasing::init() {
	enabled = true;
	tech = Resources.get("anti_aliasing.tech")->as<CRenderTechnique>();

	//TODO
}