#include "mcv_platform.h"
#include "comp_light_dir.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"

bool TCompLightDir::load(MKeyValue& atts) {
	TCompCamera::load(atts);
	color = atts.getQuat("color");

	std::string light_mask_name = atts.getString("light_mask", "textures/lightdir_mask.dds");
	light_mask = Resources.get(light_mask_name.c_str())->as<CTexture>();

	return true;
}
bool TCompLightDir::save(std::ofstream& os, MKeyValue& atts) {
	atts.put("color", color);
	atts.put("znear", getZNear());
	atts.put("zfar", getZFar());
	atts.put("fov", getFov());
	return true;
}
void TCompLightDir::render() const {
	if (debug_render) {
		PROFILE_FUNCTION("TCompLight render");
		auto axis = Resources.get("frustum.mesh")->as<CMesh>();
		shader_ctes_object.World = getViewProjection().Invert();
		shader_ctes_object.uploadToGPU();
		axis->activateAndRender();
	}
}

void TCompLightDir::renderInMenu() {
	TCompCamera::renderInMenu();
	ImGui::ColorEdit4("Color", &color.x, true);

	bool test;
	ImGui::Checkbox("SAVE LIGHT", &test);
	ImGui::Checkbox("HIDE LIGHT", &test);
}

void TCompLightDir::uploadShaderCtes(CEntity* e) {
	TCompTransform* trans = e->get<TCompTransform>();
	VEC3 lpos = trans->getPosition();
	shader_ctes_lights.LightWorldPos = VEC4(lpos);
	shader_ctes_lights.LightWorldFront = VEC4(trans->getFront());
	shader_ctes_lights.LightColor = this->color;
	shader_ctes_lights.LightViewProjection = this->getViewProjection();

	MAT44 offset = MAT44::CreateTranslation(0.5f, 0.5f, 0.f);
	MAT44 scale = MAT44::CreateScale(0.5f, -0.5f, 1.f);
	MAT44 tmx = scale * offset;
	shader_ctes_lights.LightViewProjectionOffset = shader_ctes_lights.LightViewProjection * tmx;
	shader_ctes_lights.LightOutRadius = this->getZFar();
	shader_ctes_lights.LightInRadius = this->getZNear();
	shader_ctes_lights.LightAspectRatio = this->getAspectRatio();
	shader_ctes_lights.LightCosFov = cosf(this->getFov());
	shader_ctes_lights.uploadToGPU();

	light_mask->activate(TEXTURE_SLOT_LIGHT_MASK);
}

void TCompLightDir::update(float dt) {
	CHandle owner = CHandle(this).getOwner();
	updateFromEntityTransform(owner);
}

void TCompLightDir::activate() {
	if (!enabled) return;
	activateWorldMatrix(getViewProjection().Invert());
	uploadShaderCtes(CHandle(this).getOwner());
}

//Editor
void TCompLightDir::start_editing() {
	original = new TCompLightDir;
	*original = *this;
	dbg("Test");
}