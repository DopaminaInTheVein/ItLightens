#include "mcv_platform.h"
#include "comp_transform.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "render/draw_utils.h"

void TCompTransform::render() const {
  auto axis = Resources.get("axis.mesh")->as<CMesh>();
  activateWorldMatrix(asMatrix());
  axis->activateAndRender();
}

bool TCompTransform::load(MKeyValue& atts) {
  auto p = atts.getPoint("pos");
  auto q = atts.getQuat("quat");
  auto s = atts.getFloat("scale", 1.0f);
  setPosition(p);
  setRotation(q);
  setScale(VEC3(s));

  if (atts.has("yaw") || atts.has("pitch")) {
    float yaw, pitch;
    getAngles(&yaw, &pitch);
    yaw = deg2rad(atts.getFloat("yaw", rad2deg(yaw)));
    pitch = deg2rad(atts.getFloat("pitch", rad2deg(pitch)));
    setAngles(yaw, pitch);
  }

  if (atts.has("lookat")) {
    auto target = atts.getPoint("lookat");
    lookAt(p, target, getUp());
  }
  return true;
}

void TCompTransform::renderInMenu() {
  VEC3 pos = getPosition();
  float limit = 30.f;
  if (fabsf(pos.x) >= limit || fabsf(pos.y) >= limit || fabsf(pos.z) >= limit)
    limit = 300.f;
  if (fabsf(pos.x) >= limit || fabsf(pos.y) >= limit || fabsf(pos.z) >= limit)
    limit = 1000.f;
  if (ImGui::SliderFloat3("Pos", &pos.x, -limit, limit))
    setPosition(pos);

  float yaw, pitch, roll;
  getAngles(&yaw, &pitch, &roll);
  
  yaw = rad2deg(yaw);
  pitch = rad2deg(pitch);
  roll = rad2deg(roll);
  bool yaw_changed = ImGui::SliderFloat("Yaw", &yaw, -180.f, 180.f);
  bool pitch_changed = ImGui::SliderFloat("Pitch", &pitch, -90.f + 0.001f, 90.f - 0.001f);
  bool roll_changed = ImGui::SliderFloat("Roll", &roll, -180.f, 180.f);
  if (yaw_changed || pitch_changed || roll_changed) 
    setAngles(deg2rad(yaw), deg2rad(pitch), deg2rad(roll));

  VEC3 scale = getScale();
  if (ImGui::SliderFloat3("Scale", &scale.x, -10.f, 10.f))
    setScale(scale);

  // Drag to change the scale uniformily
  float unit_scale = 1.0f;
  float a = 0.01f;
  if (ImGui::SliderFloat("Uniform Scale", &unit_scale, 1.0f / ( 1.f + a ), 1.f + a))
    setScale(scale * unit_scale);
}
