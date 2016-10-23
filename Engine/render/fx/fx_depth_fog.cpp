#include "mcv_platform.h"
#include "fx_depth_fog.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"
#include "components\comp_transform.h"
#include "components\entity.h"

extern CRenderDeferredModule * render_deferred;

// ---------------------
void TRenderDepthFog::renderInMenu() {
	
	VEC3 color_fog = shader_ctes_fog.color_fog;
	if(ImGui::DragFloat3("color fog", &color_fog.x, 0.1f)) {
		shader_ctes_fog.color_fog = color_fog;
		shader_ctes_fog.uploadToGPU();
	}

	float alpha = shader_ctes_fog.color_fog.w;
	if (ImGui::DragFloat("Intensity Fog", &alpha, 0.1f)) {
		shader_ctes_fog.color_fog.w = alpha;
		alpha_orig = shader_ctes_fog.color_fog.w;
		shader_ctes_fog.uploadToGPU();
	}

	if (ImGui::DragFloat("Density Fog", &shader_ctes_fog.fog_density, 0.1f)) {
		shader_ctes_fog.uploadToGPU();
	}

	if (ImGui::DragFloat("Upper limit", &shader_ctes_fog.fog_upper_limit, 0.1f)) {
		shader_ctes_fog.uploadToGPU();
	}

	if (ImGui::DragFloat("Max intensity", &shader_ctes_fog.fog_max_intesity, 0.1f)) {
		shader_ctes_fog.uploadToGPU();
	}

	if (ImGui::DragFloat("Fog start distance", &shader_ctes_fog.fog_distance, 0.1f)) {
		shader_ctes_fog.uploadToGPU();
	}

	if (ImGui::DragFloat("Fog floor", &shader_ctes_fog.fog_floor, 0.1)) {
		shader_ctes_fog.uploadToGPU();
	}
}

void TRenderDepthFog::ApplyFX() {
	if (enabled) {
		activateBlend(BLENDCFG_COMBINATIVE);

		//DoF
		tech->activate();

		drawFullScreen(render_deferred->GetFinalTexture(), tech);
	}
}

void TRenderDepthFog::update(float dt) {
	if (enabled_dist_calc) {
		GET_COMP(t_player, h_player, TCompTransform);

		if (!t_player) return;

		float d = simpleDist(t_player->getPosition(), m_position_point_distance);

		//calculate distance factor for alpha blend
		float alpha_factor = 1 / (1 + d);
		shader_ctes_fog.color_fog.w *= alpha_factor;
	}
}

void TRenderDepthFog::SetPosition(VEC3 new_position)
{
	m_position_point_distance = new_position;
}

VEC3 TRenderDepthFog::GetPosition() const
{
	return m_position_point_distance;
}

void TRenderDepthFog::EnableDistanceCalculation()
{
	enabled_dist_calc = true;
}

void TRenderDepthFog::DisableDistanceCalculation()
{
	enabled_dist_calc = false;
	shader_ctes_fog.color_fog.w = alpha_orig;
}

#include "player_controllers\player_controller_base.h"

void TRenderDepthFog::init() {
	enabled = true;
	tech = Resources.get("fog_depth.tech")->as<CRenderTechnique>();
	h_player = CPlayerBase::handle_player;

	shader_ctes_fog.color_fog = VEC4(0.83, 0.75, 0.7, 0.7);

	shader_ctes_fog.fog_upper_limit = 7;
	shader_ctes_fog.fog_density = 0.5f;
	shader_ctes_fog.fog_max_intesity = 0.7f;
	shader_ctes_fog.fog_distance = 0.02f;

	shader_ctes_fog.fog_floor = -22;

	shader_ctes_fog.uploadToGPU();

	enabled_dist_calc = false;

	alpha_orig = shader_ctes_fog.color_fog.w;
}