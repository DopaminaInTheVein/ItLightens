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
#include "logic/bt_scientist.h"
#include "logic/pila_container.h"

using namespace IdEntities;

//Position functions
VEC3 SLBPosition::get()
{
	return pos;
}
void SLBPosition::set(VEC3 _pos)
{
	pos = _pos;
}

void SLBPosition::setXYZ(float _x, float _y, float _z)
{
	pos.x = _x;
	pos.y = _y;
	pos.z = _z;
}

float SLBPosition::X()
{
	return pos.x;
}

float SLBPosition::Y()
{
	return pos.y;
}

float SLBPosition::Z()
{
	return pos.z;
}

// player functions
void SLBPlayer::getPlayer() {
	CHandle thePlayer = tags_manager.getFirstHavingTag("player");
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
	CHandle thePlayer = tags_manager.getFirstHavingTag("player");
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

SLBPosition SLBHandle::getPos() {
	SLBPosition p;
	if (real_handle.isValid()) {
		GET_COMP(t, real_handle, TCompTransform);
		if (t) {
			p.set(t->getPosition());
		}
	}
	return p;
}

void SLBHandle::setPos(SLBPosition p) {
	if (real_handle.isValid()) {
		GET_COMP(t, real_handle, TCompTransform);
		if (t) {
			t->setPosition(p.get());
		}
	}
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

void SLBHandle::followTracker(SLBHandle target, float speed) {
	CHandle hTarget = target.getHandle();
	if (hTarget.isValid() && real_handle.isValid()) {
		TMsgFollow msg;
		msg.follower = real_handle;
		msg.speed = speed;
		hTarget.sendMsg(msg);
	}
}

void SLBHandle::toggleGuardFormation() {
	getHandleManager<bt_guard>()->onAll(&bt_guard::toggleFormation);
}

void SLBHandle::toggleScientistBusy() {
	getHandleManager<bt_scientist>()->onAll(&bt_scientist::toggleBusyState);
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

bool SLBHandle::hasPila() {
	if (real_handle.isValid()) {
		GET_COMP(pilaContainer, real_handle, TCompPilaContainer);
		CEntity* e = real_handle;
		dbg("Pregunto por pila a [%s]\n", e->getName());
		return pilaContainer->HasPila();
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
		if (h.isValid()) {
			CHandle hPhysics = ((CEntity*)h)->get<TCompPhysics>();
			if (hPhysics.isValid()) hPhysics.destroy();
		}
	}
}

// camera control in LUA
void SLBCamera::getCamera() {
	camera_h = tags_manager.getFirstHavingTag("camera_main");
}

bool SLBCamera::checkCamera() {
	if (!camera_h.isValid()) {
		getCamera();
		return camera_h.isValid();
	}
	return true;
}

void SLBCamera::setDistanceToTarget(float distance) {
	if (!checkCamera()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setDistanceToTarget(distance);
}

void SLBCamera::setSpeed(float speed) {
	if (!checkCamera()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setSpeed(speed);
}

void SLBCamera::setSpeedUnlocked(float speed) {
	if (!checkCamera()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setSpeedUnlocked(speed);
}

void SLBCamera::setRotationSensibility(float sensibility) {
	if (!checkCamera()) return;
	CEntity * camera_e = camera_h;
	if (!camera_e) return;

	TCompController3rdPerson * camara3rd = camera_e->get<TCompController3rdPerson>();

	camara3rd->setRotationSensibility(sensibility);
}

void SLBCamera::setPositionOffset(float x_offset, float y_offset, float z_offset) {
	if (!checkCamera()) return;
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

void SLBCamera::fadeIn(float speed) {
	if (!checkCamera()) return;
	GET_COMP(fx, camera_h, TCompFadeScreen);
	fx->SetMaxTime(speed);
	fx->FadeIn();
}

void SLBCamera::fadeOut(float speed) {
	if (!checkCamera()) return;
	GET_COMP(fx, camera_h, TCompFadeScreen);
	fx->SetMaxTime(speed);
	fx->FadeOut();
}

// public generic functions
void SLBPublicFunctions::execCommand(const char* exec_code, float exec_time) {
	// create the new command
	command new_command;
	new_command.code = exec_code;
	new_command.execution_time = exec_time;
	// add the new command to the queue
	//logic_manager->getCommandQueue()->push_back(new_command);
	logic_manager->addCommand(new_command);
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

void SLBPublicFunctions::playerTalks(const char* text, const char* iconName, const char* iconText) {
	// DO Something with text...
	dbg(text);

	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;

	auto hm1 = CHandleManager::getByName("name");
	CHandle new_hn = hm1->createHandle();
	MKeyValue atts1;
	atts1.put("name", "playerTalk");
	new_hn.load(atts1);
	entity->add(new_hn);

	auto hm3 = CHandleManager::getByName("helper_message");
	CHandle new_hl = hm3->createHandle();
	MKeyValue atts3;
	atts3["text"] = text;
	atts3["icon"] = iconName;
	atts3["iconText"] = iconText;
	new_hl.load(atts3);
	entity->add(new_hl);
}

void SLBPublicFunctions::playerTalksWithColor(const char* text, const char* iconName, const char* iconText, const char* background, const char* textColor) {
	// DO Something with text...
	dbg(text);

	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;

	auto hm1 = CHandleManager::getByName("name");
	CHandle new_hn = hm1->createHandle();
	MKeyValue atts1;
	atts1.put("name", "playerTalk");
	new_hn.load(atts1);
	entity->add(new_hn);

	auto hm3 = CHandleManager::getByName("helper_message");
	CHandle new_hl = hm3->createHandle();
	MKeyValue atts3;
	atts3["text"] = text;
	atts3["icon"] = iconName;
	atts3["iconText"] = iconText;
	atts3["backgroundColor"] = background;
	atts3["textColor"] = textColor;
	new_hl.load(atts3);
	entity->add(new_hl);
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