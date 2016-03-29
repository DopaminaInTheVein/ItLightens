#include "mcv_platform.h"
#include "player_controller_cientifico.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"
#include "prefabs\magnetic_bomb.h"
#include "prefabs\static_bomb.h"
#include "components\comp_name.h"
#include "ui\ui_interface.h"
#include "components\comp_msgs.h"
#include "app_modules\io\io.h"
#include "render\static_mesh.h"
#include "components\comp_render_static_mesh.h"
#include "components/comp_charactercontroller.h"

map<string, statehandler> player_controller_cientifico::statemap = {};
map<int, string> player_controller_cientifico::out = {};

void player_controller_cientifico::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_cientifico")) {

			map<std::string, float> fields_base = readIniFileAttrMap("controller_base");

			assignValueToVar(player_max_speed, fields_base);
			assignValueToVar(player_rotation_speed, fields_base);
			assignValueToVar(jimpulse, fields_base);
			assignValueToVar(left_stick_sensibility, fields_base);
			assignValueToVar(camera_max_height, fields_base);
			assignValueToVar(camera_min_height, fields_base);

			map<std::string, float> fields_scientist = readIniFileAttrMap("controller_scientist");

			assignValueToVar(t_waiting, fields_scientist);
			assignValueToVar(t_to_explode, fields_scientist);
			assignValueToVar(t_create_beacon, fields_scientist);
			assignValueToVar(t_create_StaticBomb, fields_scientist);
			assignValueToVar(t_create_MagneticBomb, fields_scientist);
			assignValueToVar(t_explode_static_bomb, fields_scientist);
			assignValueToVar(t_create_beacon_energy, fields_scientist);
			assignValueToVar(t_create_StaticBomb_energy, fields_scientist);
			assignValueToVar(t_create_MagneticBomb_energy, fields_scientist);

		}
	}
}

void player_controller_cientifico::Init() {

	// read main attributes from file
	readIniFileAttr();

	om = getHandleManager<player_controller_cientifico>();	//player

	if (statemap.empty()) {
		//States from controller base and poss controller
		addBasicStates();
		addPossStates();

		//Specific Scientist nodes
		AddState("createMagneticBomb", (statehandler)&player_controller_cientifico::CreateMagneticBomb);
		AddState("createDisableBeacon", (statehandler)&player_controller_cientifico::CreateDisableBeacon);
		AddState("createStaticBomb", (statehandler)&player_controller_cientifico::CreateStaticBomb);
		AddState("addDisableBeacon", (statehandler)&player_controller_cientifico::AddDisableBeacon);
		AddState("useMagneticBomb", (statehandler)&player_controller_cientifico::UseMagneticBomb);
		AddState("useStaticBomb", (statehandler)&player_controller_cientifico::UseStaticBomb);
	}

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
}

//##########################################################################

//##########################################################################
//Inputs
//##########################################################################
#pragma region Inputs

void player_controller_cientifico::UpdateInputActions() {
	PROFILE_FUNCTION("player cientifico: energy dec");
	energyDecreasal(getDeltaTime()*0.5f);
	if (io->keys['1'].becomesPressed() || io->joystick.button_X.becomesPressed()) {
		ChangeState("createDisableBeacon");
	}

	if (io->keys['2'].becomesPressed() || io->joystick.button_B.becomesPressed()) {
		ChangeState("addDisableBeacon");
	}

	if (io->keys['3'].becomesPressed() || io->joystick.button_Y.becomesPressed()) {
		energyDecreasal(5.0f);
		ChangeState("createStaticBomb");
	}

	if (io->keys['5'].becomesPressed() || io->joystick.button_L.becomesPressed()) {
		energyDecreasal(5.0f);
		ChangeState("createMagneticBomb");
	}
	if (io->keys['6'].becomesPressed() || io->joystick.button_LT > io->joystick.max_trigger_value / 2) {
		energyDecreasal(10.0f);
		ChangeState("useMagneticBomb");
	}
	if (io->keys['4'].becomesPressed() || io->joystick.button_RT > io->joystick.max_trigger_value / 2) {
		energyDecreasal(10.0f);
		ChangeState("useStaticBomb");
	}
	if (io->keys['R'].becomesPressed() || io->joystick.button_R.becomesPressed())
		ExplodeBomb();
}

void player_controller_cientifico::ExplodeBomb()
{
	PROFILE_FUNCTION("player cientifico: explode bomb");
	if (obj == STATIC_BOMB_GAME && bomb_handle.isValid()) {
		CStaticBomb *bomb = bomb_handle;
		obj = EMPTY;
		bomb->toExplode();
	}
	
	if (obj == MAGNETIC_BOMB_GAME && bomb_handle.isValid()) {
		CMagneticBomb *bomb = bomb_handle;
		obj = EMPTY;
		bomb->toExplode();
	}
}

#pragma endregion

//##########################################################################

//##########################################################################
// Player States
//##########################################################################
#pragma region Player States

