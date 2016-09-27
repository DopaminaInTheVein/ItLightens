#include "mcv_platform.h"
#include "comp_light_point.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"

bool TCompLightPoint::load(MKeyValue& atts) {
	color = atts.getQuat("color");
	in_radius = atts.getFloat("in_radius", in_radius);
	out_radius = atts.getFloat("out_radius", out_radius);
	ttl = atts.getFloat("ttl", -999.0f);
	assert(out_radius >= in_radius);
	return true;
}

bool TCompLightPoint::save(std::ofstream& os, MKeyValue& atts) {
	atts.put("color", color);
	atts.put("in_radius", in_radius);
	atts.put("out_radius", out_radius);
	return true;
}

void TCompLightPoint::render() const
{
	if (debug_render) {
		CEntity *owner = CHandle(this).getOwner();
		if (!owner) return; //handle invalid

		TCompTransform* me_t = owner->get<TCompTransform>();
		auto axis = Resources.get("axis.mesh")->as<CMesh>();
		shader_ctes_object.World = me_t->asMatrix();
		shader_ctes_object.uploadToGPU();
		axis->activateAndRender();
	}
}

void TCompLightPoint::renderInMenu() {
	ImGui::ColorEdit3("Color", &color.x);
	ImGui::DragFloat("Intensity", &color.w);
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