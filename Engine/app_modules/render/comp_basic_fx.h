#ifndef INC_COMP_BASIC_FX
#define INC_COMP_BASIC_FX

#include "components\comp_base.h"

class CRenderTechnique;

class TCompBasicFX {
public:
	int id;
	CHandle handle;
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
	virtual void renderInMenu() {
		ImGui::Text(getName());
	}
};

#endif