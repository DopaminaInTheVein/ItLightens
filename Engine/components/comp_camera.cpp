#include "mcv_platform.h"
#include "comp_camera.h"
#include "comp_transform.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/draw_utils.h"
#include "entity.h"
#include "utils/XMLParser.h"
#include "imgui/imgui.h"

bool TCompCamera::load(MKeyValue& atts) {
  float znear = atts.getFloat("znear", 0.1f);
  float zfar = atts.getFloat("zfar", 1000.f);
  float fov_in_degs = atts.getFloat("fov", 70.f);
  setProjection(deg2rad(fov_in_degs), znear, zfar);
  return true;
}

void TCompCamera::render() const {
  auto axis = Resources.get("frustum.mesh")->as<CMesh>();
  shader_ctes_object.World = getViewProjection().Invert();
  shader_ctes_object.uploadToGPU();
  axis->activateAndRender();
}

void TCompCamera::update(float dt) {
  CHandle owner = CHandle(this).getOwner();
  CEntity* e_owner = owner;
  assert(e_owner);
  TCompTransform* tmx = e_owner->get<TCompTransform>();
  assert(tmx);
  this->lookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());
}

void TCompCamera::renderInMenu() {
  float fov_in_rad = getFov();
  float znear = getZNear();
  float zfar = getZFar();

  bool changed = false;
  float fov_in_deg = rad2deg(fov_in_rad);
  if (ImGui::SliderFloat("Fov", &fov_in_deg, 30.f, 110.f)) {
    changed = true;
    fov_in_rad = deg2rad(fov_in_deg);
  }
  changed |= ImGui::SliderFloat("ZNear", &znear, 0.01f, 2.f);
  changed |= ImGui::SliderFloat("ZFar", &zfar, 10.f, 1000.f);
  if( changed )
    setProjection(fov_in_rad, znear, zfar);
}
