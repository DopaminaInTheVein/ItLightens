#include "mcv_platform.h"
#include "comp_transform.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "utils/XMLParser.h"
#include "imgui/imgui.h"

#include "contants/ctes_object.h"
extern CShaderCte< TCteObject > shader_ctes_object;

void TCompTransform::render() const {
	auto axis = Resources.get("axis.mesh")->as<CMesh>();
	shader_ctes_object.World = asMatrix();
	shader_ctes_object.uploadToGPU();
	axis->activateAndRender();
}

bool TCompTransform::load(MKeyValue& atts) {
	auto p = atts.getPoint("pos");
	setPosition(p);
	return true;
}

void TCompTransform::renderInMenu() {
	VEC3 pos = getPosition();
	if (ImGui::SliderFloat3("Pos", &pos.x, -10.f, 10.f)) {
		setPosition(pos);
	}

	float yaw, pitch;
	getAngles(&yaw, &pitch);
	yaw = rad2deg(yaw);
	pitch = rad2deg(pitch);
	bool yaw_changed = ImGui::SliderFloat("Yaw", &yaw, -180.f, 180.f);
	bool pitch_changed = ImGui::SliderFloat("Pitch", &pitch, -90.f + 0.001f, 90.f - 0.001f);
	if (yaw_changed || pitch_changed)
		setAngles(deg2rad(yaw), deg2rad(pitch));
}