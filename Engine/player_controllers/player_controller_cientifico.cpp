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
#include "app_modules/gui/gui.h"

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_SCIENTIST(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_SCIENTIST_P(state) SET_ANIM_STATE_P(animController, state)

map<string, statehandler> player_controller_cientifico::statemap = {};
map<int, string> player_controller_cientifico::out = {};
std::vector<std::string> player_controller_cientifico::objs_names;

void player_controller_cientifico::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_cientifico")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields_base = readIniAtrData(file_ini, "controller_base");

			assignValueToVar(player_max_speed, fields_base);
			assignValueToVar(player_rotation_speed, fields_base);
			assignValueToVar(jimpulse, fields_base);
			assignValueToVar(left_stick_sensibility, fields_base);
			assignValueToVar(camera_max_height, fields_base);
			assignValueToVar(camera_min_height, fields_base);

			map<std::string, float> fields_scientist = readIniAtrData(file_ini, "controller_scientist");

			assignValueToVar(t_waiting, fields_scientist);
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

bool player_controller_cientifico::getUpdateInfo()
{
	if (!CPlayerBase::getUpdateInfo()) return false;
	animController = GETH_MY(SkelControllerPlayer);
	return true;
}

void player_controller_cientifico::Init() {
	// read main attributes from file
	readIniFileAttr();

	for (int amoung : objs_amoung) amoung = 0;

	if (statemap.empty()) {
		//States from controller base and poss controller
		addBasicStates();
		addPossStates();

		//Specific Scientist nodes
		AddState("createBomb", (statehandler)&player_controller_cientifico::CreateBomb);
		AddState("useBomb", (statehandler)&player_controller_cientifico::UseBomb);
		AddState("throwing", (statehandler)&player_controller_cientifico::Throwing);
		AddState("next_bomb", (statehandler)&player_controller_cientifico::NextBomb);
		AddState("repairDrone", (statehandler)&player_controller_cientifico::RepairDrone);
		//AddState("createStaticBomb", (statehandler)&player_controller_cientifico::CreateStaticBomb);
		//AddState("addDisableBeacon", (statehandler)&player_controller_cientifico::AddDisableBeacon);
		//AddState("useStaticBomb", (statehandler)&player_controller_cientifico::UseStaticBomb);
	}
	if (objs_names.empty()) {
		objs_names.resize(eObjSci::OBJ_SCI_SIZE);
		objs_names[eObjSci::EMPTY] = "empty";
		objs_names[eObjSci::DISABLE_BEACON] = "disable_beacon";
		objs_names[eObjSci::MAGNETIC_BOMB] = "magnetic_bomb"; //Only this is used at the moment!
		objs_names[eObjSci::STATIC_BOMB] = "static_bomb";
		objs_names[eObjSci::THROW_BOMB] = "throw_bomb";
	}

	myHandle = CHandle(this);
	myParent = myHandle.getOwner();
	ChangeState("idle");
	
	getUpdateInfoBase(CHandle(this).getOwner());
	SET_ANIM_SCIENTIST(AST_IDLE);
	//animController.setState(AST_IDLE);

	//Test
	____TIMER_REDEFINE_(t_throwing, 1.f);
	____TIMER_REDEFINE_(t_nextBomb, 1.f);
}

//##########################################################################

//##########################################################################
//Inputs
//##########################################################################
#pragma region Inputs

void player_controller_cientifico::WorkBenchActions() {
	PROFILE_FUNCTION("player cientifico: WorbBenchActions");
	VEC3 myPos = transform->getPosition();

	// Find nearest workbench
	//====================================================
	VEC3 nearest_wb;
	float dist_wb = FLT_MAX;
	//====================================================
	for (VEC3 wb_pos : SBB::readVEC3Vector("wb_pos")) {
		if (distY(myPos, wb_pos) < 5.f) {
			float dist = simpleDistXZ(myPos, wb_pos);
			if (dist < dist_wb) {
				dist_wb = dist;
				nearest_wb = wb_pos;
			}
		}
	}
	//----------------------------------------------------

	if (dist_wb < 1.5f) {
		if (io->keys['E'].becomesPressed() || io->mouse.left.becomesPressed()) { //io->keys['1'].becomesPressed() || io->joystick.button_X.becomesPressed()) {
			obj = THROW_BOMB;
			//TODO: Destruir bomba actual
			ChangeState("createBomb");
			moving = false;
		}
		else {
			Gui->setActionAvailable(eAction::CREATE_MAGNETIC_BOMB);
		}
	}
	else if (canRepairDrone) {
		if (io->keys['E'].becomesPressed() || io->mouse.left.becomesPressed()) {
			logic_manager->throwEvent(logic_manager->OnRepairDrone, "");
			ChangeState("repairDrone");
		}
		else {
			Gui->setActionAvailable(eAction::REPAIR_DRONE);
		}
	}
}

void player_controller_cientifico::UpdateInputActions() {
	PROFILE_FUNCTION("player cientifico: energy dec");
	//if (io->keys['1'].becomesPressed() || io->joystick.button_X.becomesPressed()) {
	//	ChangeState("createDisableBeacon");
	//}

	//if (io->keys['2'].becomesPressed() || io->joystick.button_B.becomesPressed()) {
	//	ChangeState("addDisableBeacon");
	//}

	//if (io->keys['3'].becomesPressed() || io->joystick.button_Y.becomesPressed()) {
	//	energyDecreasal(5.0f);
	//	ChangeState("createStaticBomb");
	//}

	//if (io->keys['5'].becomesPressed() || io->joystick.button_L.becomesPressed()) {
	//	energyDecreasal(5.0f);
	//	ChangeState("createMagneticBomb");
	//}
	//if (io->keys['6'].becomesPressed() || io->joystick.button_LT > io->joystick.max_trigger_value / 2) {
	//	energyDecreasal(10.0f);
	//	ChangeState("useMagneticBomb");
	//}
	//if (io->keys['4'].becomesPressed() || io->joystick.button_RT > io->joystick.max_trigger_value / 2) {
	//	energyDecreasal(10.0f);
	//	ChangeState("useStaticBomb");
	//}
	//if (io->keys['R'].becomesPressed() || io->joystick.button_R.becomesPressed())
	//	ExplodeBomb();
}

//void player_controller_cientifico::ExplodeBomb()
//{
//	PROFILE_FUNCTION("player cientifico: explode bomb");
//	if (obj == STATIC_BOMB_GAME && bomb_handle.isValid()) {
//		CStaticBomb *bomb = bomb_handle;
//		obj = EMPTY;
//		bomb->toExplode();
//	}
//
//	if (obj == MAGNETIC_BOMB_GAME && bomb_handle.isValid()) {
//		CMagneticBomb *bomb = bomb_handle;
//		obj = EMPTY;
//		bomb->toExplode();
//	}
//}

#pragma endregion

//##########################################################################

//##########################################################################
// Player States
//##########################################################################
#pragma region Player States

void player_controller_cientifico::Idle()
{
	PROFILE_FUNCTION("player cientifico: idle state");
	CPlayerBase::Idle();
	RecalcScientist();
}

void player_controller_cientifico::Moving()
{
	PROFILE_FUNCTION("player cientifico: idle state");
	CPlayerBase::Idle();
	RecalcScientist();
}

void player_controller_cientifico::RecalcScientist() {
	if (io->keys['1'].becomesPressed() || io->joystick.button_X.becomesPressed()) {
		ChangeState("useBomb");
	}
	WorkBenchActions();
}

void player_controller_cientifico::CreateBomb()
{
	PROFILE_FUNCTION("player cientifico: create bomb");
	t_waiting += getDeltaTime();
	if (t_waiting >= t_create_MagneticBomb) {
		dbg("bomb created!\n");
		t_waiting = 0;
		objs_amoung[obj] = 5;
		if (bomb_handle.isValid()) bomb_handle.destroy();
		spawnBomb();
		//bomb_handle.sendMsg(TMsgActivate());
		ChangeState("idle");
	}
	else {
		Gui->setActionAvailable(eAction::CREATING);
	}
}

void player_controller_cientifico::UseBomb()
{
	PROFILE_FUNCTION("player cientifico: use bomb");
	if (objs_amoung[obj] > 0) {
		objs_amoung[obj]--;
		ChangeState("throwing");
		bomb_handle.sendMsg(TMsgActivate()); //Notify throwing
		moving = false;
		//if (objs_amoung[obj] > 0) spawnBomb();
		//ChangeState("idle");
	}
	else {
		dbg("No bombs remain!");
		//TODO?
	}
}

void player_controller_cientifico::Throwing()
{
	PROFILE_FUNCTION("player cientifico: throwing bomb");
	//Animacion lanzar bomba

	//Test mover bomba
	____TIMER_CHECK_DO_(t_throwing);
	TMsgThrow msgThrow;
	msgThrow.dir = transform->getFront();
	bomb_handle.sendMsg(msgThrow); //Notify throwed (in the air)
	if (objs_amoung[obj] > 0) ChangeState("next_bomb");
	else ChangeState("idle");
	____TIMER_CHECK_DONE_(t_throwing);
}

void player_controller_cientifico::NextBomb()
{
	PROFILE_FUNCTION("player cientifico: next bomb");
	//Animacion sacar bomba?

	//Test mover bomba
	____TIMER_CHECK_DO_(t_nextBomb);
	spawnBomb();
	ChangeState("idle");
	____TIMER_CHECK_DONE_(t_nextBomb);
}

void player_controller_cientifico::RepairDrone()
{
	TMsgRepair msg;
	CEntity *drone_e = drone;
	drone_e->sendMsg(msg);
	ChangeState("idle");
}

//void player_controller_cientifico::CreateStaticBomb()
//{
//	PROFILE_FUNCTION("player cientifico: create static");
//	t_waiting += getDeltaTime();
//	if (t_waiting >= t_create_StaticBomb) {
//		dbg("static bomb created!\n");
//		t_waiting = 0;
//		obj = STATIC_BOMB;
//		ChangeState("idle");
//	}
//}

//void player_controller_cientifico::UseStaticBomb()
//{
//	PROFILE_FUNCTION("player cientifico: use static bomb");
//	if (obj == STATIC_BOMB) createStaticBombEntity();
//	obj = STATIC_BOMB_GAME;
//	ChangeState("idle");
//}

//void player_controller_cientifico::CreateDisableBeacon()
//{
//	PROFILE_FUNCTION("player cientifico: create disable beac");
//	//TODO: input cancel
//	t_waiting += getDeltaTime();
//	if (t_waiting >= t_create_beacon) {
//		t_waiting = 0;
//		dbg("disable beacon created!\n");
//		obj = DISABLE_BEACON;
//		ChangeState("idle");
//	}
//}

//void player_controller_cientifico::AddDisableBeacon()
//{
//	PROFILE_FUNCTION("player cientifico: add disable beac");
//	if (obj == DISABLE_BEACON) {
//		VHandles es = tags_manager.getHandlesByTag(getID("beacon"));
//		TMsgBeaconBusy msg;
//		bool resp = false;
//		msg.reply = &resp;
//		CEntity *p_e = myParent;
//		TCompTransform *mtx = p_e->get<TCompTransform>();
//		msg.pos = mtx->getPosition();		//TODO: pos player
//		for (CEntity *e : es) {
//			e->sendMsgWithReply(msg);
//			if (*msg.reply) {
//				//dbg("go idle\n");
//				obj = EMPTY;
//				ChangeState("idle");
//				break;
//			}
//		}
//	}
//
//	ChangeState("idle");
//}

#pragma endregion

//##########################################################################

//##########################################################################
//Create Game Objects
//##########################################################################

#pragma region Game objects creators

void player_controller_cientifico::spawnBomb()
{
	PROFILE_FUNCTION("player cientifico: create mag bomb");

	bomb_handle = spawnPrefab(objs_names[obj]);

	TMsgAttach msg;
	msg.handle = CHandle(this).getOwner();
	msg.bone_name = SK_RHAND;
	bomb_handle.sendMsg(msg); //TODO por aqui
	//float yaw, pitch;
	//bomb_trans->setPosition(transform->getPosition());
	//transform->getAngles(&yaw, &pitch);
	//bomb_trans->setAngles(yaw, pitch);

	switch (obj) {
	case STATIC_BOMB:
	{
		GET_COMP(bomb_component, bomb_handle, CStaticBomb);
		//bomb_component->Init();
	}
	break;
	case MAGNETIC_BOMB:
	{
		GET_COMP(bomb_component, bomb_handle, CMagneticBomb);
		//bomb_component->Init();
	}
	break;
	case THROW_BOMB:
	{
		GET_COMP(bomb_component, bomb_handle, CThrowBomb);
		//bomb_component->Init(3.f, 1.f);
	}
	break;
	default:
		assert(false || fatal("Error: unknown bomb type\n"));
		break;
	}
}

//void player_controller_cientifico::spawnStaticBombEntity()
//{
//	PROFILE_FUNCTION("player cientifico: create static bomb entity");
//	VEC3 player_position = transform->getPosition();
//
//	auto hm_e = CHandleManager::getByName("entity");
//	CHandle new_h_e = hm_e->createHandle();
//
//	auto hm_n = CHandleManager::getByName("name");
//	CHandle new_h_n = hm_n->createHandle();
//
//	auto hm_t = CHandleManager::getByName("transform");
//	CHandle new_h_t = hm_t->createHandle();
//
//	auto hm_mb = CHandleManager::getByName("static_bomb");
//	CHandle new_h_mb = hm_mb->createHandle();
//
//	auto hm_sm = CHandleManager::getByName("render_static_mesh");
//	CHandle new_h_sm = hm_sm->createHandle();
//
//	CEntity *e = new_h_e;
//	bomb_handle = new_h_mb;
//
//	e->add(new_h_n);
//	e->add(new_h_mb);
//	e->add(new_h_t);
//	e->add(new_h_sm);
//
//	TCompRenderStaticMesh *sm = new_h_sm;
//	sm->static_mesh = Resources.get("static_meshes/bomb_static.static_mesh")->as<CStaticMesh>();
//	sm->registerToRender();
//
//	TCompTransform *static_trans = e->get<TCompTransform>();
//	static_trans->setPosition(player_position);
//
//	TCompName *name_c = e->get<TCompName>();
//	name_c->setName("static_bomb");
//
//	float yaw, pitch;
//	transform->getAngles(&yaw, &pitch);
//	static_trans->setAngles(yaw, pitch);
//}

#pragma endregion

//##########################################################################

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
	//CHandle h = CHandle(this);
	//tags_manager.addTag(h.getOwner(), getID("player"));
	ChangeState("idle");
}

