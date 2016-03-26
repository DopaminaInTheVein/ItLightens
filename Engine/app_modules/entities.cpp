#include "mcv_platform.h"
#include "app_modules/entities.h"
#include "handle/handle_manager.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "handle/msgs.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "render/technique.h"
#include "resources/resources_manager.h"
#include "imgui/imgui.h"
#include "logic/sbb.h"
#include "logic/ai_water.h"
#include "logic/bt_guard.h"
#include "logic/bt_mole.h"
#include "logic/bt_speedy.h"
#include "windows/app.h"
#include "utils/utils.h"
#include <vector>

DECL_OBJ_MANAGER("entity", CEntity);		//need to be first
DECL_OBJ_MANAGER("name", TCompName);
DECL_OBJ_MANAGER("transform", TCompTransform);
DECL_OBJ_MANAGER("camera", TCompCamera);
DECL_OBJ_MANAGER("controller_3rd_person", TCompController3rdPerson);
DECL_OBJ_MANAGER("render_static_mesh", TCompRenderStaticMesh);
DECL_OBJ_MANAGER("cientifico", ai_scientific);
DECL_OBJ_MANAGER("beacon", beacon_controller);
DECL_OBJ_MANAGER("workbench", workbench_controller);
/****/DECL_OBJ_MANAGER("ai_guard", ai_guard);
DECL_OBJ_MANAGER("bt_guard", bt_guard);
/****/DECL_OBJ_MANAGER("ai_mole", ai_mole);
DECL_OBJ_MANAGER("bt_mole", bt_mole);
/****/DECL_OBJ_MANAGER("ai_speedy", ai_speedy);
DECL_OBJ_MANAGER("bt_speedy", bt_speedy);
DECL_OBJ_MANAGER("water", water_controller);
DECL_OBJ_MANAGER("player", player_controller);
DECL_OBJ_MANAGER("player_speedy", player_controller_speedy);
DECL_OBJ_MANAGER("player_mole", player_controller_mole);
DECL_OBJ_MANAGER("player_cientifico", player_controller_cientifico);
DECL_OBJ_MANAGER("life", TCompLife);
DECL_OBJ_MANAGER("wire", TCompWire);
DECL_OBJ_MANAGER("generator", TCompGenerator);
DECL_OBJ_MANAGER("skeleton", TCompSkeleton);

//Physics
DECL_OBJ_MANAGER("rigidbody", TCompPhysics);
DECL_OBJ_MANAGER("character_controller", TCompCharacterController);

//prefabs
DECL_OBJ_MANAGER("magnetic_bomb", CMagneticBomb);
DECL_OBJ_MANAGER("static_bomb", CStaticBomb);
DECL_OBJ_MANAGER("polarized", TCompPolarized);

static CHandle player;
static CHandle target;
CCamera * camera;

// The global dict of all msgs
MMsgSubscriptions msg_subscriptions;
TMsgID generateUniqueMsgID() {
	static TMsgID next_msg_id = 1;
	return next_msg_id++;
}

