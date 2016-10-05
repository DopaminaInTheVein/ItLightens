#include "mcv_platform.h"
#include "comp_basic_fx.h"

#include "app_modules/render/module_render_postprocess.h"

class CRenderTechnique;

void TCompBasicFX::Activate() {
	render_fx->ActivateFXAtEnd(getName(), 200);
}

void TCompBasicFX::renderInMenu() {
	ImGui::Separator();
	ImGui::Text(getName());
	if (ImGui::Button("Activate Default")) {
		Activate(); //should go as the last one always
	}
	if (ImGui::Button("Apply before UI")) {
		render_fx->ActivateFXBeforeUI(getName(), 200); //should go as the last one always
	}
	if (ImGui::Button("Apply at End")) {
		render_fx->ActivateFXAtEnd(getName(), 200); //should go as the last one always
	}
}