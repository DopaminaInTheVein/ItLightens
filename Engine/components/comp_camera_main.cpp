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
	transform = GETH_MY(TCompTransform);
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
		GET_COMP(gc, guidedCamera, TCompGuidedCamera);
		cameraIsGuided = gc->followGuide(transform, this);
		if (!cameraIsGuided) {
			endGuidedCamera();
		}
	}
	if (!cameraIsGuided) {
		if (GameController->GetGameState() == CGameController::RUNNING && !GameController->GetFreeCamera()) {
			//if (owner.hasTag("camera_main")) {
			VEC3 pos = transform->getPosition();
			//pos.y += 2;
			//transform->setPosition(pos);
			if (abs(smoothCurrent - smoothDefault) > 0.1f) smoothCurrent = smoothDefault * 0.05f + smoothCurrent * 0.95f;

			GET_MY(c, TCompController3rdPerson);
			if (!c) return;

			CHandle target = c->target;
			if (!target.isValid()) return;
			GET_COMP(cc, target, TCompCharacterController);
			if (!cc) return;
			GET_COMP(targett, target, TCompTransform);
			if (!targett) return;
			VEC3 pos_target = targett->getPosition() + VEC3(0, cc->GetHeight(), 0);
			float max_factor = 0.8f;

			bool colision = false;
			collision_data collisionDistanceToCam;
			float distanceToTarget = c->GetPositionDistance();
			if (detect_colsions) {
				GET_COMP(raijincontroller, target, player_controller);
				GET_COMP(molecontroller, target, player_controller_mole);
				GET_COMP(cientificocontroller, target, player_controller_cientifico);

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
				colision = checkColision(pos, distanceToTarget, collisionDistanceToCam);
			}
			if (!colision) {
				this->smoothLookAt(transform->getPosition(), transform->getPosition() + transform->getFront(), getUpAux(), smoothCurrent);
				last_pos_camera = transform->getPosition();
			}
			else {
				float radius = cc->GetRadius();
				float other_factor = ((collisionDistanceToCam.dist + radius / 2));// distanceToTarget);
				float factor = fminf(max_factor, other_factor);
				VEC3 pos_cam = pos + (pos_target - pos)*factor;
				//VEC3 pos_target = pos_cam + transform->getFront();

				if (collisionDistanceToCam.intersection) {
					int n_iters = 0;
					while (collisionDistanceToCam.dist != 0.0f && n_iters < 10) {
						getPosIfColisionClipping(pos_cam, collisionDistanceToCam);
						//pos_cam -= collisionDistanceToCam.dir*collisionDistanceToCam.dist;
						other_factor += ((collisionDistanceToCam.dist + radius / 2));// distanceToTarget);
						factor = fminf(max_factor, other_factor);
						pos_cam = pos + (pos_target - pos)*factor;

						n_iters++;
					}
					this->smoothLookAt(pos_cam, pos_target, getUpAux(), smoothCurrent);
				}
				else {
					this->smoothLookAt(pos_cam, pos_target, getUpAux(), smoothCurrent);
				}
				last_pos_camera = transform->getPosition();
			}
		}
		else if (GameController->GetFreeCamera()) {
			GET_MY(tmx, TCompTransform);
			if (tmx) this->lookAt(tmx->getPosition(), tmx->getPosition() + tmx->getFront());
		}
		else {
			GET_MY(obtarged, TCompController3rdPerson);
			CHandle targetowner = obtarged->target;
			if (targetowner.isValid()) {
				GET_COMP(targettrans, targetowner, TCompTransform);
				if (targettrans) this->smoothLookAt(transform->getPosition(), targettrans->getPosition(), getUpAux());
			}
		}
	}
}

void TCompCameraMain::endGuidedCamera()
{
	//... Guardamos guidedCamera para mensaje a Logic Manager
	CHandle cameraFinished = guidedCamera;

	//... Terminamos el modo cinematica
	StopCinematic();

	smoothCurrent = 1.f; //Return to player smoothly
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCinematicEnd, string(((CEntity*)(cameraFinished))->getName()), cameraFinished);
}

