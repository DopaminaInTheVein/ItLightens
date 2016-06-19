#include "mcv_platform.h"
#include "comp_camera.h"
#include "comp_controller_3rd_person.h"
#include "comp_transform.h"
#include "comp_guided_camera.h"
#include "comp_life.h"
#include "entity.h"
#include "entity_tags.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "entity.h"
#include "imgui/imgui.h"
#include "logic/sbb.h"
#include "logic/ai_beacon.h"
#include "logic/ai_cam.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "constants/ctes_object.h"
#include <math.h>

extern CShaderCte< TCteObject > shader_ctes_object;

#include "constants/ctes_camera.h"
extern CShaderCte< TCteCamera > shader_ctes_camera;

#include "comp_charactercontroller.h"

bool TCompCamera::load(MKeyValue& atts) {
  float znear = atts.getFloat("znear", 0.1f);
  float zfar = atts.getFloat("zfar", 1000.f);
  float fov_in_degs = atts.getFloat("fov", 70.f);

  bool is_ortho = atts.getBool("is_ortho", false);
  if (is_ortho) setOrtho(1024, 800, znear, zfar);
  else setProjection(deg2rad(fov_in_degs), znear, zfar);

  //setProjection(deg2rad(fov_in_degs), znear, zfar);

  return true;
}

void TCompCamera::onGetViewProj(const TMsgGetCullingViewProj& msg) {
  assert(msg.view_proj);
  *msg.view_proj = this->getViewProjection();
}

void TCompCamera::render() const {
  //auto axis = Resources.get("frustum.mesh")->as<CMesh>();
  shader_ctes_object.World = getViewProjection().Invert();
  shader_ctes_object.uploadToGPU();
  //axis->activateAndRender();
}

void TCompCamera::updateFromEntityTransform(CEntity* e_owner) {
  assert(e_owner);
  TCompTransform* tmx = e_owner->get<TCompTransform>();
  beacon_controller* beacon = e_owner->get<beacon_controller>();
  ai_cam* aicam = e_owner->get<ai_cam>();
  assert(tmx);
  if (beacon) {
    VEC3 lpos = tmx->getPosition();
    lpos.x += tmx->getFront().x / 2;
    lpos.y += 2.5f;
    lpos.z += tmx->getFront().z / 2;
    this->smoothLookAt(lpos, tmx->getPosition() + tmx->getFront()*beacon->getRange());
  }
  else if (aicam) {
    VEC3 lpos = tmx->getPosition();
    lpos.x += tmx->getFront().x / 2;
    lpos.z += tmx->getFront().z / 2;
    VEC3 destPos = tmx->getPosition() + tmx->getFront()*aicam->getRange();
    destPos.y -= aicam->getDistToFloor();
    this->smoothLookAt(lpos, destPos);
  }
  else {
    this->smoothLookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());
  }
}

void TCompCamera::update(float dt) {
  updateFromEntityTransform(compBaseEntity);
}

void TCompCamera::renderInMenu() {
  float fov_in_rad = getFov();
  float znear = getZNear();
  float zfar = getZFar();
  float ar = getAspectRatio();

  bool changed = false;
  //float fov_in_deg = rad2deg(fov_in_rad);
  //if (ImGui::SliderFloat("Fov", &fov_in_deg, 30.f, 110.f)) {
  //	changed = true;
  //	fov_in_rad = deg2rad(fov_in_deg);
  //}
  if (!isOrtho()) {
    float fov_in_deg = rad2deg(fov_in_rad);
    if (ImGui::SliderFloat("Fov", &fov_in_deg, 30.f, 110.f)) {
      changed = true;
      fov_in_rad = deg2rad(fov_in_deg);
    }
  }
  changed |= ImGui::SliderFloat("ZNear", &znear, 0.01f, 2.f);
  changed |= ImGui::SliderFloat("ZFar", &zfar, 10.f, 1000.f);
  if (changed)
    setProjection(fov_in_rad, znear, zfar);

  if (ImGui::SliderFloat("a/r", &ar, 0.f, 10.f)) {
    //setAspectRatio(ar);
  }
}