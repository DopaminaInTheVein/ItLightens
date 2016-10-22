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

#include "app_modules/render/module_render_postprocess.h"
#include "app_modules/entities.h"
#include "render/fx/fx_fade_screen.h"

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
	CHandle thePlayer = CPlayerBase::handle_player;
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

void SLBPlayer::unPossess() {
	getPlayer();
	if (player_handle.isValid())
		player_handle.sendMsg(TMsgUnpossesDamage());
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
	CHandle thePlayer = CPlayerBase::handle_player;
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

void SLBHandle::setSize(float size) {
	if (real_handle.isValid()) {
		GET_COMP(globe, real_handle, TCompFadingGlobe);
		if (globe) {
			globe->setWorldSize(size);
		}
		else {
			GET_COMP(t, real_handle, TCompTransform);
			if (t) {
				t->setScaleBase(size);
			}
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
	// if it has entity_controller, we get the coord from there
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();
	if (entity_controller) {
		return entity_controller->GetPosition().x;
	}
	// otherwise, we get it from the transform
	else {
		TCompTransform* entity_transform = entity->get<TCompTransform>();
		return entity_transform->getPosition().x;
	}
}

float SLBHandle::getY() {
	CEntity* entity = real_handle;
	// if it has entity_controller, we get the coord from there
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();
	if (entity_controller) {
		return entity_controller->GetPosition().y;
	}
	// otherwise, we get it from the transform
	else {
		TCompTransform* entity_transform = entity->get<TCompTransform>();
		return entity_transform->getPosition().y;
	}
}

float SLBHandle::getZ() {
	CEntity* entity = real_handle;
	// if it has entity_controller, we get the coord from there
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();
	if (entity_controller) {
		return entity_controller->GetPosition().z;
	}
	// otherwise, we get it from the transform
	else {
		TCompTransform* entity_transform = entity->get<TCompTransform>();
		return entity_transform->getPosition().z;
	}
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
		if (gui_selector) res = gui_selector->AddOption(name_fixed);
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
void SLBHandle::setGuiEnabled(bool enabled)
{
	if (real_handle.isValid()) {
		GET_COMP(gui, real_handle, TCompGui);
		if (gui) gui->SetEnabled(enabled);
	}
}
void SLBHandle::setDragValue(float value)
{
	if (real_handle.isValid()) {
		GET_COMP(gui_drag, real_handle, TCompGuiDrag);
		if (gui_drag) gui_drag->SetValue(value);
	}
}
//Handle Particles
void SLBHandle::particlesOn()
{
	if (real_handle.isValid()) {
		GET_COMP(part, real_handle, CParticleSystem); //me
		if (!part) part = GETH_COMP(real_handle.getOwner(), CParticleSystem); //or my owner
		if (part) part->ActiveParticleSystem();
	}
}
void SLBHandle::particlesOff()
{
	if (real_handle.isValid()) {
		GET_COMP(part, real_handle, CParticleSystem); //me
		if (!part) part = GETH_COMP(real_handle.getOwner(), CParticleSystem); //or my owner
		if (part) part->setLoop(false);
	}
}
void SLBHandle::particlesLoop()
{
	if (real_handle.isValid()) {
		GET_COMP(part, real_handle, CParticleSystem); //me
		if (!part) part = GETH_COMP(real_handle.getOwner(), CParticleSystem); //or my owner
		if (part) part->setLoop(true);
	}
}
void SLBHandle::particlesLoad(const char* name, int enabled)
{
	if (real_handle.isValid()) {
		CEntity* e = real_handle;
		if (e) {
			auto hm_part = CHandleManager::getByName("particle_system");
			CHandle new_hpart = hm_part->createHandle();
			CParticleSystem* part = new_hpart;
			if (part) {
				part->loadFromFile(std::string(name));
				e->add(new_hpart);
				if (enabled == 1) part->ActiveParticleSystem();
				else if (enabled > 1) part->setLoop(true);
			}
		}
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

void SLBCamera::execCinematic(const char* name, float speed, bool is_start) {
	CHandle guidedCam = tags_manager.getHandleByTagAndName("guided_camera", name);
	if (guidedCam.isValid()) {
		TMsgGuidedCamera msg_guided_cam;
		msg_guided_cam.guide = guidedCam;
		msg_guided_cam.speed = speed;
		msg_guided_cam.start = is_start;
		guidedCam.sendMsg(msg_guided_cam);
	}
}

void SLBCamera::runCinematic(const char* name, float speed) {
	execCinematic(name, speed, false);
}

void SLBCamera::startCinematic(const char* name, float speed) {
	execCinematic(name, speed, true);
}

void SLBCamera::skipCinematic() {
	if (!checkCamera()) return;
	GET_COMP(cam, camera_h, TCompCameraMain);
	if (cam) cam->skipCinematic();
}

void SLBCamera::fadeIn(float speed) {
	//TCompFadeScreen * fx = render_fx->GetFX<TCompFadeScreen>(FX_FADESCREEN_ALL);
	GET_FX(fx, TFadeScreen, FX_FADESCREEN);
	if (!render_fx->isActive(FX_FADESCREEN)) {
		fx->Activate();
	}
	fx->SetMaxTime(speed);
	fx->FadeIn();
}

void SLBCamera::fadeOut(float speed) {
	GET_FX(fx, TFadeScreen, FX_FADESCREEN);
	if (!render_fx->isActive(FX_FADESCREEN)) {
		fx->Activate();
	}
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
	GET_COMP(cam_m, camera_h, TCompCameraMain);
	cam_m->reset();
}

void SLBCamera::fx(const char* name, int enabled) {
	if (enabled > 0) {
		if (!render_fx->isActive(name)) {
			render_fx->ActivateFXBeforeUI(name);
		}
	}
	else {
		if (render_fx->isActive(name)) {
			render_fx->RemoveActiveFX(name);
		}
	}
}

void SLBCamera::startVibration(float x_max, float y_max, float speed) {
	if (!checkCamera()) return;
	GET_COMP(cam_control, camera_h, TCompController3rdPerson);
	if (cam_control) {
		cam_control->startVibration(x_max, y_max, speed);
	}
}

void SLBCamera::stopVibration(float speed) {
	if (!checkCamera()) return;
	GET_COMP(cam_control, camera_h, TCompController3rdPerson);
	if (cam_control) {
		cam_control->stopVibration(speed);
	}
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
	//TCompFadeScreen * fx = render_fx->GetFX<TCompFadeScreen>(FX_FADESCREEN_ALL);
	GET_FX(fx, TFadeScreenAll, FX_FADESCREEN_ALL);
	if (!render_fx->isActive(FX_FADESCREEN_ALL)) {
		fx->Activate();
	}
	fx->SetMaxTime(speed);
	fx->FadeIn();
}

void SLBUiCamera::fadeOut(float speed) {
	GET_FX(fx, TFadeScreenAll, FX_FADESCREEN_ALL);
	if (!render_fx->isActive(FX_FADESCREEN_ALL)) {
		fx->Activate();
	}
	fx->SetMaxTime(speed);
	fx->FadeOut();
}

void SLBUiCamera::fx(const char* name, bool enabled) {
	if (enabled) {
		if (!render_fx->isActive(name)) {
			render_fx->ActivateFXAtEnd(name);
		}
	}
	else {
		if (render_fx->isActive(name)) {
			render_fx->RemoveActiveFX(name);
		}
	}
}

// Data
SLBData::SLBData()
{
	file_name = DATA_JSON;
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
	new_command.only_runtime = GameController->GetGameState() == CGameController::RUNNING;
	// add the new command to the queue
	//logic_manager->getCommandQueue()->push_back(new_command);
	logic_manager->addCommand(new_command);
}
void SLBPublicFunctions::waitButton(const char* exec_code) {
	// create the new command
	command new_command;
	new_command.code = exec_code;
	new_command.only_runtime = GameController->GetGameState() == CGameController::RUNNING;
	// add the new command to the queue
	//logic_manager->getCommandQueue()->push_back(new_command);
	logic_manager->setWait(new_command);
}
void SLBPublicFunctions::print(const char* to_print) {
	Debug->LogWithTag("LUA", "%s\n", to_print);
}

void SLBPublicFunctions::setupGame() {
	GameController->Setup();
}

void SLBPublicFunctions::setLanguage(const char* lang) {
	lang_manager->SetLanguage(std::string(lang));
}

void SLBPublicFunctions::completeTasklist(int i) {
	VHandles tasklists = tags_manager.getHandlesByTag(getID("tasklist"));
	for (CHandle tasklist : tasklists) {
		CEntity * tasklist_e = tasklist;
		if (!tasklist_e)continue;
		Tasklist * tasklist_comp = tasklist_e->get<Tasklist>();
		if (!tasklist_comp)continue;
		tasklist_comp->completeTask(i);
	}
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

void SLBPublicFunctions::playSound(const char* sound_route, float volume = 1.f, bool looping = false) {
	sound_manager->playSound(std::string(sound_route), volume, looping);
}

void SLBPublicFunctions::play3dSound(const char* sound_route, float s_x, float s_y, float s_z, float max_volume, bool looping = false, int max_instances = 1) {
	VEC3 sound_pos = VEC3(s_x, s_y, s_z);
	// reproduce the sound
	sound_manager->play3dSound(std::string(sound_route), sound_pos, max_volume, looping, max_instances);
}

void SLBPublicFunctions::playFixed3dSound(const char* sound_route, const char* sound_name, float s_x, float s_y, float s_z, float max_volume, bool looping = false) {
	VEC3 sound_pos = VEC3(s_x, s_y, s_z);
	// reproduce the sound
	sound_manager->playFixed3dSound(std::string(sound_route), std::string(sound_name), sound_pos, max_volume, looping);
}

void SLBPublicFunctions::stopSound(const char* sound_route) {
	sound_manager->stopSound(std::string(sound_route));
}

void SLBPublicFunctions::stopFixedSound(const char* sound_name) {
	sound_manager->stopFixedSound(std::string(sound_name));
}

void SLBPublicFunctions::stopAllSounds() {
	sound_manager->stopAllSounds();
}

void SLBPublicFunctions::playMusic(const char* music_route, float volume) {
	sound_manager->playMusic(std::string(music_route), volume);
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

void SLBPublicFunctions::setSFXVolume(float volume) {
	sound_manager->setSFXVolume(volume);
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
	GET_COMP(room, CPlayerBase::handle_player, TCompRoom);
	if (!room) return;
	room->setRoom(TRoom(newRoom));
	SBB::postSala(newRoom);
}

void SLBPublicFunctions::playerTalks(const char* text) {
	// DO Something with text...
	dbg(text);
	auto text_fixed = TextEncode::Utf8ToLatin1String(text);
	getHandleManager<TCompFadingMessage>()->each([text_fixed](TCompFadingMessage * mess) {
		TCompFadingMessage::ReloadInfo atts;
		atts.text = text_fixed;
		mess->reload(atts);
	}
	);
}

void SLBPublicFunctions::showMessage(const char* text, const char* icon) {
	auto text_fixed = TextEncode::Utf8ToLatin1String(text);
	getHandleManager<TCompFadingMessage>()->each([text_fixed, icon](TCompFadingMessage * mess) {
		TCompFadingMessage::ReloadInfo atts;
		atts.permanent = true;
		atts.text = text_fixed;
		atts.icon = icon;
		mess->reload(atts);
	}
	);
}
void SLBPublicFunctions::hideMessage() {
	getHandleManager<TCompFadingMessage>()->each([](TCompFadingMessage * mess) {
		mess->kill();
	}
	);
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

void SLBPublicFunctions::addAimCircle(const char* id, const char* prefab, float char_x, float char_y, float char_z, float ttl) {
	std::string name = std::string(id);

	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;

	auto hm1 = CHandleManager::getByName("name");
	CHandle new_hn = hm1->createHandle();
	MKeyValue atts1;
	atts1.put("name", name);
	new_hn.load(atts1);
	entity->add(new_hn);

	auto hm3 = CHandleManager::getByName("character_globe");
	CHandle new_hl = hm3->createHandle();

	// Creation of the attributes
	MKeyValue atts3;
	atts3["name"] = name;
	atts3["route"] = std::string(prefab);
	atts3["ttl"] = std::to_string(ttl);
	atts3["posx"] = std::to_string(char_x);
	atts3["posy"] = std::to_string(char_y);
	atts3["posz"] = std::to_string(char_z);
	atts3["screen_z"] = "0.2";

	new_hl.load(atts3);
	entity->add(new_hl);
}
void SLBPublicFunctions::removeAimCircle(const char* id) {
	std::string id_string(id);
	getHandleManager<TCompFadingGlobe>()->each([id_string](TCompFadingGlobe * mess) {
		if (mess->getGlobeName() == id_string) { mess->forceTTLZero(); }
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

SLBHandle SLBPublicFunctions::characterGlobe(const char* route, float distance, float char_x, float char_y, float char_z, float ttl, float max_distance) {
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
	atts3["route"] = std::string(route);
	atts3["dist"] = std::to_string(distance);
	atts3["posx"] = std::to_string(char_x);
	atts3["posy"] = std::to_string(char_y);
	atts3["posz"] = std::to_string(char_z);
	atts3["ttl"] = std::to_string(ttl);
	atts3["max_distance"] = std::to_string(max_distance);

	new_hl.load(atts3);
	entity->add(new_hl);
	return SLBHandle(CHandle(entity), entity->getName());
}

void SLBPublicFunctions::toggleIntroState() {
	GameController->TogglePauseIntroState();
}

void SLBPublicFunctions::launchVictoryState() {
	GameController->SetGameState(CGameController::VICTORY);
}

//AI control
void SLBPublicFunctions::AIStart() {
	GameController->setAiUpdate(true);
}

void SLBPublicFunctions::AIStop() {
	GameController->setAiUpdate(false);
}

//void SLBPublicFunctions::showLoadingScreen() {
//	auto hm = CHandleManager::getByName("entity");
//	CHandle new_hp = hm->createHandle();
//	CEntity* entity = new_hp;
//
//	auto hm3 = CHandleManager::getByName("loading_screen");
//	CHandle new_hl = hm3->createHandle();
//
//	// Creation of the attributes
//	MKeyValue atts3;
//	atts3["name"] = "loading_screen";
//
//	new_hl.load(atts3);
//	entity->add(new_hl);
//}

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

void SLBPublicFunctions::pauseGame() {
	GameController->SetGameState(CGameController::STOPPED);
}

void SLBPublicFunctions::setCursorEnabled(bool enabled) {
	Gui->setCursorEnabled(enabled);
}
void SLBPublicFunctions::resumeGame() {
	GameController->SetGameState(CGameController::RUNNING);
}
const char* SLBPublicFunctions::getText(const char* scene, const char* event) {
	std::string res_str = lang_manager->getText(event, scene);

	char * res = new char[res_str.size() + 1];
	std::copy(res_str.begin(), res_str.end(), res);
	res[res_str.size()] = '\0';

	return res;
}

void SLBPublicFunctions::forceSenseVision() {
	TCompSenseVision * sense = GameController->getSenseVisionComp();
	if (sense) {
		sense->setSenseVisionMode(TCompSenseVision::eSenseVision::ENABLED);
	}
}
void SLBPublicFunctions::unforceSenseVision() {
	TCompSenseVision * sense = GameController->getSenseVisionComp();
	if (sense) {
		sense->setSenseVisionMode(TCompSenseVision::eSenseVision::DEFAULT);
	}
}

SLBHandle SLBPublicFunctions::create(const char* name, float x, float y, float z)
{
	CHandle h = createPrefab(name);
	GET_COMP(tmx, h, TCompTransform);
	if (tmx) tmx->setPosition(VEC3(x, y, z));
	std::string e_name = "unnamed";
	CEntity* e = h;
	if (e) e_name = e->getName();
	GET_COMP(part, h, CParticleSystem);
	if (part) part->init();
	return SLBHandle(h, e_name);
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