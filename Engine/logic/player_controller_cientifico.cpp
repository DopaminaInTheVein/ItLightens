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

#include "components\comp_msgs.h"

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
	if (Input.IsKeyPressedDown(DIK_1)) {
		ChangeState("createDisableBeacon");
	}

	if (Input.IsKeyPressedDown(DIK_2)) {
		ChangeState("addDisableBeacon");
	}

	if (Input.IsKeyPressedDown(DIK_3)) {
		energyDecreasal(5.0f);
		ChangeState("createStaticBomb");
	}

	if (Input.IsKeyPressedDown(DIK_5)) {
		energyDecreasal(5.0f);
		ChangeState("createMagneticBomb");
	}
	if (Input.IsKeyPressedDown(DIK_6)) {
		energyDecreasal(10.0f);
		ChangeState("useMagneticBomb");
	}
	if (Input.IsKeyPressedDown(DIK_4)) {
		energyDecreasal(10.0f);
		ChangeState("useStaticBomb");
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
	ChangeState("idle");
}

void player_controller_cientifico::UseStaticBomb()
{
	if (obj == STATIC_BOMB) createStaticBombEntity();
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