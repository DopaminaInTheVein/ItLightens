#include "mcv_platform.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "fx_fade_screen.h"

#include "app_modules/render/module_render_postprocess.h"

// ---------------------
void TFadeScreen::renderInMenu() {
	if (ImGui::Checkbox("fade_in", &fade_in)) fade_out = !fade_in;
	if (ImGui::Checkbox("fade_out", &fade_out)) fade_in = !fade_out;
	ImGui::DragFloat("max timer fade", &t_max_fade);
	ImGui::Text("curr time: %f\n", t_curr_fade);
	TCompBasicFX::renderInMenu();
}

void TFadeScreen::update(float elapsed)
{
	if (fade_out) {
		t_curr_fade += elapsed;
		if (t_curr_fade > t_max_fade) {
			fade_out = false;
		}
	}
	else if (fade_in) {
		t_curr_fade -= elapsed;
		if (t_curr_fade < 0) {
			fade_in = false;
		}
	}
	else {
		return;
	}
	clamp_me(t_curr_fade, 0.f, t_max_fade);

	shader_ctes_globals.fade_black_screen = t_curr_fade / t_max_fade;
	shader_ctes_globals.uploadToGPU();
}

void TFadeScreen::SetMaxTime(float new_time) {
	t_max_fade = (new_time > 0) ? new_time : t_max_fade_default;
}

void TFadeScreen::FadeIn()
{
	fade_in = true;
	fade_out = false;
}

void TFadeScreen::FadeOut()
{
	fade_in = false;
	fade_out = true;
}

void TFadeScreen::ApplyFX()
{
	activateZ(ZCFG_ALL_DISABLED);
	activateBlend(BLENDCFG_COMBINATIVE);

	if (t_curr_fade > 0) {
		tech->activate();
		drawFullScreen(all_black, tech);	//wont use texture all_black
	}
}

void TFadeScreen::init()
{
	tech = Resources.get("fade_screen.tech")->as<CRenderTechnique>();
	fade_in = false;		//by default false on start
	fade_out = !fade_in;
	t_max_fade = t_max_fade_default = 2.0f;
	t_curr_fade = 0.0f;
}