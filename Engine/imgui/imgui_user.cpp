#include "mcv_platform.h"
#include "imgui_user.h"
#include "imgui_internal.h"

bool ImGui::TreeNodeCheck(const char* label, bool* checked, bool &changed, bool check_box_enabled)
{
	//changed = ImGui::Checkbox("", checked);
	ImGui::PushID(checked);
	if (check_box_enabled) {
		changed = ImGui::Checkbox("", checked);
	}
	else {
		ImGui::Text(*checked ? "S" : " ");
	}
	ImGui::PopID();

	ImGui::SameLine();
	return ImGui::TreeNode(label);
}