#include "mcv_platform.h"
#include "gui_basic.h"

void TCompGui::setRenderTarget(float rs_target, float speed = FLT_MAX)
{
	render_target = rs_target;
	render_speed = speed;
}

void TCompGui::setRenderState(float rs_state)
{
	render_state = rs_state;
}

// load Xml
bool TCompGui::load(MKeyValue& atts)
{
	render_state = atts.getFloat("render_state", 0.f);
	render_target = render_state;
	render_speed = 0.f;
	return true;
}

void TCompGui::renderInMenu()
{
	IMGUI_SHOW_FLOAT(render_state);
	IMGUI_SHOW_FLOAT(render_speed);
	IMGUI_SHOW_FLOAT(render_target);
}

void TCompGui::update(float elapsed)
{
	//Apply speed
	float delta = getDeltaTime(true) * render_speed;
	if (render_target > render_state) {
		render_state = clamp(render_state + delta, render_state, render_target);
	}
	else if (render_target < render_state) {
		render_state = clamp(render_state - delta, render_target, render_state);
	}
}