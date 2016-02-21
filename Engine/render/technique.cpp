#include "mcv_platform.h"
#include "technique.h"
#include "resources/resources_manager.h"
#include "shaders.h"

template<> IResource::eType getTypeOfResource<CRenderTechnique>() { return IResource::TECHNIQUE; }

// name = techniques/tech_solid_colored.tech
template<>
IResource* createObjFromName<CRenderTechnique>(const std::string& name) {
	CRenderTechnique* tech = new CRenderTechnique;
	const CVertexShader* vs = nullptr;
	const CPixelShader*  ps = nullptr;

	// ----------------------------------
	if (name == "tech_solid_colored.tech") {
		vs = Resources.get("solid_colored.vs")->as<CVertexShader>();
		ps = Resources.get("solid_colored.ps")->as<CPixelShader>();
	}
	else if (name == "tech_textured_colored.tech") {
		vs = Resources.get("textured_colored.vs")->as<CVertexShader>();
		ps = Resources.get("textured.ps")->as<CPixelShader>();
	}
	else {
		fatal("Unknown tech %s\n", name.c_str());
	}
	tech->create(name.c_str(), vs, ps);
	return tech;
}

void CRenderTechnique::destroy() {
	vs = nullptr;
	ps = nullptr;
}

bool CRenderTechnique::create(const char* new_name, const CVertexShader* new_vs, const CPixelShader* new_ps) {
	name = new_name;
	vs = new_vs;
	ps = new_ps;
	return true;
}

void CRenderTechnique::activate() const {
	assert(isValid());
	ps->activate();
	vs->activate();
}