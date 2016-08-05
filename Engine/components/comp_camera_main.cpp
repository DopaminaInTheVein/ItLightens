#include "mcv_platform.h"
#include "comp_camera_main.h"
#include "comp_controller_3rd_person.h"
#include "comp_transform.h"
#include "comp_guided_camera.h"
#include "comp_life.h"
#include "entity.h"
#include "entity_tags.h"
#include "player_controllers\player_controller.h"
#include "player_controllers\player_controller_mole.h"
#include "player_controllers\player_controller_cientifico.h"
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
			if (abs(smoothCurrent - smoothDefault) > 0.1f) smoothCurrent = smoothDefault * 0.05f + smoothCurrent * 0.95f;

			CEntity *e_me = compBaseEntity;
			TCompController3rdPerson *c = e_me->get<TCompController3rdPerson>();

			CEntity *target = c->target;
			TCompCharacterController *cc = target->get<TCompCharacterController>();
			TCompTransform *targett = target->get<TCompTransform>();
			VEC3 pos_target = targett->getPosition() + VEC3(0, cc->GetHeight(), 0);
			float factor = 0.8f;

			bool colision = false;
			if (detect_colsions) {
				player_controller * raijincontroller = target->get<player_controller>();
				player_controller_mole * molecontroller = target->get<player_controller_mole>();
				player_controller_cientifico * cientificocontroller = target->get<player_controller_cientifico>();

				if (raijincontroller) {
					colision = raijincontroller->getEnabled();
				}
				if (molecontroller) {
					colision = molecontroller->getEnabled();
					factor = 0.89f;
				}
				if (cientificocontroller) {
					colision = cientificocontroller->getEnabled();
					factor = 0.89f;
				}
			}
			colision = (colision ? checkColision(pos, smoothCurrent) : false);
			if (!colision) {
				this->smoothLookAt(transform->getPosition(), transform->getPosition() + transform->getFront(), getUpAux(), smoothCurrent);
				last_pos_camera = transform->getPosition();
			}
			else {
				VEC3 pos_cam = pos + (pos_target - pos)*factor;
				//pos_cam.y += cc->GetHeight();
				this->smoothLookAt(pos_cam, pos_cam + transform->getFront(), getUpAux(), smoothCurrent);
				last_pos_camera = transform->getPosition();
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

bool TCompCameraMain::checkColision(const VEC3 & pos, const float smoothCurrent)
{
	CEntity *e_me = compBaseEntity;
	TCompController3rdPerson *c = e_me->get<TCompController3rdPerson>();
	CEntity * target = c->target;
	//float dist = c->GetPositionDistance();

	PxQueryFilterData fd = PxQueryFilterData();

	fd.data.word0 = PXM_NO_PLAYER_NPC;

	//efficient raycast to search if there is some type of vision on the player
	//if it is possible to see the player will not be considered camera obstruction
	PxRaycastBuffer hit;
	bool collision = false;
	VEC3 direction;
	if (!collision) {
		direction = VEC3(1, 0, 0);
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(-1, 0, 0);
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, 0, 1);
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, 0, -1);
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		TCompCharacterController *cc = target->get<TCompCharacterController>();
		TCompTransform *targett = target->get<TCompTransform>();
		VEC3 pos_target = targett->getPosition() + VEC3(0, cc->GetHeight(), 0);
		// direction to player
		direction = pos_target - pos;
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, c->GetPositionDistance(), hit, fd);
		if (!collision) {
			direction = pos - pos_target;
			direction.Normalize();
			collision = g_PhysxManager->raycast(pos_target, direction, c->GetPositionDistance(), hit, fd);
		}
	}

	/*
	if (!collision) {
		direction = VEC3(1, 0, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(-1, 0, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(-1, 0, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(1, 0, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}*/

	return collision;
}