#include "mcv_platform.h"
#include "ai_cam.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_light_dir_shadows.h"
#include "components\entity.h"
#include "components\entity_tags.h"

map<string, statehandler> ai_cam::statemap = {};

map<int, string> ai_cam::out = {};

bool ai_cam::load(MKeyValue& atts) {
	maxRot = deg2rad(atts.getFloat("max_rotation", 120.0f));
	range = atts.getFloat("range", range);
	width = atts.getFloat("width", width);
	rot_speed_sonar = atts.getFloat("rot_speed_sonar", rot_speed_sonar);
	max_idle_waiting = atts.getFloat("max_idle_waiting", max_idle_waiting);
	rotatingR = atts.getBool("rotating_left", true);
	return true;
}

void ai_cam::Init() {
	//read main attributes from file
	full_name = "ai_cam_" + to_string(id_camera);

	if (statemap.empty()) {
		AddState("idle", (statehandler)&ai_cam::Idle);
		AddState("RotatingLeft", (statehandler)&ai_cam::RotatingLeft);
		AddState("RotatingRight", (statehandler)&ai_cam::RotatingRight);
		AddState("AimPlayer", (statehandler)&ai_cam::AimPlayer);
	}
	om = getHandleManager<ai_cam>();
	SetHandleMeInit();				//need to be initialized after all handles, ¿awake/init?
	SetMyEntity();
	idle_wait = 0.0f;

	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	TCompLightDirShadows *lshd = myEntity->get<TCompLightDirShadows>();
	PxRaycastBuffer hit;

	bool ret = g_PhysxManager->raycast(me_transform->getPosition(), VEC3(0.0f, -1.0f, 0.0f), 20.0f, hit);
	if (ret) {
		distToFloor = hit.getAnyHit(0).distance;
	}

	if (lshd) {
		VEC3 initialFront = me_transform->getFront();
		VEC3 myposinitial = me_transform->getPosition();
		VEC3 origin = me_transform->getPosition();
		myposinitial.y -= distToFloor;
		VEC3 lookingPoint = myposinitial + initialFront*range;
		float yaw, pitch;
		me_transform->getAngles(&yaw, &pitch);
		float pitchdiff = me_transform->getDeltaPitchToAimTo(lookingPoint);
		me_transform->setAngles(yaw, pitchdiff);

		VEC3 vec1 = origin - (myposinitial + initialFront*(range + width));
		VEC3 vec2 = origin - (myposinitial + initialFront*(range - width));

		float dot = vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
		float lenSq1 = vec1.x*vec1.x + vec1.y*vec1.y + vec1.z*vec1.z;
		float lenSq2 = vec2.x*vec2.x + vec2.y*vec2.y + vec2.z*vec2.z;
		float fov_in_rads = acos(dot / sqrt(lenSq1 * lenSq2));
		lshd->setNewFov(fov_in_rads);
	}
	else {
		fatal("camera needs a shadow camera component");
	}
	if (rotatingR) {
		ChangeState("RotatingRight");
	}
	else {
		ChangeState("RotatingLeft");
	}
}

void ai_cam::Idle() {
	//Nothing to do
	if (idle_wait > max_idle_waiting) {
		idle_wait = 0.0f;
		if (rotatingR) {
			ChangeState("RotatingRight");
		}
		else {
			ChangeState("RotatingLeft");
		}
	}
	else {
		idle_wait += getDeltaTime();
	}
}
bool ai_cam::playerInRange() {
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	// player detection
	CHandle hPlayer = tags_manager.getFirstHavingTag("raijin");
	CEntity * eplayer = hPlayer;
	TCompTransform * tplayer = eplayer->get<TCompTransform>();
	VEC3 myposinitial = me_transform->getPosition();
	myposinitial.y -= distToFloor;
	if (!me_transform->isHalfConeVision(tplayer->getPosition(), deg2rad(15.0f)) || squaredDist(tplayer->getPosition(), myposinitial) > 100) {
		return false;
	}
	VEC3 front = me_transform->getFront();
	front.y = 0.0f;
	front.Normalize();
	VEC3 destiny = myposinitial + front*range;
	VEC3 origin = me_transform->getPosition();

	VEC3 direction = origin - destiny;
	float height = origin.y - destiny.y;

	TCompCharacterController * cplayer = eplayer->get<TCompCharacterController>();

	float playerHeight = cplayer->GetHeight();

	VEC3 minplayerPos = tplayer->getPosition();
	VEC3 maxplayerPos = tplayer->getPosition();
	maxplayerPos.y += playerHeight;

	float factorMin = (minplayerPos.y - destiny.y) / height;
	float factorMax = (maxplayerPos.y - destiny.y) / height;

	VEC3 lookingPointMin = destiny + direction * factorMin;
	VEC3 lookingPointMax = destiny + direction * factorMax;
	bool b1 = realDistXZ(minplayerPos, lookingPointMin) < width;
	bool b2 = realDistXZ(maxplayerPos, lookingPointMax) < width;
	bool b = b1 || b2;
	if (b) {
		//raycast to look for down distance
		PxRaycastBuffer hit;
		CHandle hanHitted;
		VEC3 playerPos = minplayerPos;
		VEC3 playerPosUp = maxplayerPos;
		playerPosUp.y -= 0.1;
		VEC3 playerPosDown = minplayerPos;
		playerPosDown.y += 0.1f;
		playerPos.y += playerHeight / 2;
		VEC3 playerPosRight = (playerPos + me_transform->getLeft() * (cplayer->GetRadius()*0.9));
		VEC3 playerPosLeft = (playerPos + me_transform->getLeft() * (cplayer->GetRadius()*0.9));

		// raycasts
#ifndef NDEBUG
		Debug->DrawLine(origin, playerPos, VEC3(1.0f, 0.0f, 0.0f));
		Debug->DrawLine(origin, playerPosUp, VEC3(1.0f, 0.0f, 0.0f));
		Debug->DrawLine(origin, playerPosDown, VEC3(1.0f, 0.0f, 0.0f));
		Debug->DrawLine(origin, playerPosRight, VEC3(1.0f, 0.0f, 0.0f));
		Debug->DrawLine(origin, playerPosLeft, VEC3(1.0f, 0.0f, 0.0f));
#endif

		// player center
		VEC3 d1 = playerPos - origin;
		float dist1 = d1.Length();
		d1.Normalize();
		bool hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
		if (hitted) {
			hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (hPlayer == hanHitted) {
				return true;
			}
		}
		// player up
		d1 = playerPosUp - origin;
		dist1 = d1.Length();
		d1.Normalize();
		hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
		if (hitted) {
			hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (hPlayer == hanHitted) {
				return true;
			}
		}
		// player down
		d1 = playerPosDown - origin;
		dist1 = d1.Length();
		d1.Normalize();
		hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
		if (hitted) {
			hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (hPlayer == hanHitted) {
				return true;
			}
		}
		// player left
		d1 = playerPosLeft - origin;
		dist1 = d1.Length();
		d1.Normalize();
		hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
		if (hitted) {
			hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (hPlayer == hanHitted) {
				return true;
			}
		}
		// player right
		d1 = playerPosRight - origin;
		dist1 = d1.Length();
		d1.Normalize();
		hitted = g_PhysxManager->raycast(origin, d1, dist1, hit);
		if (hitted) {
			hanHitted = PhysxConversion::GetEntityHandle(*hit.getAnyHit(0).actor);
			if (hPlayer == hanHitted) {
				return true;
			}
		}
	}
	return false;
}

