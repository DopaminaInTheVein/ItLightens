#include "mcv_platform.h"
#include "fx_dream.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderDream::renderInMenu() {
	if (ImGui::DragFloat4("color influence", &shader_ctes_dream.color_influence.x, 0.01f)) {
		shader_ctes_dream.uploadToGPU();
	}
	if (ImGui::DragFloat("waves size", &shader_ctes_dream.dream_waves_size, 0.01f)) {
		shader_ctes_dream.uploadToGPU();
	}
	if (ImGui::DragFloat("waves speed", &shader_ctes_dream.dream_speed, 0.01f)) {
		shader_ctes_dream.uploadToGPU();
	}
	if (ImGui::DragFloat("waves intensity", &shader_ctes_dream.dream_wave_amplitude, 0.01f)) {
		shader_ctes_dream.uploadToGPU();
	}
	if (ImGui::DragFloat("distorsion strenght", &shader_ctes_dream.dream_distorsion_strenght, 0.01f)) {
		shader_ctes_dream.uploadToGPU();
	}
	if (ImGui::DragFloat("distorsion expansion", &shader_ctes_dream.dream_distorsion_expansion, 0.01f)) {
		shader_ctes_dream.uploadToGPU();
	}

	if (ImGui::SmallButton("Load defaults")) {
		shader_ctes_dream.color_influence = VEC4(0.2, 0.2, 0.3, 1);
		shader_ctes_dream.dream_speed = 1;
		shader_ctes_dream.dream_waves_size = 0.5;
		shader_ctes_dream.dream_wave_amplitude = 8;

		shader_ctes_dream.dream_distorsion_expansion = 0.85;
		shader_ctes_dream.dream_distorsion_strenght = 55;
	}
}

void TRenderDream::ApplyFX() {
	if (enabled) {
		activateBlend(BLENDCFG_COMBINATIVE);
		
		tech->activate();

		drawFullScreen(render_deferred->GetFinalTexture(), tech);
	}
}

void TRenderDream::init() {
	enabled = true;
	tech = Resources.get("dream_effect.tech")->as<CRenderTechnique>();

	shader_ctes_dream.color_influence = VEC4(0.2, 0.2, 0.3, 1);
	shader_ctes_dream.dream_speed = 1;
	shader_ctes_dream.dream_waves_size = 0.5;
	shader_ctes_dream.dream_wave_amplitude = 8;

	shader_ctes_dream.dream_distorsion_expansion = 0.85;
	shader_ctes_dream.dream_distorsion_strenght = 55;

	shader_ctes_dream.uploadToGPU();
}