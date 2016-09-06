#include "mcv_platform.h"
#include "gui_basic.h"

#include "render\draw_utils.h"

void TCompGui::setRenderTarget(float rs_target, float speed = FLT_MAX)
{
	render_target = rs_target;
	render_speed = speed;
}

void TCompGui::setRenderState(float rs_state)
{
	render_state = rs_state;
}

RectNormalized TCompGui::getTxCoords()
{
	return text_coords;
}

// load Xml
bool TCompGui::load(MKeyValue& atts)
{
	// Render state
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
	ImGui::Text("Text coords (x, sizeX, y, sizeY):");
	ImGui::DragFloat4("", (float*)(&text_coords), 0.01f, 0.f, 1.f);
}

#include "components\comp_transform.h"
#include "components\entity.h"
void TCompGui::update(float elapsed)
{
	//Check if needs to update
	if (render_speed == 0.f || render_state == render_target) return;

	//Apply speed
	float delta = getDeltaTime(true) * render_speed;
	if (render_target > render_state) {
		render_state = clamp(render_state + delta, render_state, render_target);
	}
	else if (render_target < render_state) {
		render_state = clamp(render_state - delta, render_target, render_state);
	}
}

void TCompGui::uploadCtes() {
	//position
	shader_ctes_gui.pos_x = getTxCoords().x;
	shader_ctes_gui.pos_y = getTxCoords().y;

	//size
	shader_ctes_gui.size_x = getTxCoords().sx;
	shader_ctes_gui.size_y = getTxCoords().sy;
}