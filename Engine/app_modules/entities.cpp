#include "mcv_platform.h"
#include "app_modules/entities.h"
#include "handle/handle_manager.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "handle/msgs.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "imgui/imgui.h"
#include "logic/aicontroller.h"
#include "logic/ai_mole.h"
#include "logic/sbb.h"
#include "input/input.h"
#include "windows/app.h"
#include "utils/utils.h"
#include <vector>

DECL_OBJ_MANAGER("entity", CEntity);
DECL_OBJ_MANAGER("name", TCompName);
DECL_OBJ_MANAGER("transform", TCompTransform);
DECL_OBJ_MANAGER("camera", TCompCamera);
DECL_OBJ_MANAGER("controller_3rd_person", TCompController3rdPerson);
DECL_OBJ_MANAGER("cientifico", ai_scientific);
DECL_OBJ_MANAGER("beacon", beacon_controller);
DECL_OBJ_MANAGER("ai_guard", ai_guard);
DECL_OBJ_MANAGER("ai_mole", ai_mole);
DECL_OBJ_MANAGER("ai_speedy", ai_speedy);
//DECL_OBJ_MANAGER("nombre_IA_xml", NameClass):
DECL_OBJ_MANAGER("life", TCompLife);

static vector<aicontroller*> ais;
static CHandle player;
static CHandle target;
CInput input;
CCamera * camera;

// The global dict of all msgs
MMsgSubscriptions msg_subscriptions;
TMsgID generateUniqueMsgID() {
	static TMsgID next_msg_id = 1;
	return next_msg_id++;
}

