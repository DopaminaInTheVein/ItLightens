#include "mcv_platform.h"
#include "gui_utils.h"

void GUI::drawRect(Rect rect, ImColor color) {
	ImGui::GetWindowDrawList()->AddRectFilled(
		ImVec2(rect.x, rect.y),
		ImVec2(rect.x + rect.sx, rect.y + rect.sy),
		color
		);
}

Rect GUI::createRect(float x, float y, float sx, float sy) {
	int res_x = CApp::get().getXRes();
	int res_y = CApp::get().getYRes();
	return Rect(x* res_x, y*res_y, sx* res_x, sy*res_y);
}