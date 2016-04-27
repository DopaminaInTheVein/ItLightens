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

void TCompLightDir::renderInMenu() {
  TCompCamera::renderInMenu();
  ImGui::ColorEdit4("Color", &color.x, true);
}

void TCompLightDir::uploadShaderCtes() {
  CEntity* e = CHandle(this).getOwner();
  TCompTransform* trans = e->get<TCompTransform>();
  shader_ctes_lights.LightWorldPos = VEC4(trans->getPosition());
  shader_ctes_lights.LightWorldFront = VEC4(trans->getFront());
  shader_ctes_lights.LightColor = this->color;
  shader_ctes_lights.LightViewProjection = this->getViewProjection();
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
  activateWorldMatrix(getViewProjection().Invert());
  uploadShaderCtes();
}