void player_controller_cientifico::CreateMagneticBomb()
{
	PROFILE_FUNCTION("player cientifico: vreate mag bomb");
	t_waiting += getDeltaTime();
	if (t_waiting >= t_create_MagneticBomb) {
		dbg("magnetic bomb created!\n");
		t_waiting = 0;
		obj = MAGNETIC_BOMB;
		ChangeState("idle");
	}
}

void player_controller_cientifico::CreateDisableBeacon()
{
	PROFILE_FUNCTION("player cientifico: create disable beac");
	//TODO: input cancel
	t_waiting += getDeltaTime();
	if (t_waiting >= t_create_beacon) {
		t_waiting = 0;
		dbg("disable beacon created!\n");
		obj = DISABLE_BEACON;
		ChangeState("idle");
	}
}

void player_controller_cientifico::CreateStaticBomb()
{
	PROFILE_FUNCTION("player cientifico: create static");
	t_waiting += getDeltaTime();
	if (t_waiting >= t_create_StaticBomb) {
		dbg("static bomb created!\n");
		t_waiting = 0;
		obj = STATIC_BOMB;
		ChangeState("idle");
	}
}

void player_controller_cientifico::AddDisableBeacon()
{
	PROFILE_FUNCTION("player cientifico: add disable beac");
	if (obj == DISABLE_BEACON) {
		VHandles es = tags_manager.getHandlesByTag(getID("beacon"));
		TMsgBeaconBusy msg;
		bool resp = false;
		msg.reply = &resp;
		CEntity *p_e = myParent;
		TCompTransform *mtx = p_e->get<TCompTransform>();
		msg.pos = mtx->getPosition();		//TODO: pos player
		for (CEntity *e : es) {
			e->sendMsgWithReply(msg);
			if (*msg.reply) {
				//dbg("go idle\n");
				obj = EMPTY;
				ChangeState("idle");
				break;
			}
		}
	}

	ChangeState("idle");
}

void player_controller_cientifico::UseMagneticBomb()
{
	PROFILE_FUNCTION("player cientifico: use mag bomb");
	if (obj == MAGNETIC_BOMB) createMagneticBombEntity();
	obj = MAGNETIC_BOMB_GAME;
	ChangeState("idle");
}

void player_controller_cientifico::UseStaticBomb()
{
	PROFILE_FUNCTION("player cientifico: use static bomb");
	if (obj == STATIC_BOMB) createStaticBombEntity();
	obj = STATIC_BOMB_GAME;
	ChangeState("idle");
}

#pragma endregion

//##########################################################################

//##########################################################################
//Create Game Objects
//##########################################################################

#pragma region Game objects creators

void player_controller_cientifico::createMagneticBombEntity()
{
	PROFILE_FUNCTION("player cientifico: create mag bomb");
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	auto hm_e = CHandleManager::getByName("entity");
	CHandle new_h_e = hm_e->createHandle();

	auto hm_n = CHandleManager::getByName("name");
	CHandle new_h_n = hm_n->createHandle();

	auto hm_t = CHandleManager::getByName("transform");
	CHandle new_h_t = hm_t->createHandle();

	auto hm_mb = CHandleManager::getByName("magnetic_bomb");
	CHandle new_h_mb = hm_mb->createHandle();

	auto hm_sm = CHandleManager::getByName("render_static_mesh");
	CHandle new_h_sm = hm_sm->createHandle();

	CEntity *e = new_h_e;
	bomb_handle = new_h_mb;

	e->add(new_h_n);
	e->add(new_h_mb);
	e->add(new_h_t);
	e->add(new_h_sm);

	TCompRenderStaticMesh *sm = new_h_sm;
	sm->static_mesh = Resources.get("static_meshes/bomb.static_mesh")->as<CStaticMesh>();
	sm->registerToRender();

	TCompTransform *mag_trans = e->get<TCompTransform>();
	mag_trans->setPosition(player_position);

	TCompName *name_c = e->get<TCompName>();
	name_c->setName("magnetic_bomb");

	float yaw, pitch;
	player_transform->getAngles(&yaw, &pitch);
	mag_trans->setAngles(yaw, pitch);

	CMagneticBomb *mag_bomb = e->get<CMagneticBomb>();
	mag_bomb->Init();
}

