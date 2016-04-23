#include "mcv_platform.h"
#include "gui_button_std.h"

#include "imgui/imgui_internal.h"

CGuiButtonStd::CGuiButtonStd(Rect r, float sz_font, char * txt)
{
	rect = r;
	sizeFont = sz_font * CApp::get().getYRes();
	text = txt;
}

bool CGuiButtonStd::render()
{
	ImGui::SetCursorPos(ImVec2(rect.x, rect.y));
	ImGui::PushFont(GImGui->Font);
	GImGui->FontSize = sizeFont;
	bool res  = (ImGui::Button(text, ImVec2(rect.sx, rect.sy)));
	ImGui::PopFont();
	return res;
}