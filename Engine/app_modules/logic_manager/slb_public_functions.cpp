#include "mcv_platform.h"
#include "slb_public_functions.h"
#include "handle/handle_manager.h"
#include "handle/handle.h"
#include "components/comp_name.h"
#include "components/entity.h"
#include "components/entity_parser.h"
#include "components/comp_charactercontroller.h"
#include "components/comp_life.h"
#include "logic/bt_guard.h"

using namespace IdEntities;

// player functions
void SLBPlayer::getPlayer() {
	VHandles targets = tags_manager.getHandlesByTag(getID("player"));
	CHandle thePlayer = targets[targets.size() - 1];
	player_handle = thePlayer;
}

void SLBPlayer::setPlayerPosition(float x, float y, float z) {
	getPlayer();
	const PxVec3 new_position(x, y, z);

	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	entity_controller->teleport(new_position);
}

void SLBPlayer::teleport(const char * point_name) {
	getPlayer();
	if (player_handle.isValid()) {
		CHandle target = tags_manager.getHandleByTagAndName("teleport", "WirePosUp");
		GET_COMP(tTarget, target, TCompTransform);
		GET_COMP(tPlayer, player_handle, TCompTransform);
		GET_COMP(ccPlayer, player_handle, TCompCharacterController);
		if (tPlayer && tTarget && ccPlayer) {
			ccPlayer->teleport(tTarget->getPosition());
			tPlayer->setYaw(tTarget->getYaw());
		}
	}
}

float SLBPlayer::getPlayerX() {
	getPlayer();
	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().x;
}

float SLBPlayer::getPlayerY() {
	getPlayer();
	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().y;
}

float SLBPlayer::getPlayerZ() {
	getPlayer();
	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().z;
}

void SLBPlayer::addEnergy(int energy_to_add) {
	getPlayer();
	CEntity* entity = player_handle;

	TCompLife* player_life = entity->get<TCompLife>();

	player_life->currentlife += energy_to_add;
}

void SLBPlayer::refillEnergy() {
	getPlayer();
	CEntity* entity = player_handle;

	TCompLife* player_life = entity->get<TCompLife>();

	player_life->currentlife = player_life->maxlife;
}

// generic handle function
CHandle SLBHandle::getHandle() {
	return real_handle;
}

// We need ti aply generic actions to player as well
void SLBHandle::getPlayer() {
	VHandles targets = tags_manager.getHandlesByTag(getID("player"));
	CHandle thePlayer = targets[targets.size() - 1];
	real_handle = thePlayer;
}

void SLBHandle::getHandleById(int id) {
	CHandle handle = IdEntities::findById(id);
	real_handle = handle;
	if (real_handle.isValid()) {
		dbg("[LUA] getHandleById: %s\n", ((CEntity*)(real_handle))->getName());
	}
	else {
		dbg("[LUA] getHandleById: Error Unknown Id!\n");
	}
}

void SLBHandle::getHandleByNameTag(const char* name, const char* tag) {
	handle_name = std::string(name);
	handle_tag = std::string(tag);
	real_handle = tags_manager.getHandleByTagAndName(tag, name);
}

void SLBHandle::getHandleCaller() {
	real_handle = logic_manager->getCaller();
}

void SLBHandle::destroy() {
	if (real_handle.isValid()) real_handle.destroy();
}

void SLBHandle::setPosition(float x, float y, float z) {
	const PxVec3 new_position(x, y, z);

	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	entity_controller->teleport(new_position);
}

float SLBHandle::getX() {
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().x;
}

float SLBHandle::getY() {
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().y;
}

float SLBHandle::getZ() {
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().z;
}

void SLBHandle::goToPoint(float x, float y, float z) {
	VEC3 dest(x, y, z);
	// depending on the tag, we need to use a different manager
	if (handle_tag.find("guard") != std::string::npos) {
		auto guard = getHandleManager<bt_guard>()->getAddrFromHandle(real_handle);
		guard->goToPoint(dest);
	}
}
void SLBHandle::goAndLookAs(SLBHandle target, std::string code_arrived) {
	CHandle hTarget = target.getHandle();
	if (hTarget.isValid() && real_handle.isValid()) {
		TMsgGoAndLook msg;
		msg.target = hTarget;
		msg.code_arrived = code_arrived;
		real_handle.sendMsg(msg);
	}
}

void SLBHandle::toggleGuardFormation() {
	getHandleManager<bt_guard>()->onAll(&bt_guard::toggleFormation);
}

void SLBHandle::setActionable(int enabled) {
	if (real_handle.isValid()) {
		CEntity* eTarget = real_handle;
		TMsgSetActivable msg;
		msg.activable = (enabled != 0);
		eTarget->sendMsg(msg);
	}
}

void SLBHandle::activate() {
	if (real_handle.isValid()) {
		CEntity* eTarget = real_handle;
		TMsgActivate msg;
		eTarget->sendMsg(msg);
	}
}

void SLBHandle::setPolarity(int polarity) {
	if (real_handle.isValid()) {
		CEntity* eTarget = real_handle;
		TMsgSetPolarity msg;
		if (polarity < 0) msg.polarity = MINUS;
		else if (polarity > 0) msg.polarity = PLUS;
		else msg.polarity = NEUTRAL;
		eTarget->sendMsg(msg);
	}
}