bool CEntitiesModule::start() {
	CApp& app = CApp::get();
	input.Initialize(app.getHInstance(), app.getHWnd(), 800, 600);

	getHandleManager<CEntity>()->init(MAX_ENTITIES);
	getHandleManager<TCompName>()->init(MAX_ENTITIES);
	getHandleManager<TCompTransform>()->init(MAX_ENTITIES);
	getHandleManager<TCompCamera>()->init(4);
	getHandleManager<TCompController3rdPerson>()->init(4);
	getHandleManager<TCompLife>()->init(MAX_ENTITIES);

	getHandleManager<ai_guard>()->init(MAX_ENTITIES);
	getHandleManager<ai_mole>()->init(MAX_ENTITIES);
	getHandleManager<ai_scientific>()->init(MAX_ENTITIES);
	getHandleManager<ai_speedy>()->init(MAX_ENTITIES);
	getHandleManager<beacon_controller>()->init(MAX_ENTITIES);

	SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(TCompLife, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(ai_scientific, TMsgBeaconToRemove, onRemoveBeacon);			//Beacon to remove
	SUBSCRIBE(ai_scientific, TMsgBeaconEmpty, onEmptyBeacon);				//Beacon empty

	CEntityParser ep;
	bool is_ok = ep.xmlParseFile("data/scenes/scene00.xml");
	assert(is_ok);

	TTagID tagIDplayer = getID("player");
	TTagID tagIDbox = getID("box");
	TTagID tagIDboxleave = getID("box_leavepoint");

	player = tags_manager.getFirstHavingTag(tagIDplayer);

	CEntity * player_e = player;
	TCompCamera * pcam = player_e->get<TCompCamera>();
	camera = pcam;
	CHandle t = tags_manager.getFirstHavingTag(getID("target"));

	if (player_e && t.isValid()) {
		TMsgSetTarget msg;
		msg.target = t;
		player_e->sendMsg(msg);
	}

	target = t;

	SBB::postHandles("wptsBoxes", tags_manager.getHandlesByTag(tagIDbox));
	SBB::postHandles("wptsBoxLeavePoint", tags_manager.getHandlesByTag(tagIDboxleave));

	//for (auto ai : logics) {
	//	ai_mole * moleAi = new ai_mole;
	//	moleAi->Init(ai, 3.50f);
	//	ais.push_back(moleAi);
	//}

	getHandleManager<ai_guard>()->onAll(&ai_guard::Init);
	getHandleManager<ai_mole>()->onAll(&ai_mole::Init);
	getHandleManager<ai_scientific>()->onAll(&ai_scientific::Init);
	//getHandleManager<ai_speedy>()->onAll(&ai_speedy::Init);
	getHandleManager<beacon_controller>()->onAll(&beacon_controller::Init);

	return true;
}

void CEntitiesModule::stop() {
	// Stop input
	input.Shutdown();
}

void CEntitiesModule::update(float dt) {
	CEntity * target_e = target;

	if (!ImGui::GetIO().WantCaptureKeyboard) {

		TCompTransform* player_transform = target_e->get<TCompTransform>();
		VEC3 position = player_transform->getPosition();
		VEC3 front = player_transform->getFront();
		dt = getDeltaTime();
		input.Frame();
		float yaw = 0.0f, pitch = 0.0f;
		player_transform->getAngles(&yaw, &pitch);

		if (input.IsUpPressed())
		{
			position.x += front.x * dt * 2;
			position.z += front.z * dt * 2;
		}
		if (input.IsDownPressed())
		{
			position.x -= front.x * dt * 2;
			position.z -= front.z * dt * 2;
		}
		if (input.IsLeftPressed())
		{
			position.x += front.z * dt * 2;
			position.z -= front.x * dt * 2;
		}
		if (input.IsRightPressed())
		{
			position.x -= front.z * dt * 2;
			position.z += front.x * dt * 2;
		}
		if (input.IsOrientLeftPressed())
		{
			player_transform->setAngles(yaw + dt, 0.0f);
			orbitCamera(dt);
		}
		if (input.IsOrientRightPressed())
		{
			player_transform->setAngles(yaw - dt, 0.0f);
			orbitCamera(-dt);
		}
		if (input.IsSpacePressed()) {
			dbg("SALTO!\n");
		}
		if (input.IsLeftClickPressed()) {
			dbg("ACCION!\n");
		}
		if (input.IsRightClickPressed()) {
			dbg("POSESION!\n");
		}

		player_transform->setPosition(position);

	}
	else
		input.Unacquire();

	getHandleManager<TCompController3rdPerson>()->updateAll(dt);
	getHandleManager<TCompCamera>()->updateAll(dt);

	getHandleManager<ai_mole>()->updateAll(dt);
	getHandleManager<ai_guard>()->updateAll(dt);
	getHandleManager<ai_scientific>()->updateAll(dt);
	getHandleManager<beacon_controller>()->updateAll(dt);
	//getHandleManager<ai_speedy>()->updateAll(dt);

	//for (aicontroller * ai : ais) {
	//	ai->Recalc();
	//}
	// Show a menu to modify any entity
	//renderInMenu();
}

void CEntitiesModule::render() {
	// for each manager
	// if manager has debug render active
	// manager->renderAll()
	getHandleManager<TCompTransform>()->onAll(&TCompTransform::render);
	getHandleManager<TCompCamera>()->onAll(&TCompCamera::render);

	//getHandleManager<NameClass>()->renderAll();
}

void CEntitiesModule::renderInMenu() {
	ImGui::Begin("Entities");
	if (ImGui::TreeNode("All entities...")) {
		getHandleManager<CEntity>()->onAll(&CEntity::renderInMenu);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Entities by Tag...")) {
		// Show all defined tags
		ImGui::TreePop();
	}
	ImGui::End();
}

void CEntitiesModule::orbitCamera(float angle) {
	float s = sin(angle);
	float c = cos(angle);
	CEntity * player_e = player;
	CEntity * target_e = target;

	// translate point back to origin:
	TCompTransform* player_transform = player_e->get<TCompTransform>();
	TCompTransform* target_transform = target_e->get<TCompTransform>();

	VEC3 entPos = player_transform->getPosition();
	entPos.x -= target_transform->getPosition().x;
	entPos.z -= target_transform->getPosition().z;

	// rotate point
	float xnew = entPos.x * c - entPos.z * s;
	float ynew = entPos.x * s + entPos.z * c;

	// translate point back:
	entPos.x = xnew + target_transform->getPosition().x;
	entPos.z = ynew + target_transform->getPosition().z;

	player_transform->setPosition(entPos);

	angle = player_transform->getDeltaYawToAimTo(target_transform->getPosition());
	float yaw = 0.0f, pitch = 0.0f;
	player_transform->getAngles(&yaw, &pitch);
	player_transform->setAngles(yaw + angle, 0.0f);
}