#include "mcv_platform.h"
#include "gui_utils.h"

#include "app_modules/gui/gui.h"

void GUI::drawRect(const Rect& rect, const ImColor& color) {
	ImGui::GetWindowDrawList()->AddRectFilled(
		ImVec2(rect.x, rect.y),
		ImVec2(rect.x + rect.sx, rect.y + rect.sy),
		color
	);
}

void GUI::drawText(float x, float y, ImFont * font, float size, ImColor color, const char * text) {
	int res_x = CApp::get().getXRes();
	int res_y = CApp::get().getYRes();
	Pixel p(x * res_x, y * res_y);
	drawText(p, font, size, color, text);
}

void GUI::drawText(Pixel p, ImFont * font, float size, ImColor color, const char * text) {
	if (Gui->window_actived) {
		float sizeFont = size * CApp::get().getYRes();

		ImGui::GetWindowDrawList()->AddText(
			font,
			sizeFont,
			ImVec2(p.x, p.y),
			color,
			text
		);
	}
	Gui->there_is_text = true;
}

void GUI::drawImage(float x1, float y1, float x2, float y2, ImTextureID id_tex) {
	int res_x = CApp::get().getXRes();
	int res_y = CApp::get().getYRes();
	Pixel p(x1 * res_x, y1 * res_y);
	Pixel q(x2 * res_x, y2 * res_y);
	drawImage(p, q, id_tex);
}

void GUI::drawImage(Pixel p, Pixel q, ImTextureID id_tex) {
	ImGui::GetWindowDrawList()->AddImage(id_tex, ImVec2(p.x, p.y), ImVec2(q.x, q.y), ImVec2(0, 0), ImVec2(1, 1));
}

Rect GUI::createRect(float x, float y, float sx, float sy) {
	int res_x = CApp::get().getXRes();
	int res_y = CApp::get().getYRes();
	return Rect(x* res_x, y*res_y, sx* res_x, sy*res_y);
}