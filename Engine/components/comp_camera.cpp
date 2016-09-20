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
#include "app_modules/logic_manager/logic_manager.h"
#include "constants/ctes_object.h"
#include <math.h>

extern CShaderCte< TCteObject > shader_ctes_object;

#include "constants/ctes_camera.h"
extern CShaderCte< TCteCamera > shader_ctes_camera;

#include "comp_charactercontroller.h"

bool TCompCamera::load(MKeyValue& atts) {
	float znear = atts.getFloat("znear", 0.01f);
	float zfar = atts.getFloat("zfar", 1000.f);
	float fov_in_degs = atts.getFloat("fov", 70.f);
	float size_x = atts.getFloat("size_x", 10.f);
	float size_y = atts.getFloat("size_y", 10.f);
	bool use_ar = atts.getBool("use_ar", true);

	bool is_ortho = atts.getBool("is_ortho", false);
	float x = CApp::get().getXRes();
	float y = CApp::get().getYRes();
	if (is_ortho) setOrtho(size_x, size_y, znear, zfar, use_ar ? x / y : 1.f);
	else setProjection(deg2rad(fov_in_degs), znear, zfar);

	//setProjection(deg2rad(fov_in_degs), znear, zfar);

	return true;
}

void TCompCamera::onGetViewProj(const TMsgGetCullingViewProj& msg) {
	assert(msg.view_proj);
	*msg.view_proj = this->getViewProjection();
}

// Ojo! Esto solo va para camara mirando para z negativa, y up = (0,1,0) !!!
VEC3  TCompCamera::getMinOrtho() const
{
	GET_MY(tmx, TCompTransform);
	return (tmx ? tmx->getPosition() : VEC3()) + min_ortho;
}
VEC3  TCompCamera::getMaxOrtho() const
{
	GET_MY(tmx, TCompTransform);
	return (tmx ? tmx->getPosition() : VEC3()) + max_ortho;
}

void TCompCamera::render() const {
#ifndef NDEBUG
	PROFILE_FUNCTION("TCompCamera render");
	auto axis = Resources.get("frustum.mesh")->as<CMesh>();
	shader_ctes_object.World = getViewProjection().Invert();
	shader_ctes_object.uploadToGPU();
	axis->activateAndRender();
#endif
}

void TCompCamera::updateFromEntityTransform(CEntity* e_owner) {
	assert(e_owner);
	TCompTransform* tmx = e_owner->get<TCompTransform>();
	assert(tmx);
	this->smoothLookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());
}

void TCompCamera::update(float dt) {
	if (!isOrtho()) updateFromEntityTransform(compBaseEntity);
	else {
		GET_MY(tmx, TCompTransform);
		if (tmx) this->lookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());
	}
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