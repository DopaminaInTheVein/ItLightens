#include "mcv_platform.h"
#include "slb_public_functions.h"
#include "handle/handle_manager.h"
#include "handle/handle.h"
#include "components/comp_name.h"
#include "components/entity.h"
#include "components/entity_parser.h"
#include "components/comp_charactercontroller.h"
#include "components/comp_life.h"
#include "components/comp_video.h"
#include "components/comp_tasklist.h"
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
void SLBPlayer::getRaijin() {
	CHandle thePlayer = tags_manager.getFirstHavingTag("raijin");
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
		CHandle target = tags_manager.getHandleByTagAndName("teleport", point_name);
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
	getRaijin();
	CEntity* entity = player_handle;

	TCompLife* player_life = entity->get<TCompLife>();

	player_life->currentlife += energy_to_add;
}

void SLBPlayer::refillEnergy() {
	getRaijin();
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
void SLBHandle::getRaijin() {
	CHandle thePlayer = tags_manager.getFirstHavingTag("raijin");
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

bool SLBHandle::isValid() {
	bool valid = real_handle.isValid();
	dbg("SLBHANDLE: Is valid?: %d\n", valid);
	return valid;
}

const char* SLBHandle::getName() {
	if (real_handle.isValid()) return ((CEntity*)real_handle)->getName();
	return "";
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

int SLBHandle::activate() {
	int ret = -1;
	if (real_handle.isValid()) {
		CEntity* eTarget = real_handle;
		elevator * elev = eTarget->get<elevator>();
		if (elev) ret = elev->state;
		TMsgActivate msg;
		eTarget->sendMsg(msg);
	}
	return ret;
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
	float speed = 0.f;
	if (locked < 0) {
		speed = -locked;
	}

	if (real_handle.isValid()) {
		CEntity* eTarget = real_handle;
		TMsgSetLocked msg;
		msg.locked = (locked != 0);
		msg.speed = speed;
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

bool SLBHandle::hasPilaCharged() {
	if (real_handle.isValid()) {
		GET_COMP(pilaContainer, real_handle, TCompPilaContainer);
		CEntity* e = real_handle;
		dbg("Pregunto por pila a [%s]\n", e->getName());
		return pilaContainer->HasPilaCharged();
	}
}

void SLBHandle::setCharged(bool charged) {
	if (real_handle.isValid()) {
		TMsgSetCharged msg;
		msg.charged = charged;
		real_handle.sendMsg(msg);
	}
}

bool SLBHandle::isCharged() {
	if (real_handle.isValid()) {
		TMsgIsCharged msg;
		real_handle.sendMsgWithReply(msg);
		return msg.charged;
	}
	return false;
}
void SLBHandle::_setAnim(const char* name, bool loop) {
	if (real_handle.isValid()) {
		TMsgSetAnim msg;
		msg.loop = loop;
		msg.name = vector<string>(1, string(name));
		real_handle.sendMsg(msg);
	}
}
void SLBHandle::setAnim(const char* name) {
	_setAnim(name, false);
}
void SLBHandle::setAnimLoop(const char* name) {
	_setAnim(name, true);
}
int SLBHandle::addOption(const char* name) {
	int res = -1;
	if (real_handle.isValid()) {
		GET_COMP(gui_selector, real_handle, TCompGuiSelector);
		auto name_fixed = TextEncode::Utf8ToLatin1String(name);
		if (gui_selector) res = gui_selector->AddOption(string(name_fixed));
	}
	return res;
}
void SLBHandle::selectOption(int id) {
	if (real_handle.isValid()) {
		GET_COMP(gui_selector, real_handle, TCompGuiSelector);
		if (gui_selector) gui_selector->SelectOption(id);
	}
}
bool SLBHandle::isPatrolling() {
	bool patrol = false;
	if (real_handle.isValid()) {
		GET_COMP(guard, real_handle, bt_guard);
		if (guard) patrol = guard->isPatrolling();
	}
	dbg("SLBHANDLE::isPatroling: %d", patrol);
	return patrol;
}
bool SLBHandle::isComeBack() {
	bool comeback = false;
	if (real_handle.isValid()) {
		GET_COMP(guard, real_handle, bt_guard);
		if (guard) comeback = guard->isInFirstSeekPoint();
	}
	dbg("SLBHANDLE::isPatroling: %d", comeback);
	return comeback;
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

// Remove physics to the group
void SLBHandleGroup::destroy() {
	for (auto h : handle_group) {
		if (h.isValid()) {
			h.destroy();
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

void SLBCamera::orbit(bool new_orbit) {
	if (!checkCamera()) return;
	GET_COMP(cam_control, camera_h, TCompController3rdPerson);
	if (cam_control) {
		if (new_orbit) cam_control->StartOrbit();
		else cam_control->StopOrbit();
	}
}

void SLBCamera::resetCamera() {
	// restore the normal 3rd person camera
	if (!checkCamera()) return;
	// turn manual control off
	CEntity * camera_e = camera_h;
	GET_COMP(cam_m, camera_e, TCompCameraMain);
	cam_m->setManualControl(false);
	GameController->SetManualCameraState(false);
	// restore normal controls
	TMsgSetControllable msg;
	msg.control = true;
	camera_e->sendMsg(msg);
	GET_COMP(cam_control, camera_e, TCompController3rdPerson);
	if (cam_control) cam_control->StopOrbit();
}

//Ui Camera control in LUA
void SLBUiCamera::getCamera() {
	ui_camera_h = tags_manager.getFirstHavingTag("ui_camera");
}

bool SLBUiCamera::checkCamera() {
	if (!ui_camera_h.isValid()) {
		getCamera();
		return ui_camera_h.isValid();
	}
	return true;
}

void SLBUiCamera::fadeIn(float speed) {
	if (!checkCamera()) return;
	GET_COMP(fx, ui_camera_h, TCompFadeScreen);
	fx->SetMaxTime(speed);
	fx->FadeIn();
}

void SLBUiCamera::fadeOut(float speed) {
	if (!checkCamera()) return;
	GET_COMP(fx, ui_camera_h, TCompFadeScreen);
	fx->SetMaxTime(speed);
	fx->FadeOut();
}

// Data
SLBData::SLBData()
{
	file_name = "data/data.json";
	data = readIniAtrData(file_name, CApp::get().getCurrentRealLevel());
}
float SLBData::getFloat(const char* key)
{
	return data[std::string(key)];
}

bool SLBData::getBool(const char* key)
{
	return data[std::string(key)] != 0.f;
}
void SLBData::putFloat(const char* key, float value)
{
	data[std::string(key)] = value;
}
void SLBData::putBool(const char* key, bool value)
{
	data[std::string(key)] = value ? 1.f : 0.f;
}
void SLBData::write()
{
	writeIniAtrData(file_name, CApp::get().getCurrentRealLevel(), data);
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

void SLBPublicFunctions::setupGame() {
	GameController->Setup();
}

void SLBPublicFunctions::setLanguage(const char* lang) {
	GameController->SetLanguage(std::string(lang));
}

void SLBPublicFunctions::completeTasklist(int i) {
	CHandle tasklist = tags_manager.getFirstHavingTag(getID("tasklist"));
	CEntity * tasklist_e = tasklist;
	Tasklist * tasklist_comp = tasklist_e->get<Tasklist>();
	tasklist_comp->completeTask(i);
}

void SLBPublicFunctions::setControlEnabled(int enabled) {
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	CHandle main_camera = tags_manager.getFirstHavingTag(getID("camera_main"));
	if (player.isValid() && main_camera.isValid()) {
		TMsgSetControllable msg;
		msg.control = (enabled != 0);
		player.sendMsg(msg);
		main_camera.sendMsg(msg);
	}
}

void SLBPublicFunctions::setPlayerEnabled(int enabled) {
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (player.isValid()) {
		TMsgSetControllable msg;
		msg.control = (enabled != 0);
		player.sendMsg(msg);
	}
}

void SLBPublicFunctions::setCameraEnabled(int enabled) {
	CHandle main_camera = tags_manager.getFirstHavingTag(getID("camera_main"));
	if (main_camera.isValid()) {
		TMsgSetControllable msg;
		msg.control = (enabled != 0);
		main_camera.sendMsg(msg);
	}
}
void SLBPublicFunctions::setOnlySense(int enabled) {
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (player.isValid()) {
		TMsgSetOnlySense msg;
		msg.sense = (enabled != 0);
		player.sendMsg(msg);
	}
}

void SLBPublicFunctions::playSound(const char* sound_route, float volume = 1.f) {
	sound_manager->playSound(std::string(sound_route), volume);
}

void SLBPublicFunctions::play3dSound(const char* sound_route, float pl_x, float pl_y, float pl_z, float s_x, float s_y, float s_z, float volume = 1.f) {
	VEC3 player_pos = VEC3(pl_x, pl_y, pl_z);
	player_pos.Normalize();

	VEC3 sound_pos = VEC3(s_x, s_y, s_z);
	sound_pos.Normalize();

	sound_manager->play3dSound(std::string(sound_route), player_pos, sound_pos, volume);
}

void SLBPublicFunctions::stopSound(const char* sound_route) {
	sound_manager->stopSound(std::string(sound_route));
}

void SLBPublicFunctions::playMusic(const char* music_route) {
	sound_manager->playMusic(std::string(music_route));
}

void SLBPublicFunctions::playLoopingMusic(const char* music_route) {
	sound_manager->playLoopingMusic(std::string(music_route));
}

void SLBPublicFunctions::stopMusic() {
	sound_manager->stopMusic();
}

void SLBPublicFunctions::playVoice(const char* voice_route) {
	sound_manager->playVoice(std::string(voice_route));
}

void SLBPublicFunctions::playAmbient(const char* ambient_route) {
	sound_manager->playAmbient(std::string(ambient_route));
}

void SLBPublicFunctions::setMusicVolume(float volume) {
	sound_manager->setMusicVolume(volume);
}

void SLBPublicFunctions::playVideo(const char* video_route) {
	CHandle h = createPrefab("video_player");
	GET_COMP(video_player, h, TCompVideo);
	video_player->setup(video_route);
	video_player->init();
}

void SLBPublicFunctions::playVideoAndDo(const char* video_route, const char* lua_code) {
	CHandle h = createPrefab("video_player");
	GET_COMP(video_player, h, TCompVideo);
	video_player->setup(video_route, lua_code);
	video_player->init();
}

void SLBPublicFunctions::playerRoom(int newRoom) {
	std::vector<int> new_room_vec;
	new_room_vec.push_back(newRoom);

	CHandle p = tags_manager.getFirstHavingTag("player");
	CEntity * pe = p;
	TCompRoom * room = pe->get<TCompRoom>();
	if (room->setName(new_room_vec)) {
		SBB::postSala(newRoom);
	}
}

void SLBPublicFunctions::playerTalks(const char* text) {
	// DO Something with text...
	dbg(text);
	for (auto handles : tags_manager.getHandlesByTag("talk_text")) handles.destroy();
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
	new_hl.load(atts3);
	entity->add(new_hl);

	//Add tag talk text
	TMsgSetTag msg;
	msg.add = true;
	msg.tag = "talk_text";
	new_hp.sendMsg(msg);
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

void SLBPublicFunctions::putText(const char* id, const char* text, float posx, float posy, const char* textColor, float scale, const char* textColorTarget, float textColorSpeed, float textColorSpeedLag) {
	auto text_fixed = TextEncode::Utf8ToLatin1String(text);
	CHandle h = createPrefab("ui/text");
	GET_COMP(t, h, TCompText);
	t->setup(std::string(id), text_fixed, posx, posy, std::string(textColor), scale, std::string(textColorTarget), textColorSpeed, textColorSpeedLag);
}

void SLBPublicFunctions::removeText(const char* id) {
	std::string id_string(id);
	getHandleManager<TCompText>()->each([id_string](TCompText * mess) {
		if (mess->getId() == id_string) { mess->forceTTLZero(); }
	}
	);
}
//void SLBPublicFunctions::alterText(const char* id, float new_posx, float new_posy, float new_scale) {
//	std::string id_string(id);
//	getHandleManager<TCompText>()->each([id_string, new_posx, new_posy, new_scale](TCompText * mess) {
//		if (mess->getId() == id_string) { mess->setAttr(new_posx, new_posy, new_scale); }
//	}
//	);
//}

void SLBPublicFunctions::characterGlobe(float distance, float char_x, float char_y, float char_z) {
	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;

	auto hm1 = CHandleManager::getByName("name");
	CHandle new_hn = hm1->createHandle();
	MKeyValue atts1;
	atts1.put("name", "characterGlobe");
	new_hn.load(atts1);
	entity->add(new_hn);

	auto hm3 = CHandleManager::getByName("character_globe");
	CHandle new_hl = hm3->createHandle();

	// generation of a unique name
	std::string name = "globe_" + to_string(char_x) + "_" + to_string(char_z);

	// Creation of the attributes
	MKeyValue atts3;
	atts3["name"] = name;
	atts3["dist"] = std::to_string(distance);
	atts3["posx"] = std::to_string(char_x);
	atts3["posy"] = std::to_string(char_y);
	atts3["posz"] = std::to_string(char_z);

	new_hl.load(atts3);
	entity->add(new_hl);
}

void SLBPublicFunctions::toggleIntroState() {
	GameController->TogglePauseIntroState();
}

void SLBPublicFunctions::launchVictoryState() {
	GameController->SetGameState(CGameController::VICTORY);
}

void SLBPublicFunctions::showLoadingScreen() {
	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;

	auto hm3 = CHandleManager::getByName("loading_screen");
	CHandle new_hl = hm3->createHandle();

	// Creation of the attributes
	MKeyValue atts3;
	atts3["name"] = "loading_screen";

	new_hl.load(atts3);
	entity->add(new_hl);
}

void SLBPublicFunctions::loadLevel(const char* level_name) {
	CApp::get().changeScene(level_name);
}

void SLBPublicFunctions::saveLevel() {
	CApp::get().saveLevel();
}

void SLBPublicFunctions::clearSaveData() {
	CApp::get().clearSaveData();
}

void SLBPublicFunctions::loadEntities(const char* file_name) {
	CApp::get().loadEntities(file_name);
}

void SLBPublicFunctions::resume() {
	controller->ChangeMouseState(true);
	GameController->SetGameState(CGameController::RUNNING);
}

void SLBPublicFunctions::jsonEdit(std::string filename, std::string group, std::string name, float new_value) {
	std::map<std::string, float> all_values = readIniAtrData(filename, group);
	all_values[name] = new_value;
	writeIniAtrData(filename, group, all_values);
}

float SLBPublicFunctions::jsonRead(std::string filename, std::string group, std::string name) {
	std::map<std::string, float> all_values = readIniAtrData(filename, group);
	return all_values[name];
}

void SLBPublicFunctions::jsonEditStr(std::string filename, std::string group, std::string name, string new_value) {
	std::map<std::string, string> all_values = readIniAtrDataStr(filename, group);
	all_values[name] = new_value;
	writeIniAtrDataStr(filename, group, all_values);
}

string SLBPublicFunctions::jsonReadStr(std::string filename, std::string group, std::string name) {
	std::map<std::string, string> all_values = readIniAtrDataStr(filename, group);
	return all_values[name];
}

void SLBPublicFunctions::exit() {
	CApp::get().exitGame();
}

const char* SLBPublicFunctions::getText(const char* scene, const char* event) {
	std::string res_str = lang_manager->getText(scene, event);

	char * res = new char[res_str.size() + 1];
	std::copy(res_str.begin(), res_str.end(), res);
	res[res_str.size()] = '\0';

	return res;
}

void SLBPublicFunctions::reloadLanguageFile(const char* language) {
	lang_manager->reloadLanguageFile(language);
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