#include "mcv_platform.h"
#include "comp_light_dir.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"

bool TCompLightDir::load(MKeyValue& atts) {
	TCompCamera::load(atts);
	color = atts.getQuat("color");
	return true;
}

void TCompLightDir::renderInMenu() {
	TCompCamera::renderInMenu();
	ImGui::ColorEdit4("Color", &color.x, true);
}

void TCompLightDir::uploadShaderCtes() {
	CEntity* e = CHandle(this).getOwner();
	TCompTransform* trans = e->get<TCompTransform>();
  shader_ctes_lights.LightWorldPos = VEC4(trans->getPosition());
  shader_ctes_lights.LightColor = this->color;
  shader_ctes_lights.LightViewProjection = this->getViewProjection();
  shader_ctes_lights.uploadToGPU();
}

void TCompLightDir::update(float dt) {
  CHandle owner = CHandle(this).getOwner();
  updateFromEntityTransform(owner);
}

void TCompLightDir::activate() {
  activateWorldMatrix(getViewProjection().Invert());
  uploadShaderCtes();
}