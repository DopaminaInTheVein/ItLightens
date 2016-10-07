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
#include "app_modules/gui/gui.h"
#include "constants/ctes_object.h"
#include <math.h>

extern CShaderCte< TCteObject > shader_ctes_object;

#include "constants/ctes_camera.h"
extern CShaderCte< TCteCamera > shader_ctes_camera;

#include "comp_charactercontroller.h"

CHandle TCompCameraMain::prev_camera_main = CHandle();

void TCompCameraMain::init()
{
	CHandle player = tags_manager.getFirstHavingTag("player");
	TMsgSetTarget msg;
	msg.target = player;
	TMsgGetWhoAmI msgWho;
	player.sendMsgWithReply(msgWho);
	msg.who = msgWho.who;
	CHandle hMe = CHandle(this).getOwner();
	hMe.sendMsg(msg);

	TMsgSetCamera msgCam;
	msgCam.camera = hMe;
	player.sendMsg(msgCam);
}

bool TCompCameraMain::load(MKeyValue& atts) {
	if (!TCompCamera::load(atts)) return false;
	detect_colsions = atts.getBool("collision", true);
	smoothCurrent = smoothDefault = 10.f;
	return true;
}

void TCompCameraMain::onCreate(const TMsgEntityCreated&)
{
	if (prev_camera_main.isValid()) prev_camera_main.destroy();
	prev_camera_main = CHandle(this).getOwner();
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
void TCompCameraMain::skipCinematic() {
	if (guidedCamera.isValid()) {
		GET_COMP(gcam, guidedCamera, TCompGuidedCamera);
		gcam->skip();
	}
}
void TCompCameraMain::update(float dt) {
	bool cameraIsGuided = false;

	if (manual_control) { //is_ui_control check needed?
		return;
	}
	if (Gui->IsUiControl()) return;
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
			TMsgGetWhoAmI msg_who;
			t.sendMsgWithReply(msg_who);
			//CEntity * target_e = t;
			if (t.isValid()) {
				TMsgSetTarget msg;
				msg.target = t;
				msg.who = msg_who.who;
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
			if (!c) return;

			CEntity *target = c->target;
			if (!target) return;
			TCompCharacterController *cc = target->get<TCompCharacterController>();
			if (!cc) return;
			TCompTransform *targett = target->get<TCompTransform>();
			if (!targett) return;
			VEC3 pos_target = targett->getPosition() + VEC3(0, cc->GetHeight(), 0);
			float max_factor = 0.8f;

			bool colision = false;
			collision_data* collisionDistanceToCam = nullptr;
			float distanceToTarget = c->GetPositionDistance();
			if (detect_colsions) {
				player_controller * raijincontroller = target->get<player_controller>();
				player_controller_mole * molecontroller = target->get<player_controller_mole>();
				player_controller_cientifico * cientificocontroller = target->get<player_controller_cientifico>();

				if (raijincontroller) {
					colision = raijincontroller->getEnabled();
				}
				else if (molecontroller) {
					colision = molecontroller->getEnabled();
					max_factor = 0.89f;
				}
				else if (cientificocontroller) {
					colision = cientificocontroller->getEnabled();
					max_factor = 0.89f;
				}
				else {
					return;
				}
			}
			if (colision) {
				collisionDistanceToCam = checkColision(pos, distanceToTarget);
				colision = collisionDistanceToCam != nullptr;
			}
			if (!colision) {
				this->smoothLookAt(transform->getPosition(), transform->getPosition() + transform->getFront(), getUpAux(), smoothCurrent);
				last_pos_camera = transform->getPosition();
			}
			else {
				float factor = fminf(max_factor, ((collisionDistanceToCam->dist + cc->GetRadius() / 2) / distanceToTarget));
				VEC3 pos_cam = pos + (pos_target - pos)*factor;
				//VEC3 pos_target = pos_cam + transform->getFront();

				if (collisionDistanceToCam->intersection) {
					int n_iters = 0;
					while (collisionDistanceToCam->dist != 0.0f && n_iters < 10) {
						collisionDistanceToCam = getPosIfColisionClipping(pos_cam);
						pos_cam -= collisionDistanceToCam->dir*collisionDistanceToCam->dist;
						n_iters++;
					}
					//char n_iters_c[10];
					//sprintf(n_iters_c, "%d\n", n_iters);
					//Debug->LogError(n_iters_c);
					this->smoothLookAt(pos_cam, pos_target, getUpAux(), smoothCurrent);
				}
				else {
					this->smoothLookAt(pos_cam, pos_target, getUpAux(), smoothCurrent);
				}
				last_pos_camera = transform->getPosition();
			}
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

collision_data* TCompCameraMain::getPosIfColisionClipping(const VEC3 & pos) {
	PxQueryFilterData fd = PxQueryFilterData();

	fd.data.word0 = PXM_NO_PLAYER_NPC;

	//efficient raycast to search if there is some type of vision on the player
	//if it is possible to see the player will not be considered camera obstruction
	PxRaycastBuffer hit;
	bool collision = false;
	collision_data* result = new collision_data();
	if (!collision) {
		result->dir = VEC3(1, 0, 0);
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(-1, 0, 0);
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, 0, 1);
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, 0, -1);
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, 1, 0);
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, -1, 0);
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result->dir = VEC3(0, -1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, 1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result->dir = VEC3(1, 0, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(-1, 0, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(1, 0, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(-1, 0, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result->dir = VEC3(1, -1, 0);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(-1, 1, 0);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(1, 1, 0);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(-1, -1, 0);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result->dir = VEC3(0, 1, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, -1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, 1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result->dir = VEC3(0, -1, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result->dir = VEC3(1, 1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result->dir = VEC3(1, 1, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result->dir = VEC3(1, -1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result->dir = VEC3(1, -1, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result->dir = VEC3(-1, 1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result->dir = VEC3(-1, 1, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result->dir = VEC3(-1, -1, 1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result->dir = VEC3(-1, -1, -1);
		result->dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result->dir, hitDistanceMin, hit, fd);
	}

	if (collision) {
		result->dir = result->dir;
		result->dist = hit.getAnyHit(0).distance;
	}
	else {
		result->dist = 0.0f;
	}
	return result;
}

collision_data* TCompCameraMain::checkColision(const VEC3 & pos, const float distanceToTarget)
{
	CEntity *e_me = compBaseEntity;
	TCompController3rdPerson *c = e_me->get<TCompController3rdPerson>();
	CEntity * target = c->target;
	//float dist = c->GetPositionDistance();
	if (!target) return false;
	PxQueryFilterData fd = PxQueryFilterData();

	fd.data.word0 = PXM_NO_PLAYER_NPC;

	//efficient raycast to search if there is some type of vision on the player
	//if it is possible to see the player will not be considered camera obstruction
	PxRaycastBuffer hit;
	bool collision = false;
	VEC3 direction;
	collision_data* result = new collision_data();

	TCompCharacterController *cc = target->get<TCompCharacterController>();
	if (!cc) return nullptr;
	TCompTransform *targett = target->get<TCompTransform>();
	if (!targett) return nullptr;
	VEC3 pos_target = targett->getPosition() + VEC3(0, cc->GetHeight(), 0);
	// direction to player
	direction = pos - pos_target;
	direction.Normalize();
	collision = g_PhysxManager->raycast(pos_target, direction, c->GetPositionDistance(), hit, fd);

	if (collision) {
		result->dist = distanceToTarget - hit.getAnyHit(0).distance + 0.1f;
		result->intersection = true;
		return result;
	}
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
		direction = VEC3(0, 1, 0);
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, -1, 0);
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, -1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, 1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}

	if (!collision) {
		direction = VEC3(1, 0, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(-1, 0, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
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
		direction = VEC3(1, -1, 0);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(-1, 1, 0);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(1, 1, 0);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(-1, -1, 0);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}

	if (!collision) {
		direction = VEC3(0, 1, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, -1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, 1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}
	if (!collision) {
		direction = VEC3(0, -1, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}

	if (!collision) {
		direction = VEC3(1, 1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}	if (!collision) {
		direction = VEC3(1, 1, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}	if (!collision) {
		direction = VEC3(1, -1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}	if (!collision) {
		direction = VEC3(1, -1, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}	if (!collision) {
		direction = VEC3(-1, 1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}	if (!collision) {
		direction = VEC3(-1, 1, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}	if (!collision) {
		direction = VEC3(-1, -1, 1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}	if (!collision) {
		direction = VEC3(-1, -1, -1);
		direction.Normalize();
		collision = g_PhysxManager->raycast(pos, direction, hitDistance, hit, fd);
	}

	if (collision) {
		result->dist = hitDistance;
		return result;
	}

	return nullptr;
}