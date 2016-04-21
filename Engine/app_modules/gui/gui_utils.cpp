#include "mcv_platform.h"
#include "gui_utils.h"

void GUI::drawRect(Rect rect, ImColor color) {
	ImGui::GetWindowDrawList()->AddRectFilled(
		ImVec2(rect.x, rect.y),
		ImVec2(rect.sx, rect.sy),
		color
		);
}