#include "mcv_platform.h"
#include "comp_light_fadable.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"

bool TCompLightFadable::load(MKeyValue& atts) {
	color = atts.getQuat("color");
	in_radius = atts.getFloat("in_radius", in_radius);
	out_radius = atts.getFloat("out_radius", out_radius);
	ttl = atts.getFloat("ttl", 0.0f);
	assert(out_radius >= in_radius);
	return true;
}

void TCompLightFadable::update(float dt) {
	if (ttl >= 0) {
		// ttl only defined by player evolve/devolve
		//follow player
		CHandle t = tags_manager.getFirstHavingTag("player");
		CEntity * target_e = t;
		TCompTransform * pt = target_e->get<TCompTransform>();
		VEC3 ppos = pt->getPosition();
		ppos.y += 1.0f;
		CEntity * le = CHandle(this).getOwner();
		TCompTransform * lt = le->get<TCompTransform>();
		lt->setPosition(ppos);
		ttl -= getDeltaTime();
		if (ttl < 0) {
			in_radius = 0.0f;
			out_radius = 0.0001f;
			ppos.y += 1000.0f;
			lt->setPosition(ppos);
		}
	}
	else {
		auto hm3 = CHandleManager::getByName("light_fadable");
		CHandle h = CHandle(this).getOwner();
		h.destroy();
	}
}
void TCompLightFadable::renderInMenu() {
	ImGui::ColorEdit4("Color", &color.x, true);
	ImGui::InputFloat("in_radius", &in_radius);
	ImGui::InputFloat("out_radius", &out_radius);
}

void TCompLightFadable::uploadShaderCtes() {
	CEntity* e = CHandle(this).getOwner();
	TCompTransform* trans = e->get<TCompTransform>();
	shader_ctes_lights.LightWorldPos = VEC4(trans->getPosition());
	// LightViewProj not updated
	shader_ctes_lights.LightColor = this->color;
	shader_ctes_lights.LightInRadius = in_radius;
	shader_ctes_lights.LightOutRadius = out_radius;
	shader_ctes_lights.uploadToGPU();
}

void TCompLightFadable::activate() {
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