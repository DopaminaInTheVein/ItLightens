#include "mcv_platform.h"
#include "gui_basic.h"

#include "render\draw_utils.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "gui_cursor.h"
#include "../gui.h"

#include "app_modules/lang_manager/lang_manager.h"

using namespace std;

map<string, GuiMatrix> TCompGui::gui_screens = map<string, GuiMatrix>();

void TCompGui::onCreate(const TMsgEntityCreated&)
{
	if (menu_name != "" && row >= 0 && col >= 0) {
		addGuiElement(menu_name, col, row, MY_OWNER);
	}
	RenderManager.ModifyUI();
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
	if (!language) return text_coords;
	std::string lang_code = lang_manager->GetLanguage();
	RectNormalized sub_rect(0.f, 0.f, 0.5f, 0.5f);
	if (lang_code == "EN" || lang_code == "GA") sub_rect.y = .5f;
	if (lang_code == "CAT" || lang_code == "GA") sub_rect.x = .5f;
	return text_coords.subRect(sub_rect);
}
void TCompGui::setTxCoords(RectNormalized coords)
{
	text_coords = coords;
	text_coords_16 = text_coords * 16.f;
}
void TCompGui::setTxLetter(unsigned char letter)
{
	setTxCoords(Font::getTxtCoords(letter));
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
	width = atts.getFloat("width", 0.f);
	height = atts.getFloat("height", 0.f);
	color = VEC4(1, 1, 1, 1);
	language = atts.getBool("lang", false);
	size_world = atts.getFloat("size_world", -1.f);
	return true;
}

float TCompGui::GetWidth()
{
	return width;
}
float TCompGui::GetHeight()
{
	return height;
}

void TCompGui::SetWidth(float w)
{
	width = w;
}
void TCompGui::SetHeight(float h)
{
	height = h;
}

// Parent
void TCompGui::SetParent(CHandle h)
{
	if (h.isValid()) {
		parent = h;
		GET_COMP(gui, parent, TCompGui);
		menu_name = gui->GetMenuName();
	}
}

void TCompGui::renderInMenu()
{
	ImGui::Checkbox("Enabled", &enabled);
	IMGUI_SHOW_FLOAT(render_state);
	IMGUI_SHOW_FLOAT(render_speed);
	IMGUI_SHOW_FLOAT(render_target);
	ImGui::Separator();
	IMGUI_SHOW_BOOL(language);
	RectNormalized public_coords = getTxCoords();
	IMGUI_SHOW_FLOAT(public_coords.x);
	IMGUI_SHOW_FLOAT(public_coords.y);
	IMGUI_SHOW_FLOAT(public_coords.sx);
	IMGUI_SHOW_FLOAT(public_coords.sy);
	ImGui::Separator();
	ImGui::Text("Text coords (x, y, sizeX, sizeY):");
	static bool  text_coord_changed = false;
	if (ImGui::DragFloat4("Normalized", (float*)(&text_coords), 0.01f, 0.f, 1.f)) {
		if (!text_coord_changed) text_coords_16 = text_coords * 16.f;
	}
	ImGui::Text("Text coords * 16 (x, sizeX, y, sizeY):");
	if (ImGui::DragFloat4("Not normalized", (float*)(&text_coords_16), 1.f, 0.f, 16.f)) {
		text_coords = text_coords_16 / 16.f;
		text_coord_changed = true;
	}
	else {
		text_coord_changed = false;
	}
	ImGui::Separator();
	ImGui::DragFloat4("Color", &color.x, 0.001f, 0.f, 1.f);
}

CHandle TCompGui::getMatrixHandle(std::string menu_name, int row, int col)
{
	return gui_screens[menu_name].elem[row][col];
}

GuiMatrix TCompGui::getGuiMatrix(std::string menu_name)
{
	return gui_screens[menu_name];
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

CHandle TCompGui::getCursor()
{
	CHandle res = Gui->getCursor();
	if (res.isValid()) {
		GET_COMP(gui, res, TCompGui);
		if (menu_name != gui->GetMenuName()) res = CHandle();
	}
	return res;
}

#include "components\comp_transform.h"
#include "components\entity.h"
void TCompGui::update(float elapsed)
{
	if (parent != CHandle() && !parent.isValid()) {
		MY_OWNER.destroy();
		return;
	}

	if (loop_color) updateColorLag(elapsed);
	else updateColor(elapsed);
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
	if (value < 1.f) value = 1.f;
	trans->setScale(VEC3(value, value, value));
}

void TCompGui::uploadCtes() {
	//position
	shader_ctes_gui.pos_x = getTxCoords().x;
	shader_ctes_gui.pos_y = getTxCoords().y;

	//size
	shader_ctes_gui.size_x = getTxCoords().sx;
	shader_ctes_gui.size_y = getTxCoords().sy;

	shader_ctes_gui.color_ui = color;
}
int TCompGui::GetRow()
{
	return row;
}
int TCompGui::GetCol()
{
	return col;
}
#define checkColorReach(x) if (abs(prev_color.x - target_color.x) < abs(color.x - target_color.x)) color.x = target_color.x; else reach = false
void TCompGui::updateColor(float elapsed)
{
	if (color_speed > 0.f) {
		VEC4 prev_color = color;
		color += delta_color * color_speed;
		bool reach = true;
		checkColorReach(x);
		checkColorReach(y);
		checkColorReach(z);
		checkColorReach(w);
		if (reach) color_speed = 0.f;
	}
}
void TCompGui::updateColorLag(float elapsed)
{
	static float timeAcum = 0.0f;
	timeAcum += elapsed / 200;
	if (color_speed > 0.0f) {
		float mesura = timeAcum*color_speed - color_speed_lag;
		if (mesura > 0.0f) {
			float mesuramoduled = fmod(mesura, 2.0f);
			float proportion = mesuramoduled;
			if (mesuramoduled > 1.0f) {
				proportion = 2.0 - mesuramoduled;
			}
			color = origin_color + (target_color - origin_color)*proportion;
		}
	}
}

TCompGui::~TCompGui()
{
	RenderManager.ModifyUI();
}