CEntity* player_controller_cientifico::getMyEntity() {
	PROFILE_FUNCTION("player cientifico: get my entity");
	CHandle me = CHandle(this);
	return me.getOwner();
}

//void player_controller_cientifico::update_msgs()
//{
//	PROFILE_FUNCTION("player cientifico: update mesg");
//	ui.addTextInstructions("Press 'shift' to exit possession\n");
//	if (obj == EMPTY) {
//		ui.addTextInstructions("Press '1' to create object to disable beacons\n");
//		ui.addTextInstructions("Press '3' to create static bombs\n");
//		ui.addTextInstructions("Press '5' to create magnetic bombs\n");
//	}
//	else if (obj == DISABLE_BEACON) {
//		ui.addTextInstructions("Press '2' to disable beacon next to it");
//	}
//	else if (obj == MAGNETIC_BOMB) {
//		ui.addTextInstructions("Can reduce the range and movements from guards");
//		ui.addTextInstructions("Press '6' to shot magnetic bomb");
//	}
//	else if (obj == STATIC_BOMB) {
//		ui.addTextInstructions("Can stun NPC when explode");
//		ui.addTextInstructions("Press '4' to leave static bomb");
//	}
//	else if (obj == STATIC_BOMB_GAME) {
//		ui.addTextInstructions("Press '1' to create object to disable beacons\n");
//		ui.addTextInstructions("Press '3' to create static bombs\n");
//		ui.addTextInstructions("Press '5' to create magnetic bombs\n");
//
//		ui.addTextInstructions("\nPress 'R' to explode static bomb before time\n");
//		std::string text = "To explode in: " + to_string(t_to_explode);
//		ui.addTextInstructions(text);
//	}
//	else if (obj == MAGNETIC_BOMB_GAME) {
//		ui.addTextInstructions("Press '1' to create object to disable beacons\n");
//		ui.addTextInstructions("Press '3' to create static bombs\n");
//		ui.addTextInstructions("Press '5' to create magnetic bombs\n");
//
//		ui.addTextInstructions("\nPress 'R' to explode magnetic bomb before time\n");
//		std::string text = "To explode in: " + to_string(t_to_explode);
//		ui.addTextInstructions(text);
//	}
//}

void player_controller_cientifico::myUpdate()
{
	PROFILE_FUNCTION("player cientifico: myUpdate");

	//Update del componente?
	//if (obj == STATIC_BOMB_GAME || obj == MAGNETIC_BOMB_GAME) {
	//	t_to_explode -= getDeltaTime();
	//	if (t_to_explode <= 0.0f) {
	//		obj = EMPTY;
	//		t_to_explode = 5.0f;
	//	}
	//}
}

void player_controller_cientifico::onCanRepairDrone(const TMsgCanRechargeDrone & msg)
{
	canRepairDrone = msg.range;
	drone = msg.han;
}

//Anims
void player_controller_cientifico::ChangeCommonState(std::string state) {
	if (state == "moving") {
		SET_ANIM_SCIENTIST(AST_MOVE);
	}
	else if (state == "running") {
		SET_ANIM_SCIENTIST(AST_RUN);
	}
	else if (state == "jumping") {
		SET_ANIM_SCIENTIST(AST_JUMP);
	}
	else if (state == "idle") {
		SET_ANIM_SCIENTIST(AST_IDLE);
	}
}