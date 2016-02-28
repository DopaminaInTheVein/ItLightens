#include "mcv_platform.h"
#include "comp_camera.h"
#include "comp_controller_3rd_person.h"
#include "comp_transform.h"
#include "comp_life.h"
#include "entity.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "entity.h"
#include "utils/XMLParser.h"
#include "imgui/imgui.h"

#include "contants/ctes_object.h"
extern CShaderCte< TCteObject > shader_ctes_object;

#include "contants/ctes_camera.h"
extern CShaderCte< TCteCamera > shader_ctes_camera;

bool TCompCamera::load(MKeyValue& atts) {
	float znear = atts.getFloat("znear", 0.1f);
	float zfar = atts.getFloat("zfar", 1000.f);
	float fov_in_degs = atts.getFloat("fov", 70.f);
	setProjection(deg2rad(fov_in_degs), znear, zfar);
	return true;
}

void TCompCamera::render() const {
	//auto axis = Resources.get("frustum.mesh")->as<CMesh>();
	//shader_ctes_object.World = getViewProjection().Invert();
	//shader_ctes_object.uploadToGPU();
	//axis->activateAndRender();

	shader_ctes_camera.activate(CTE_SHADER_CAMERA_SLOT);
	shader_ctes_camera.ViewProjection = getViewProjection();
	shader_ctes_camera.uploadToGPU();
}

void TCompCamera::update(float dt) {
	CHandle owner = CHandle(this).getOwner();
	CEntity* e_owner = owner;
	assert(e_owner);
	TCompTransform* tmx = e_owner->get<TCompTransform>();
	assert(tmx);
	TCompController3rdPerson * obtarged = e_owner->get<TCompController3rdPerson>();
	CHandle targetowner = obtarged->target;
	CEntity* targeted = targetowner;
	TCompLife * targetlife = targeted->get<TCompLife>();
	TCompTransform * targettrans = targeted->get<TCompTransform>();

	if (targetlife->currentlife > 0.0f) {
		VEC3 pos = tmx->getPosition();
		pos.y += 2;
		tmx->setPosition(pos);
		this->smoothLookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront(), getUpAux());	//smooth movement
	}
	else {
		this->smoothLookAt(tmx->getPosition(), targettrans->getPosition(), getUpAux());	//smooth movement
	}
	//this->lookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());		//too robotic
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
	if (changed)
		setProjection(fov_in_rad, znear, zfar);
}