void SLBHandle::setLocked(int locked) {
	if (real_handle.isValid()) {
		CEntity* eTarget = real_handle;
		TMsgSetLocked msg;
		msg.locked = (locked != 0);
		eTarget->sendMsg(msg);
	}
}

// Handle group By Tag
void SLBHandleGroup::getHandlesByTag(const char * tag) {
	handle_group = tags_manager.getHandlesByTag(string(tag));
	dbg("[LUA] getHandlesByTag: %d\n", handle_group.size());
}

// Awake group
void SLBHandleGroup::awake() {
	TMsgAwake msgAwake;
	for (auto h : handle_group) {
		if (!h.isValid()) continue;
		CEntity * e = h;
		e->sendMsg(msgAwake);
		dbg("Awake to %s\n", e->getName());
	}
	dbg("[LUA] Awake (group): %d\n", handle_group.size());
}

// Remove physics to the group
void SLBHandleGroup::removePhysics() {
	for (auto h : handle_group) {
		CHandle hPhysics = ((CEntity*)h)->get<TCompPhysics>();
		if (hPhysics.isValid()) hPhysics.destroy();
	}
}

// camera control in LUA
void SLBCamera::getCamera() {
	camera_h = tags_manager.getFirstHavingTag("camera_main");
}

void SLBCamera::setDistanceToTarget(float distance) {
	if (!camera_h.isValid()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setDistanceToTarget(distance);
}

void SLBCamera::setSpeed(float speed) {
	if (!camera_h.isValid()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setSpeed(speed);
}

void SLBCamera::setSpeedUnlocked(float speed) {
	if (!camera_h.isValid()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setSpeedUnlocked(speed);
}

void SLBCamera::setRotationSensibility(float sensibility) {
	if (!camera_h.isValid()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setRotationSensibility(sensibility);
}

void SLBCamera::setPositionOffset(float x_offset, float y_offset, float z_offset) {
	if (!camera_h.isValid()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	VEC3 offset(x_offset, y_offset, z_offset);
	camara3rd->setPositionOffset(offset);
}

void SLBCamera::runCinematic(const char* name, float speed) {
	CHandle guidedCam = tags_manager.getHandleByTagAndName("guided_camera", name);
	CEntity * guidedCamE = guidedCam;
	if (guidedCamE) {
		TMsgGuidedCamera msg_guided_cam;
		msg_guided_cam.guide = guidedCam;
		msg_guided_cam.speed = speed;
		guidedCamE->sendMsg(msg_guided_cam);
	}
}

// public generic functions
void SLBPublicFunctions::execCommand(const char* exec_code, float exec_time) {
	// create the new command
	command new_command;
	new_command.code = exec_code;
	new_command.execution_time = exec_time;
	// add the new command to the queue
	logic_manager->getCommandQueue()->push_back(new_command);
}

void SLBPublicFunctions::print(const char* to_print) {
	Debug->LogWithTag("LUA", "%s\n", to_print);
}

void SLBPublicFunctions::setControlEnabled(int enabled) {
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (player.isValid()) {
		CEntity * ePlayer = player;
		TMsgSetControllable msg;
		msg.control = (enabled != 0);
		ePlayer->sendMsg(msg);
	}
}

void SLBPublicFunctions::playSound(const char* sound_route) {
	sound_manager->playSound(std::string(sound_route));
}

void SLBPublicFunctions::playMusic(const char* music_route) {
	sound_manager->playMusic(std::string(music_route));
}

void SLBPublicFunctions::playVoice(const char* voice_route) {
	sound_manager->playVoice(std::string(voice_route));
}

void SLBPublicFunctions::playAmbient(const char* ambient_route) {
	sound_manager->playAmbient(std::string(ambient_route));
}

void SLBPublicFunctions::setSoundVolume(float volume) {
	sound_manager->setVolume(CSoundManagerModule::SFX, volume);
}

void SLBPublicFunctions::setMusicVolume(float volume) {
	sound_manager->setVolume(CSoundManagerModule::MUSIC, volume);
}

void SLBPublicFunctions::setVoicesVolume(float volume) {
	sound_manager->setVolume(CSoundManagerModule::VOICES, volume);
}

void SLBPublicFunctions::setAmbientVolume(float volume) {
	sound_manager->setVolume(CSoundManagerModule::AMBIENT, volume);
}

void SLBPublicFunctions::stopSoundChannel() {
	sound_manager->stopChannel(CSoundManagerModule::SFX);
}

void SLBPublicFunctions::stopMusicChannel() {
	sound_manager->stopChannel(CSoundManagerModule::MUSIC);
}

void SLBPublicFunctions::stopVoicesChannel() {
	sound_manager->stopChannel(CSoundManagerModule::VOICES);
}

void SLBPublicFunctions::stopAmbientChannel() {
	sound_manager->stopChannel(CSoundManagerModule::AMBIENT);
}

void SLBPublicFunctions::toggleIntroState() {
	GameController->TogglePauseIntroState();
}

//test
void SLBPublicFunctions::test(const char* to_print) {
	//CHandle h = CHandle();
	//h.fromUnsigned(h_num);

	//if (h.isValid()) {
	//	CEntity *e = h;
	//	TCompName *name = e->get<TCompName>();
	Debug->LogWithTag("LUA", "%s\n", to_print);
	//}
}