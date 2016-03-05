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

void player_controller_cientifico::Init() {
	om = getHandleManager<player_controller_cientifico>();	//player

	//Specific Scientist nodes
	AddState("createMagneticBomb", (statehandler)&player_controller_cientifico::CreateMagneticBomb);
	AddState("createDisableBeacon", (statehandler)&player_controller_cientifico::CreateDisableBeacon);
	AddState("createStaticBomb", (statehandler)&player_controller_cientifico::CreateStaticBomb);
	AddState("addDisableBeacon", (statehandler)&player_controller_cientifico::AddDisableBeacon);
	AddState("useMagneticBomb", (statehandler)&player_controller_cientifico::UseMagneticBomb);
	AddState("useStaticBomb", (statehandler)&player_controller_cientifico::UseStaticBomb);

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
	energyDecreasal(getDeltaTime()*0.5f);
	if (io->keys['1'].becomesPressed()) {
		ChangeState("createDisableBeacon");
	}

	if (io->keys['2'].becomesPressed()) {
		ChangeState("addDisableBeacon");
	}

	if (io->keys['3'].becomesPressed()) {
		energyDecreasal(5.0f);
		ChangeState("createStaticBomb");
	}

	if (io->keys['5'].becomesPressed()) {
		energyDecreasal(5.0f);
		ChangeState("createMagneticBomb");
	}
	if (io->keys['6'].becomesPressed()) {
		energyDecreasal(10.0f);
		ChangeState("useMagneticBomb");
	}
	if (io->keys['4'].becomesPressed()) {
		energyDecreasal(10.0f);
		ChangeState("useStaticBomb");
	}
	if (io->keys['R'].becomesPressed())
		ExplodeBomb();
}

void player_controller_cientifico::ExplodeBomb()
{
	if (obj == STATIC_BOMB_GAME) {
		CStaticBomb *bomb = bomb_handle;
		obj = EMPTY;
		bomb->Explode();
	}
	
	if (obj == MAGNETIC_BOMB_GAME) {
		CMagneticBomb *bomb = bomb_handle;
		obj = EMPTY;
		bomb->Explode();
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
	if (obj == DISABLE_BEACON) {
		VEntities es = tags_manager.getHandlesPointerByTag(getID("beacon"));
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
	if (obj == MAGNETIC_BOMB) createMagneticBombEntity();
	obj = MAGNETIC_BOMB_GAME;
	ChangeState("idle");
}

void player_controller_cientifico::UseStaticBomb()
{
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
	CEntity *e = new_h_e;
	bomb_handle = new_h_mb;

	e->add(new_h_n);
	e->add(new_h_mb);
	e->add(new_h_t);

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
	CEntity *e = new_h_e;
	bomb_handle = new_h_mb;

	e->add(new_h_n);
	e->add(new_h_mb);
	e->add(new_h_t);

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
	myEntity = myParent;
}

void player_controller_cientifico::renderInMenu()
{
	VEC3 direction = directionForward + directionLateral;
	direction.Normalize();
	direction = direction + directionJump;

	ImGui::Text("NODE: %s\n", state.c_str());
	ImGui::Text("direction: %.4f, %.4f, %.4f", direction.x, direction.y, direction.z);
	ImGui::Text("jump: %.5f", jspeed);
}

void player_controller_cientifico::DisabledState() {
}
void player_controller_cientifico::InitControlState() {
	ChangeState("idle");
}
CEntity* player_controller_cientifico::getMyEntity() {
	CHandle me = CHandle(this);
	return me.getOwner();
}

void player_controller_cientifico::update_msgs()
{
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
	if (obj == STATIC_BOMB_GAME || obj == MAGNETIC_BOMB_GAME) {
		t_to_explode -= getDeltaTime();
		if (t_to_explode <= 0.0f) {
			obj = EMPTY;
			t_to_explode = 5.0f;
		}
	}
}
