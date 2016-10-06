#ifndef INC_COMP_BASIC_FX
#define INC_COMP_BASIC_FX

#include "components\comp_base.h"
#include "app_modules/render/module_render_postprocess.h"
class CRenderTechnique;

class TCompBasicFX {
public:
	virtual void ApplyFX() = 0;

	const CRenderTechnique *tech;

	virtual void update(float dt) {
		//nothing to do
	}

	virtual void init() {
		//nothing to do
	}
	virtual void render() {
		//here will go the uploads on the GPU that are needed
	}

	virtual const char* getName() const = 0;

	virtual void Activate() {
		render_fx->ActivateFXAtEnd(getName(), 200);
	}

	virtual void renderInMenu() {
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
};

#endif