void player_controller_cientifico::createStaticBombEntity()
{
	PROFILE_FUNCTION("player cientifico: create static bomb entity");
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	auto hm_e = CHandleManager::getByName("entity");
	CHandle new_h_e = hm_e->createHandle();

	auto hm_n = CHandleManager::getByName("name");
	CHandle new_h_n = hm_n->createHandle();

	auto hm_t = CHandleManager::getByName("transform");
	CHandle new_h_t = hm_t->createHandle();

	auto hm_mb = CHandleManager::getByName("static_bomb");
	CHandle new_h_mb = hm_mb->createHandle();

	auto hm_sm = CHandleManager::getByName("render_static_mesh");
	CHandle new_h_sm = hm_sm->createHandle();

	CEntity *e = new_h_e;
	bomb_handle = new_h_mb;

	e->add(new_h_n);
	e->add(new_h_mb);
	e->add(new_h_t);
	e->add(new_h_sm);

	TCompRenderStaticMesh *sm = new_h_sm;
	sm->static_mesh = Resources.get("static_meshes/bomb_static.static_mesh")->as<CStaticMesh>();
	sm->registerToRender();

	

	TCompTransform *static_trans = e->get<TCompTransform>();
	static_trans->setPosition(player_position);

	TCompName *name_c = e->get<TCompName>();
	name_c->setName("static_bomb");

	float yaw, pitch;
	player_transform->getAngles(&yaw, &pitch);
	static_trans->setAngles(yaw, pitch);
}

#pragma endregion

//##########################################################################

// Sets the entity
void player_controller_cientifico::SetMyEntity() {
	PROFILE_FUNCTION("player cientico set my entity");
	myEntity = myParent;
}

void player_controller_cientifico::renderInMenu()
{
	PROFILE_FUNCTION("player cientifico: render in menu");
	VEC3 direction = directionForward + directionLateral;
	direction.Normalize();
	direction = direction + directionJump;

	ImGui::Text("NODE: %s\n", state.c_str());
	ImGui::Text("direction: %.4f, %.4f, %.4f", direction.x, direction.y, direction.z);
	ImGui::Text("jump: %.5f", jspeed);
	ImGui::Text("object: %d\n", obj);
}

void player_controller_cientifico::UpdateUnpossess() {
	PROFILE_FUNCTION("player cientifico: update unposses");
	CHandle h = CHandle(this);
	tags_manager.removeTag(h.getOwner(), getID("player"));
}

void player_controller_cientifico::DisabledState() {	
	PROFILE_FUNCTION("player cientifico: disabled satte");
}
void player_controller_cientifico::InitControlState() {
	PROFILE_FUNCTION("player cientifico: init control state");
	CHandle h = CHandle(this);
	tags_manager.addTag(h.getOwner(), getID("player"));
	ChangeState("idle");
}
CEntity* player_controller_cientifico::getMyEntity() {
	PROFILE_FUNCTION("player cientifico: get my entity");
	CHandle me = CHandle(this);
	return me.getOwner();
}

void player_controller_cientifico::update_msgs()
{
	PROFILE_FUNCTION("player cientifico: update mesg");
	ui.addTextInstructions("Press 'shift' to exit possession\n");
	if (obj == EMPTY) {
		ui.addTextInstructions("Press '1' to create object to disable beacons\n");
		ui.addTextInstructions("Press '3' to create static bombs\n");
		ui.addTextInstructions("Press '5' to create magnetic bombs\n");
	}
	else if (obj == DISABLE_BEACON) {
		ui.addTextInstructions("Press '2' to disable beacon next to it");
	}
	else if (obj == MAGNETIC_BOMB) {
		ui.addTextInstructions("Can reduce the range and movements from guards");
		ui.addTextInstructions("Press '6' to shot magnetic bomb");
	}
	else if (obj == STATIC_BOMB) {
		ui.addTextInstructions("Can stun NPC when explode");
		ui.addTextInstructions("Press '4' to leave static bomb");
	}
	else if (obj == STATIC_BOMB_GAME) {
		ui.addTextInstructions("Press '1' to create object to disable beacons\n");
		ui.addTextInstructions("Press '3' to create static bombs\n");
		ui.addTextInstructions("Press '5' to create magnetic bombs\n");

		ui.addTextInstructions("\nPress 'R' to explode static bomb before time\n");
		std::string text = "To explode in: "+to_string(t_to_explode);
		ui.addTextInstructions(text);

	}
	else if (obj == MAGNETIC_BOMB_GAME) {
		ui.addTextInstructions("Press '1' to create object to disable beacons\n");
		ui.addTextInstructions("Press '3' to create static bombs\n");
		ui.addTextInstructions("Press '5' to create magnetic bombs\n");

		ui.addTextInstructions("\nPress 'R' to explode magnetic bomb before time\n");
		std::string text = "To explode in: " + to_string(t_to_explode);
		ui.addTextInstructions(text);

	}
}

void player_controller_cientifico::myUpdate()
{
	PROFILE_FUNCTION("player cientifico: myUpdate");
	if (obj == STATIC_BOMB_GAME || obj == MAGNETIC_BOMB_GAME) {
		t_to_explode -= getDeltaTime();
		if (t_to_explode <= 0.0f) {
			obj = EMPTY;
			t_to_explode = 5.0f;
		}
	}
}

void player_controller_cientifico::SetCharacterController()
{
	PROFILE_FUNCTION("player cientifico: setCharacter controller");
	SetMyEntity();
	cc = myEntity->get<TCompCharacterController>();
}