bool CEntitiesModule::start() {
	SBB::init();

	getHandleManager<CEntity>()->init(MAX_ENTITIES);

	getHandleManager<player_controller>()->init(8);
	getHandleManager<player_controller_speedy>()->init(8);
	getHandleManager<player_controller_mole>()->init(8);
	getHandleManager<player_controller_cientifico>()->init(8);
	getHandleManager<TCompRenderStaticMesh>()->init(MAX_ENTITIES);
	getHandleManager<TCompSkeleton>()->init(MAX_ENTITIES);
	getHandleManager<TCompName>()->init(MAX_ENTITIES);
	getHandleManager<TCompTransform>()->init(MAX_ENTITIES);
	getHandleManager<TCompRenderStaticMesh>()->init(MAX_ENTITIES);
	getHandleManager<TCompCamera>()->init(4);
	getHandleManager<TCompController3rdPerson>()->init(4);
	getHandleManager<TCompLife>()->init(MAX_ENTITIES);
	getHandleManager<TCompWire>()->init(10);
	getHandleManager<TCompGenerator>()->init(10);
	getHandleManager<TCompPolarized>()->init(MAX_ENTITIES);

	getHandleManager<bt_guard>()->init(MAX_ENTITIES);
	getHandleManager<bt_mole>()->init(MAX_ENTITIES);
	getHandleManager<bt_speedy>()->init(MAX_ENTITIES);
	getHandleManager<ai_scientific>()->init(MAX_ENTITIES);
	getHandleManager<beacon_controller>()->init(MAX_ENTITIES);
	getHandleManager<workbench_controller>()->init(MAX_ENTITIES);
	getHandleManager<water_controller>()->init(MAX_ENTITIES);

	getHandleManager<CStaticBomb>()->init(MAX_ENTITIES);
	getHandleManager<CMagneticBomb>()->init(MAX_ENTITIES);

	//colliders
	getHandleManager<TCompPhysics>()->init(MAX_ENTITIES);
	getHandleManager<TCompCharacterController>()->init(MAX_ENTITIES);

	//SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(TCompLife, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompPhysics, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompCharacterController, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(TCompController3rdPerson, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller_speedy, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller_mole, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(ai_speedy, TMsgSetPlayer, onSetPlayer);
	SUBSCRIBE(bt_speedy, TMsgSetPlayer, onSetPlayer);
	SUBSCRIBE(water_controller, TMsgSetWaterType, onSetWaterType);
	SUBSCRIBE(ai_scientific, TMsgBeaconToRemove, onRemoveBeacon);			//Beacon to remove
	SUBSCRIBE(ai_scientific, TMsgBeaconEmpty, onEmptyBeacon);				//Beacon empty
	SUBSCRIBE(ai_scientific, TMsgWBEmpty, onEmptyWB);						//Workbench empty
	SUBSCRIBE(TCompRenderStaticMesh, TMsgEntityCreated, onCreate);

	SUBSCRIBE(beacon_controller, TMsgBeaconBusy, onPlayerAction);
	SUBSCRIBE(ai_scientific, TMsgBeaconTakenByPlayer, onTakenBeacon);
	SUBSCRIBE(ai_scientific, TMsgWBTakenByPlayer, onTakenWB);

	//bombs
	SUBSCRIBE(ai_scientific, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_guard, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_mole, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_speedy, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_guard, TMsgMagneticBomb, onMagneticBomb);
	SUBSCRIBE(bt_guard, TMsgNoise, noise);

	//WIRES
	SUBSCRIBE(TCompWire, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgWirePass, onWirePass);

	//generator
	SUBSCRIBE(TCompGenerator, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgCanRec, onCanRec);

	//polarized
	SUBSCRIBE(TCompPolarized, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgPolarize, onPolarize);
	SUBSCRIBE(TCompPolarized, TMsgPlayerPolarize, onPolarize);

	//Posesiones Mensajes
	//..Cientifico
	SUBSCRIBE(ai_scientific, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(ai_scientific, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_cientifico, TMsgControllerSetEnable, onSetEnable);
	//..Speedy
	SUBSCRIBE(bt_speedy, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(bt_speedy, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_speedy, TMsgControllerSetEnable, onSetEnable);
	//..Mole
	SUBSCRIBE(bt_mole, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(bt_mole, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_mole, TMsgControllerSetEnable, onSetEnable);

	//..PJ Principal
	SUBSCRIBE(player_controller, TMsgPossessionLeave, onLeaveFromPossession);

	//Damage
	SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(player_controller, TMsgDamage, onDamage);
	SUBSCRIBE(player_controller_cientifico, TMsgDamage, onDamage);
	SUBSCRIBE(player_controller_speedy, TMsgDamage, onDamage);
	SUBSCRIBE(player_controller_mole, TMsgDamage, onDamage);

	CEntityParser ep;
	//bool is_ok = ep.xmlParseFile("data/scenes/scene_milestone_1.xml");
	bool is_ok = ep.xmlParseFile("data/scenes/pruebaExportador.xml");
	assert(is_ok);

	TTagID tagIDplayer = getID("player");
	TTagID tagIDbox = getID("box");
	TTagID tagIDboxleave = getID("box_leavepoint");
	TTagID tagIDwall = getID("breakable_wall");
	TTagID tagIDminus = getID("minus_wall");
	TTagID tagIDplus = getID("plus_wall");
	TTagID tagIDrec = getID("recover_point");

	// Camara del player
	player = tags_manager.getFirstHavingTag(tagIDplayer);
	CEntity * player_e = player;
	TCompCamera * pcam = player_e->get<TCompCamera>();
	camera = pcam;
	// Player real
	CHandle t = tags_manager.getFirstHavingTag(getID("target"));
	CEntity * target_e = t;

	// Set the player in the 3rdPersonController
	if (player_e && t.isValid()) {
		TMsgSetTarget msg;
		msg.target = t;
		player_e->sendMsg(msg);

		TMsgSetCamera msg_camera;
		msg_camera.camera = player;
		target_e->sendMsg(msg_camera);
	}

	// Set the player in the Speedy AIs
	TTagID tagIDSpeedy = getID("AI_speedy");
	VHandles speedyHandles = tags_manager.getHandlesByTag(tagIDSpeedy);

	for (CHandle speedyHandle : speedyHandles) {
		CEntity * speedy_e = speedyHandle;
		TMsgSetPlayer msg_player;
		msg_player.player = t;
		speedy_e->sendMsg(msg_player);
	}

	// Set the type for the starting water zones to 0 (PERMANENT)
	TTagID tagIDWater = getID("water");
	VHandles waterHandles = tags_manager.getHandlesByTag(tagIDWater);

	for (CHandle waterHandle : waterHandles) {
		CEntity * water_e = waterHandle;
		TMsgSetWaterType msg_water;
		msg_water.type = 0;
		water_e->sendMsg(msg_water);
	}

	SBB::postHandlesVector("wptsBoxes", tags_manager.getHandlesByTag(tagIDbox));
	SBB::postHandlesVector("wptsBreakableWall", tags_manager.getHandlesByTag(tagIDwall));
	SBB::postHandlesVector("wptsBoxLeavePoint", tags_manager.getHandlesByTag(tagIDboxleave));
	SBB::postHandlesVector("wptsMinusPoint", tags_manager.getHandlesByTag(tagIDminus));
	SBB::postHandlesVector("wptsPlusPoint", tags_manager.getHandlesByTag(tagIDplus));
	SBB::postHandlesVector("wptsRecoverPoint", tags_manager.getHandlesByTag(tagIDrec));

	getHandleManager<player_controller>()->onAll(&player_controller::Init);
	getHandleManager<player_controller_speedy>()->onAll(&player_controller_speedy::Init);
	getHandleManager<player_controller_cientifico>()->onAll(&player_controller_cientifico::Init);
	getHandleManager<player_controller_mole>()->onAll(&player_controller_mole::Init);

	getHandleManager<bt_guard>()->onAll(&bt_guard::Init);
	getHandleManager<bt_mole>()->onAll(&bt_mole::Init);
	getHandleManager<bt_speedy>()->onAll(&bt_speedy::Init);
	getHandleManager<ai_scientific>()->onAll(&ai_scientific::Init);
	getHandleManager<water_controller>()->onAll(&water_controller::Init);
	getHandleManager<beacon_controller>()->onAll(&beacon_controller::Init);
	getHandleManager<workbench_controller>()->onAll(&workbench_controller::Init);
	getHandleManager<TCompWire>()->onAll(&TCompWire::init);
	getHandleManager<TCompPolarized>()->onAll(&TCompPolarized::init);

	return true;
}

void CEntitiesModule::stop() {
}

void CEntitiesModule::update(float dt) {
	// May need here a switch to update wich player controller takes the action - possession rulez
	getHandleManager<player_controller>()->updateAll(dt);
	getHandleManager<player_controller_speedy>()->updateAll(dt);
	getHandleManager<player_controller_mole>()->updateAll(dt);
	getHandleManager<player_controller_cientifico>()->updateAll(dt);

	getHandleManager<TCompController3rdPerson>()->updateAll(dt);
	getHandleManager<TCompCamera>()->updateAll(dt);

	getHandleManager<bt_mole>()->updateAll(dt);
	getHandleManager<bt_guard>()->updateAll(dt);
	getHandleManager<ai_scientific>()->updateAll(dt);
	getHandleManager<beacon_controller>()->updateAll(dt);
	getHandleManager<workbench_controller>()->updateAll(dt);
	getHandleManager<TCompSkeleton>()->updateAll(dt);
	getHandleManager<bt_speedy>()->updateAll(dt);
	getHandleManager<water_controller>()->updateAll(dt);

	getHandleManager<CStaticBomb>()->updateAll(dt);
	getHandleManager<CMagneticBomb>()->updateAll(dt);

	getHandleManager<TCompWire>()->updateAll(dt);
	getHandleManager<TCompGenerator>()->updateAll(dt);
	getHandleManager<TCompPolarized>()->updateAll(dt);

	//physx objects
	getHandleManager<TCompCharacterController>()->updateAll(dt);
	getHandleManager<TCompPhysics>()->updateAll(dt);
}

void CEntitiesModule::render() {
	// for each manager
	// if manager has debug render active
	// manager->renderAll()
	auto tech = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	tech->activate();

#ifdef _DEBUG
	//getHandleManager<TCompTransform>()->onAll(&TCompTransform::render);
#endif

	getHandleManager<TCompSkeleton>()->onAll(&TCompSkeleton::render);
	getHandleManager<TCompCamera>()->onAll(&TCompCamera::render);
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