void ai_cam::RotatingLeft() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 front = me_transform->getFront();
	front.y = 0.0f;
	front.Normalize();
#ifndef NDEBUG
	VEC3 myposinitial = me_transform->getPosition();
	VEC3 origin = myposinitial;
	myposinitial.y -= distToFloor;
	Debug->DrawLine(origin, (myposinitial + front*(range + width)), VEC3(1.0f, 0.0f, 0.0f));
	Debug->DrawLine(origin, (myposinitial + front*(range - width)), VEC3(1.0f, 0.0f, 0.0f));
#endif

	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);
	float rot = rot_speed_sonar*getDeltaTime();
	rotatedTo += rot;
	me_transform->setAngles(yaw - rot, pitch);
	if (playerInRange()) {
		ChangeState("AimPlayer");
	}
	else if (rotatedTo > maxRot) {
		rotatedTo = 0.0f;
		rotatingR = true;
		ChangeState("RotatingRight");
	}
}

void ai_cam::RotatingRight() {
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	VEC3 front = me_transform->getFront();
	front.y = 0.0f;
	front.Normalize();
#ifndef NDEBUG
	VEC3 myposinitial = me_transform->getPosition();
	VEC3 origin = myposinitial;
	myposinitial.y -= distToFloor;
	Debug->DrawLine(origin, (myposinitial + front*(range + width)), VEC3(1.0f, 0.0f, 0.0f));
	Debug->DrawLine(origin, (myposinitial + front*(range - width)), VEC3(1.0f, 0.0f, 0.0f));
#endif

	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);
	float rot = rot_speed_sonar*getDeltaTime();
	rotatedTo += rot;
	me_transform->setAngles(yaw + rot_speed_sonar*getDeltaTime(), pitch);
	if (playerInRange()) {
		ChangeState("AimPlayer");
	}
	else if (rotatedTo > maxRot) {
		rotatedTo = 0.0f;
		rotatingR = false;
		ChangeState("RotatingLeft");
	}
}
void ai_cam::AimPlayer()
{
	SetMyEntity(); //needed in case address Entity moved by handle_manager
	if (!myEntity) return;
	TCompTransform *me_transform = myEntity->get<TCompTransform>();
	CHandle hPlayer = tags_manager.getFirstHavingTag("raijin");
	CEntity * eplayer = hPlayer;
	TCompTransform * tplayer = eplayer->get<TCompTransform>();

	float yaw, pitch;
	me_transform->getAngles(&yaw, &pitch);

	float aimtoplayer = me_transform->getDeltaYawToAimTo(tplayer->getPosition());

	if (aimtoplayer > 0.0f) {
		aimtoplayer = fminf(aimtoplayer, rot_speed_sonar * getDeltaTime());
	}
	else {
		aimtoplayer = fmaxf(aimtoplayer, -rot_speed_sonar * getDeltaTime());
	}
	rotatedTo += aimtoplayer;
	if (rotatedTo > 0.0f && rotatedTo < maxRot) {
		me_transform->setAngles(yaw + aimtoplayer, pitch);
	}
	if (playerInRange()) {
		TMsgNoise msg;
		msg.source = tplayer->getPosition();
		for (CHandle guardHandle : tags_manager.getHandlesByTag(getID("AI_guard"))) {
			CEntity * ePoss = guardHandle;
			ePoss->sendMsg(msg);
		}
	}
	else {
		idle_wait = 0.0f;
		ChangeState("idle");
	}
}

void ai_cam::renderInMenu()
{
	ImGui::Text("Node : %s", out[SBB::readInt(full_name)].c_str());
}

void ai_cam::SetHandleMeInit()
{
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
}

void ai_cam::SetMyEntity() {
	myEntity = myParent;
}