bool TCompCameraMain::getPosIfColisionClipping(const VEC3 & pos, collision_data& result) {
	PxQueryFilterData fd = PxQueryFilterData();

	fd.data.word0 = PXM_NO_PLAYER_NPC;

	//efficient raycast to search if there is some type of vision on the player
	//if it is possible to see the player will not be considered camera obstruction
	result = collision_data();
	PxRaycastBuffer hit;
	bool collision = false;
	if (!collision) {
		result.dir = VEC3(1, 0, 0);
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(-1, 0, 0);
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, 0, 1);
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, 0, -1);
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, 1, 0);
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, -1, 0);
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result.dir = VEC3(0, -1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, 1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result.dir = VEC3(1, 0, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(-1, 0, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(1, 0, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(-1, 0, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result.dir = VEC3(1, -1, 0);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(-1, 1, 0);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(1, 1, 0);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(-1, -1, 0);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result.dir = VEC3(0, 1, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, -1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, 1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}
	if (!collision) {
		result.dir = VEC3(0, -1, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}

	if (!collision) {
		result.dir = VEC3(1, 1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result.dir = VEC3(1, 1, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result.dir = VEC3(1, -1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result.dir = VEC3(1, -1, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result.dir = VEC3(-1, 1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result.dir = VEC3(-1, 1, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result.dir = VEC3(-1, -1, 1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}	if (!collision) {
		result.dir = VEC3(-1, -1, -1);
		result.dir.Normalize();
		collision = g_PhysxManager->raycast(pos, result.dir, hitDistanceMin, hit, fd);
	}

	if (collision) {
		result.dir = result.dir;
		result.dist = hit.getAnyHit(0).distance;
	}
	else {
		result.dist = 0.0f;
	}
	return collision;
}

bool TCompCameraMain::checkColision(const VEC3 & pos, const float distanceToTarget, collision_data& result)
{
	GET_MY(c, TCompController3rdPerson);
	CHandle target = c->target;
	if (!target.isValid()) return nullptr;
	PxQueryFilterData fd = PxQueryFilterData();

	fd.data.word0 = PXM_NO_PLAYER_NPC;

	//efficient raycast to search if there is some type of vision on the player
	//if it is possible to see the player will not be considered camera obstruction
	PxRaycastBuffer hit;
	VEC3 direction;

	GET_COMP(cc, target, TCompCharacterController);
	if (!cc) return nullptr;

	GET_COMP(targett, target, TCompTransform);
	if (!targett) return nullptr;
	VEC3 pos_target = targett->getPosition() + VEC3(0, cc->GetHeight(), 0);
	// direction to player
	direction = pos - pos_target;
	direction.Normalize();

	bool collision = false;
	collision = g_PhysxManager->raycast(pos_target, direction, c->GetPositionDistance(), hit, fd);

	if (collision) {
		result.dist = distanceToTarget - hit.getAnyHit(0).distance + 0.1f;
		result.intersection = true;
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
		result.dist = hitDistance;
	}

	return collision;
}

void TCompCameraMain::StopCinematic()
{
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
		MY_OWNER.sendMsg(msg);		//set camera

		TMsgSetCamera msg_camera;
		msg_camera.camera = CHandle(this).getOwner();
		t.sendMsg(msg_camera);	//set target camera
	}
}

void TCompCameraMain::reset()
{
	StopCinematic();
	setManualControl(false);
	GameController->SetManualCameraState(false);
	// restore normal controls
	TMsgSetControllable msg;
	msg.control = true;
	MY_OWNER.sendMsg(msg);
	GET_MY(cam_control, TCompController3rdPerson);
	if (cam_control) cam_control->StopOrbit();

	CHandle player = CPlayerBase::handle_player;
	if (player.isValid()) {
		GET_COMP(player_tmx, player, TCompTransform);
		if (player_tmx) {
			GET_MY(cam_tmx, TCompTransform);
			if (cam_tmx) cam_tmx->setPosition(player_tmx->getPosition());
		}
	}
}