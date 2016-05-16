#include "mcv_platform.h"
#include "comp_transform.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "imgui/imgui.h"

#include "constants/ctes_object.h"

#include "handle\handle.h"
#include "components\entity.h"

extern CShaderCte< TCteObject > shader_ctes_object;

void TCompTransform::render() const
{
	auto axis = Resources.get("axis.mesh")->as<CMesh>();
	shader_ctes_object.World = asMatrix();
	shader_ctes_object.uploadToGPU();
	axis->activateAndRender();
}

bool TCompTransform::load(MKeyValue& atts) {
  auto p = atts.getPoint("pos");
  auto q = atts.getQuat("quat");
  auto s = atts.getFloat("scale", 1.0f);
  setPosition(p);
  setRotation(q);
  setScale(VEC3(s));
  if (atts.getString("lookat", "") != "") {
    auto target = atts.getPoint("lookat");
    lookAt(p, target, getUp());
  }
  return true;
}

void TCompTransform::renderInMenu() {
	VEC3 pos = getPosition();
	if (ImGui::DragFloat3("Pos", &pos.x, -0.1f, 0.1f)) {
		setPosition(pos);
	}

	float yaw, pitch;
	getAngles(&yaw, &pitch);
	yaw = rad2deg(yaw);
	pitch = rad2deg(pitch);
	bool yaw_changed = ImGui::DragFloat("Yaw", &yaw, -0.1f, 0.1f);
	bool pitch_changed = ImGui::DragFloat("Pitch", &pitch, -0.1f, 0.1f);
	if (yaw_changed || pitch_changed)
		setAngles(deg2rad(yaw), deg2rad(pitch));

	float scale = getScale().x;
	if (ImGui::DragFloat("Scale", &scale, -0.1f, 0.1f)) {
		setScale(VEC3(scale, scale, scale));
	}

	//Show front and left
	VEC3 f = getFront();
	VEC3 l = getLeft();
	ImGui::Text("Front: (%.2f, %.2f, %.2f), Left: (%.2f, %.2f, %.2f)",
		f.x, f.y, f.z, l.x, l.y, l.z);
}