#include "mcv_platform.h"
#include "comp_light_point.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"

bool TCompLightPoint::load(MKeyValue& atts) {
	color = atts.getQuat("color");
	in_radius = atts.getFloat("in_radius", in_radius);
	out_radius = atts.getFloat("out_radius", out_radius);
	ttl = atts.getFloat("ttl", -999.0f);
	assert(out_radius >= in_radius);
	return true;
}

void TCompLightPoint::renderInMenu() {
	ImGui::ColorEdit4("Color", &color.x, true);
	ImGui::InputFloat("in_radius", &in_radius);
	ImGui::InputFloat("out_radius", &out_radius);
}

void TCompLightPoint::uploadShaderCtes() {
	CEntity* e = CHandle(this).getOwner();
	TCompTransform* trans = e->get<TCompTransform>();
	shader_ctes_lights.LightWorldPos = VEC4(trans->getPosition());
	// LightViewProj not updated
	shader_ctes_lights.LightColor = this->color;
	shader_ctes_lights.LightInRadius = in_radius;
	shader_ctes_lights.LightOutRadius = out_radius;
	shader_ctes_lights.uploadToGPU();
}

void TCompLightPoint::activate() {
	CEntity* e = CHandle(this).getOwner();
	assert(e);
	TCompTransform* trans = e->get<TCompTransform>();
	assert(trans);

	// So the unit sphere goes to the proper place in screen
	// 1.05 because the unit_sphere is inscribed inside the
	// perfect geometry
	MAT44 world =
		MAT44::CreateScale(out_radius * 1.05f)
		*
		MAT44::CreateTranslation(trans->getPosition());
	activateWorldMatrix(world);

	uploadShaderCtes();
}