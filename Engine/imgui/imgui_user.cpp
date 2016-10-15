#include "mcv_platform.h"
#include "imgui_user.h"
#include "imgui_internal.h"

bool ImGui::TreeNodeCheck(const char* label, bool* checked, bool &changed)
{
	//changed = ImGui::Checkbox("", checked);
	ImGui::PushID(checked);
	changed = ImGui::Checkbox("", checked);
	ImGui::SameLine();
	ImGui::PopID();

	return ImGui::TreeNode(label);
}