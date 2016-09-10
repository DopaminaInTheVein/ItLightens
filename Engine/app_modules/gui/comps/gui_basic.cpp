#include "mcv_platform.h"
#include "gui_basic.h"

#include "render\draw_utils.h"
#include "components\comp_transform.h"
#include "components\entity.h"

using namespace std;

map<string, GuiMatrix> TCompGui::gui_screens = map<string, GuiMatrix>();

void TCompGui::onCreate(const TMsgEntityCreated&)
{
	if (menu_name != "" && row >= 0 && col >= 0) {
		addGuiElement(menu_name, col, row, MY_OWNER);
	}
}

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
	menu_name = atts.getString("menu_name", "");
	row = atts.getInt("row", -1);
	col = atts.getInt("col", -1);

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

void TCompGui::addGuiElement(string menu_name, int row, int col, CHandle h_gui)
{
	auto it = gui_screens.find(menu_name);
	if (it != gui_screens.end()) {
		it->second.elem[row][col] = h_gui;
	}
	else {
		gui_screens[menu_name] = GuiMatrix();
		gui_screens[menu_name].elem[row][col] = h_gui;
	}
}

void TCompGui::clearScreen(string menu_name)
{
	gui_screens.erase(menu_name);
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

	//update size buttons
	CEntity* e_owner = CHandle(this).getOwner();
	TCompTransform* trans = e_owner->get<TCompTransform>();
	float offset = getRenderState();

	//reset offset
	if (offset > RSTATE_OVER) {
		offset = (RSTATE_RELEASED - offset) / RSTATE_RELEASED;
	}
	// +1 because default render state is 0
	float value = 1 + offset*0.25f;
	if (value != 0)
		trans->setScale(VEC3(value, value, value));
}

void TCompGui::uploadCtes() {
	//position
	shader_ctes_gui.pos_x = getTxCoords().x;
	shader_ctes_gui.pos_y = getTxCoords().y;

	//size
	shader_ctes_gui.size_x = getTxCoords().sx;
	shader_ctes_gui.size_y = getTxCoords().sy;
}
int TCompGui::GetRow()
{
	return row;
}
int TCompGui::GetCol()
{
	return col;
}