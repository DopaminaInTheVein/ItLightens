#include "mcv_platform.h"
#include "fx_hatching.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderHatching::renderInMenu() {
	ImGui::Text("Limit lines hatching (DISABLED, not working)");
	if (ImGui::DragFloat("Rim strength", &shader_ctes_hatching.rim_strenght, 0.1f)) {
		shader_ctes_hatching.uploadToGPU();
	}
	if (ImGui::DragFloat("Specular strength", &shader_ctes_hatching.specular_strenght, 0.1f)) {
		shader_ctes_hatching.uploadToGPU();
	}
	if (ImGui::DragFloat("Diffuse strength", &shader_ctes_hatching.diffuse_strenght, 0.1f)) {
		shader_ctes_hatching.uploadToGPU();
	}

	ImGui::Separator();
	ImGui::Text("Lines hatching options");
	if (ImGui::DragFloat("Size hatching lines", &shader_ctes_hatching.frequency_texture, 0.1f)) {
		shader_ctes_hatching.uploadToGPU();
	}
	if (ImGui::DragFloat("Intensity transparency lines", &shader_ctes_hatching.intensity_sketch, 0.1f)) {
		shader_ctes_hatching.uploadToGPU();
	}
	if (ImGui::DragFloat("Frequency change offset", &shader_ctes_hatching.frequency_offset, 0.1f)) {
		shader_ctes_hatching.uploadToGPU();
	}
}

void TRenderHatching::ApplyFX() {
	if (enabled) {
		activateBlend(BLENDCFG_COMBINATIVE);
		drawFullScreen(render_deferred->GetFinalTexture(), tech);
	}
}

void TRenderHatching::init() {
	enabled = true;
	tech = Resources.get("hatching.tech")->as<CRenderTechnique>();

	//enable textures
	Resources.get("textures/hatching/hatching_tileable_ALPHAS.dds")->as<CTexture>()->activate(TEXTURE_SLOT_HATCHING);
	Resources.get("textures/hatching/hatching_tileable_prueba_plugin.dds")->as<CTexture>()->activate(64);

	//hatching texture
	//Resources.get("textures/hatching/hatch_0.dds")->as<CTexture>()->activate(TEXTURE_SLOT_HATCHING);

	shader_ctes_hatching.edge_lines_detection = 0.02f;
	shader_ctes_hatching.frequency_offset = 8.0f;
	shader_ctes_hatching.intensity_sketch = 2.0f;
	shader_ctes_hatching.rim_strenght = 1.0f;
	shader_ctes_hatching.specular_strenght = 50.0f;
	shader_ctes_hatching.diffuse_strenght = 1.0f;
	shader_ctes_hatching.frequency_texture = 10.0f;
	shader_ctes_hatching.color_ramp = 0.0f;
	shader_ctes_hatching.specular_force = 0.5f;
	shader_ctes_hatching.rim_specular = 1.5f;

	shader_ctes_hatching.uploadToGPU();
}