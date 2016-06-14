#include "mcv_platform.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "comp_render_fade_screen.h"

// ---------------------
void TCompFadeScreen::renderInMenu() {

	ImGui::Checkbox("start fade", &enabled);
	ImGui::DragFloat("max timer fade", &t_max_fade);
  
}

void TCompFadeScreen::ActiveFade()
{
	t_curr_fade = 0.0f;
	enabled = true;
}

void TCompFadeScreen::update(float elapsed)
{
	if (enabled) {
		t_curr_fade += elapsed;
		if (t_curr_fade > t_max_fade) {
			DeactivateFade();
		}
	}
}

void TCompFadeScreen::DeactivateFade()
{
	t_curr_fade = 0.0f;
	enabled = false;
}

void TCompFadeScreen::render()
{
	shader_ctes_data.fade_black_screen = t_curr_fade;
	tech->activate();
}


bool TCompFadeScreen::load(MKeyValue& atts) {
  enabled = atts.getBool("enabled", false);		//by default false on start
  t_max_fade = atts.getFloat("max time", 5.0f);
  t_curr_fade = 0.0f;

  return true;

}

void TCompFadeScreen::init()
{
	tech = Resources.get("blur_glow.tech")->as<CRenderTechnique>();
}

