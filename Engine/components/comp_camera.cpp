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

	setProjection(deg2rad(fov_in_degs), znear, zfar);
	detect_colsions = atts.getBool("collision", false);
	smoothDefault = smoothCurrent = 10.f;
	return true;
}

void TCompCamera::onGuidedCamera(const TMsgGuidedCamera& msg) {
	if (msg.guide.isValid()) guidedCamera = msg.guide;
	GameController->SetCinematic(true);
}

void TCompCamera::onGetViewProj(const TMsgGetCullingViewProj& msg) {
	assert(msg.view_proj);
	*msg.view_proj = this->getViewProjection();
}

void TCompCamera::render() const {
	auto axis = Resources.get("frustum.mesh")->as<CMesh>();
	shader_ctes_object.World = getViewProjection().Invert();
	shader_ctes_object.uploadToGPU();
	axis->activateAndRender();
}

void TCompCamera::updateFromEntityTransform(CEntity* e_owner) {
	assert(e_owner);
	TCompTransform* tmx = e_owner->get<TCompTransform>();
	assert(tmx);
	this->smoothLookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());
}

void TCompCamera::update(float dt) {
	CHandle owner = CHandle(this).getOwner();
	CEntity* e_owner = owner;
	assert(e_owner);
	TCompTransform* tmx = e_owner->get<TCompTransform>();
	assert(tmx);
	bool cameraIsGuided = false;
	if (guidedCamera.isValid()) {
		//Camara guida
		CEntity * egc = guidedCamera;
		TCompGuidedCamera * gc = egc->get<TCompGuidedCamera>();
		cameraIsGuided = gc->followGuide(tmx, this);
		if (!cameraIsGuided) {
			//Fin recorrido  ...
			//... Guardamos guidedCamera para mensaje a Logic Manager
			CHandle cameraFinished = guidedCamera;

			//... Terminamos el modo cinematica
			guidedCamera = CHandle();
			GameController->SetCinematic(false);

			// Set the player in the 3rdPersonController
			CHandle t = tags_manager.getFirstHavingTag("player");
			CEntity * target_e = t;
			if (e_owner && t.isValid()) {
				TMsgSetTarget msg;
				msg.target = t;
				msg.who = PLAYER; //TODO: Siempre player? 
				e_owner->sendMsg(msg);		//set camera

				TMsgSetCamera msg_camera;
				msg_camera.camera = owner;
				target_e->sendMsg(msg_camera);	//set target camera
			}

			smoothCurrent = 1.f; //Return to player smoothly
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCinematicEnd, string(egc->getName()), cameraFinished);
		}
	}
	if (!cameraIsGuided) {

		if (GameController->GetGameState() == CGameController::RUNNING && !GameController->GetFreeCamera()) {
			//if (owner.hasTag("camera_main")) {
				VEC3 pos = tmx->getPosition();
				if (owner.hasTag("camera_main")) {
					pos.y += 2;
				}
				tmx->setPosition(pos);
				if (detect_colsions) {
					if (!checkColision(pos))
						this->smoothLookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront(), getUpAux());
				}
				if (!detect_colsions || !checkColision(pos)) {
					if (abs(smoothCurrent - smoothDefault) > 0.1f) smoothCurrent = smoothDefault * 0.05f + smoothCurrent * 0.95f;
					this->smoothLookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront(), getUpAux(), smoothCurrent);
				}
			//}
		}
		else if (GameController->GetFreeCamera()) {
			CHandle owner = CHandle(this).getOwner();
			CEntity* e_owner = owner;
			assert(e_owner);
			TCompTransform* tmx = e_owner->get<TCompTransform>();
			assert(tmx);
			this->lookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());
		}
		else {
			if (e_owner) {
				TCompController3rdPerson * obtarged = e_owner->get<TCompController3rdPerson>();
				CHandle targetowner = obtarged->target;
				if (targetowner.isValid()) {
					CEntity* targeted = targetowner;
					TCompTransform * targettrans = targeted->get<TCompTransform>();
					if (targettrans) this->smoothLookAt(tmx->getPosition(), targettrans->getPosition(), getUpAux());
				}
			}
		}
	}
}

bool TCompCamera::checkColision(const VEC3 & pos)
{
	//TODO:for now only for camera_main

	CEntity *e_me = CHandle(this).getOwner();
	TCompController3rdPerson *c = e_me->get<TCompController3rdPerson>();
	TCompTransform *t = e_me->get<TCompTransform>();
	VEC3 real_pos = t->getPosition();

	CEntity *target = c->target;
	TCompCharacterController *cc = target->get<TCompCharacterController>();
	TCompTransform *targett = target->get<TCompTransform>();
	VEC3 pos_target = targett->getPosition() + VEC3(0, cc->GetHeight(), 0);

	VEC3 direction = real_pos - pos_target;
	direction.Normalize();
	float dist = c->GetPositionDistance();
	PxQueryFilterData fd = PxQueryFilterData();

	//fd.data.word0 = PXM_NO_PLAYER_CRYSTAL;	//will ignore crystals

	fd.data.word0 = PXM_CAMERA_COLLISIONS;

	pos_target += c->GetOffset() + VEC3(0, 1.0f, 0);

	//efficient raycast to search if there is some type of vision on the player
	//if it is possible to see the player will not be considered camera obstruction
	PxRaycastBuffer hit;
	bool collision = g_PhysxManager->raycast(pos_target, direction, dist, hit, fd);
	if (collision) {
		//if there is obstruction, look for good position
		PxSweepBuffer hit_sphere;
		collision = g_PhysxManager->raySphere(0.3f, pos_target, direction, dist, hit_sphere, fd);
		if (collision) {
			int i = 0;
			if (hit.getAnyHit(i).distance == 0.0f) {
				i++;
				if (hit.getNbTouches() == 1) return true;	//will not move the camera this frame if there are not any good position
			}
			VEC3 hit_pos = PhysxConversion::PxVec3ToVec3(hit_sphere.getAnyHit(i).position);
			smoothLookAt(hit_pos, pos_target, getUpAux());
		}
	}

	return collision;
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