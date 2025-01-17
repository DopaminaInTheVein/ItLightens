#include "mcv_platform.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "comp_render_fade_screen.h"

// ---------------------
void TFadeScreenFX::renderInMenu() {
	if (ImGui::Checkbox("fade_in", &fade_in)) fade_out = !fade_in;
	if (ImGui::Checkbox("fade_out", &fade_out)) fade_in = !fade_out;
	ImGui::DragFloat("max timer fade", &t_max_fade);
	ImGui::Text("curr time: %f\n", t_curr_fade);
}

void TFadeScreenFX::update(float elapsed)
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
}

void TFadeScreenFX::SetMaxTime(float new_time) {
	t_max_fade = (new_time > 0) ? new_time : t_max_fade_default;
}

void TFadeScreenFX::FadeIn()
{
	fade_in = true;
	fade_out = false;
}

void TFadeScreenFX::FadeOut()
{
	fade_in = false;
	fade_out = true;
}

void TFadeScreenFX::render()
{
	if (t_curr_fade > 0) {
		shader_ctes_data.fade_black_screen = t_curr_fade / t_max_fade;
		shader_ctes_data.uploadToGPU();
		tech->activate();
		drawFullScreen(all_black, tech);	//wont use texture all_black
	}
}

bool TFadeScreenFX::load(MKeyValue& atts) {
	fade_in = atts.getBool("enabled", true);		//by default false on start
	fade_out = !fade_in;
	t_max_fade = t_max_fade_default = atts.getFloat("max time", 2.0f);
	t_curr_fade = 0.0f;

	return true;
}

void TFadeScreenFX::init()
{
	tech = Resources.get("fade_screen.tech")->as<CRenderTechnique>();
}