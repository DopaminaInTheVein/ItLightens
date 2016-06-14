#include "mcv_platform.h"
#include "comp_camera_main.h"
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

bool TCompCameraMain::load(MKeyValue& atts) {
	if (!TCompCamera::load(atts)) return false;
	detect_colsions = atts.getBool("collision", false);
	smoothCurrent = smoothDefault = 10.f;
	return true;
}


void TCompCameraMain::onGuidedCamera(const TMsgGuidedCamera& msg) {
	if (msg.guide.isValid()) guidedCamera = msg.guide;
	GameController->SetCinematic(true);
}

bool TCompCameraMain::getUpdateInfo() {
	transform = compBaseEntity->get<TCompTransform>();
	if (!transform) return false;

	return true;
}

void TCompCameraMain::update(float dt) {
	bool cameraIsGuided = false;
	if (guidedCamera.isValid()) {
		//Camara guida
		CEntity * egc = guidedCamera;
		TCompGuidedCamera * gc = egc->get<TCompGuidedCamera>();
		cameraIsGuided = gc->followGuide(transform, this);
		if (!cameraIsGuided) {
			//Fin recorrido  ...
			//... Guardamos guidedCamera para mensaje a Logic Manager
			CHandle cameraFinished = guidedCamera;

			//... Terminamos el modo cinematica
			guidedCamera = CHandle();
			GameController->SetCinematic(false);

			// Set the player in the 3rdPersonController
			CHandle t = tags_manager.getFirstHavingTag("player");
			//CEntity * target_e = t;
			if (t.isValid()) {
				TMsgSetTarget msg;
				msg.target = t;
				msg.who = PLAYER; //TODO: Siempre player?
				compBaseEntity->sendMsg(msg);		//set camera

				TMsgSetCamera msg_camera;
				msg_camera.camera = CHandle(this).getOwner();
				t.sendMsg(msg_camera);	//set target camera
			}

			smoothCurrent = 1.f; //Return to player smoothly
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCinematicEnd, string(egc->getName()), cameraFinished);
		}
	}
	if (!cameraIsGuided) {
		if (GameController->GetGameState() == CGameController::RUNNING && !GameController->GetFreeCamera()) {
			//if (owner.hasTag("camera_main")) {
			VEC3 pos = transform->getPosition();
			pos.y += 2;
			transform->setPosition(pos);
			if (detect_colsions) {
				if (!checkColision(pos))
					this->smoothLookAt(transform->getPosition(), transform->getPosition() + transform->getFront(), getUpAux());
			}
			if (!detect_colsions || !checkColision(pos)) {
				if (abs(smoothCurrent - smoothDefault) > 0.1f) smoothCurrent = smoothDefault * 0.05f + smoothCurrent * 0.95f;
				this->smoothLookAt(transform->getPosition(), transform->getPosition() + transform->getFront(), getUpAux(), smoothCurrent);
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
			if (compBaseEntity) {
				TCompController3rdPerson * obtarged = compBaseEntity->get<TCompController3rdPerson>();
				CHandle targetowner = obtarged->target;
				if (targetowner.isValid()) {
					CEntity* targeted = targetowner;
					TCompTransform * targettrans = targeted->get<TCompTransform>();
					if (targettrans) this->smoothLookAt(transform->getPosition(), targettrans->getPosition(), getUpAux());
				}
			}
		}
	}
}

bool TCompCameraMain::checkColision(const VEC3 & pos)
{
	//TODO:for now only for camera_main

	CEntity *e_me = compBaseEntity;
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