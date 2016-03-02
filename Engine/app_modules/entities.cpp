#include "mcv_platform.h"
#include "app_modules/entities.h"
#include "handle/handle_manager.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "handle/msgs.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "imgui/imgui.h"
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
DECL_OBJ_MANAGER("render_static_mesh", TCompRenderStaticMesh);
DECL_OBJ_MANAGER("cientifico", ai_scientific);
DECL_OBJ_MANAGER("beacon", beacon_controller);
DECL_OBJ_MANAGER("workbench", workbench_controller);
DECL_OBJ_MANAGER("ai_guard", ai_guard);
DECL_OBJ_MANAGER("ai_mole", ai_mole);
DECL_OBJ_MANAGER("ai_speedy", ai_speedy);
DECL_OBJ_MANAGER("player", player_controller);
DECL_OBJ_MANAGER("player_speedy", player_controller_speedy);
DECL_OBJ_MANAGER("player_mole", player_controller_mole);
DECL_OBJ_MANAGER("player_cientifico", player_controller_cientifico);
DECL_OBJ_MANAGER("life", TCompLife);

//Physics
DECL_OBJ_MANAGER("colCylinder", TCompColCillinder);

//prefabs
DECL_OBJ_MANAGER("magnetic_bomb", CMagneticBomb);
DECL_OBJ_MANAGER("static_bomb", CStaticBomb);

//colliders
DECL_OBJ_MANAGER("sphere_collider", sphereCollider);
DECL_OBJ_MANAGER("box_collider", boxCollider);

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
	getHandleManager<player_controller>()->init(8);
	getHandleManager<player_controller_speedy>()->init(8);
	getHandleManager<player_controller_mole>()->init(8);
	getHandleManager<player_controller_cientifico>()->init(8);

	getHandleManager<CEntity>()->init(MAX_ENTITIES);
	getHandleManager<TCompName>()->init(MAX_ENTITIES);
	getHandleManager<TCompTransform>()->init(MAX_ENTITIES);
	getHandleManager<TCompRenderStaticMesh>()->init(MAX_ENTITIES);
	getHandleManager<TCompCamera>()->init(4);
	getHandleManager<TCompController3rdPerson>()->init(4);
	getHandleManager<TCompLife>()->init(MAX_ENTITIES);

	//Physics
	getHandleManager<TCompColCillinder>()->init(MAX_ENTITIES);

	getHandleManager<ai_guard>()->init(MAX_ENTITIES);
	getHandleManager<ai_mole>()->init(MAX_ENTITIES);
	getHandleManager<ai_scientific>()->init(MAX_ENTITIES);
	getHandleManager<ai_speedy>()->init(MAX_ENTITIES);
	getHandleManager<beacon_controller>()->init(MAX_ENTITIES);
	getHandleManager<workbench_controller>()->init(MAX_ENTITIES);

	getHandleManager<CStaticBomb>()->init(MAX_ENTITIES);
	getHandleManager<CMagneticBomb>()->init(MAX_ENTITIES);

	//colliders
	getHandleManager<sphereCollider>()->init(MAX_ENTITIES);
	getHandleManager<boxCollider>()->init(MAX_ENTITIES);

	SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(TCompLife, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(player_controller, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller_speedy, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller_mole, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(ai_speedy, TMsgSetPlayer, onSetPlayer);
	SUBSCRIBE(ai_scientific, TMsgBeaconToRemove, onRemoveBeacon);			//Beacon to remove
	SUBSCRIBE(ai_scientific, TMsgBeaconEmpty, onEmptyBeacon);				//Beacon empty
	SUBSCRIBE(ai_scientific, TMsgWBEmpty, onEmptyWB);					//Workbench empty
	SUBSCRIBE(TCompRenderStaticMesh, TMsgEntityCreated, onCreate);

	SUBSCRIBE(beacon_controller, TMsgBeaconBusy, onPlayerAction);
	SUBSCRIBE(ai_scientific, TMsgBeaconTakenByPlayer, onTakenBeacon);
	SUBSCRIBE(ai_scientific, TMsgWBTakenByPlayer, onTakenWB);
	SUBSCRIBE(ai_scientific, TMsgStaticBomb, onStaticBomb);

	//Posesiones Mensajes
	//..Cientifico
	SUBSCRIBE(ai_scientific, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(ai_scientific, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_cientifico, TMsgControllerSetEnable, onSetEnable);
	//..Speedy
	SUBSCRIBE(ai_speedy, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(ai_speedy, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_speedy, TMsgControllerSetEnable, onSetEnable);
	//..Mole
	SUBSCRIBE(ai_mole, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(ai_mole, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_mole, TMsgControllerSetEnable, onSetEnable);

	//..PJ Principal
	SUBSCRIBE(player_controller, TMsgPossessionLeave, onLeaveFromPossession);
	
	//Damage
	SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(player_controller, TMsgDamage, onDamage);

	//colliders
	SUBSCRIBE(sphereCollider, TMsgEntityCreated, onCreate);
	SUBSCRIBE(boxCollider, TMsgEntityCreated, onCreate);

	CEntityParser ep;
	bool is_ok = ep.xmlParseFile("data/scenes/scene00.xml");
	assert(is_ok);

	TTagID tagIDplayer = getID("player");
	TTagID tagIDbox = getID("box");
	TTagID tagIDboxleave = getID("box_leavepoint");
	TTagID tagIDwall = getID("breakable_wall");
	TTagID tagIDminus = getID("minus_wall");
	TTagID tagIDplus = getID("plus_wall");

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

	SBB::postHandlesVector("wptsBoxes", tags_manager.getHandlesByTag(tagIDbox));
	SBB::postHandlesVector("wptsBreakableWall", tags_manager.getHandlesByTag(tagIDwall));
	SBB::postHandlesVector("wptsBoxLeavePoint", tags_manager.getHandlesByTag(tagIDboxleave));
	SBB::postHandlesVector("wptsMinusPoint", tags_manager.getHandlesByTag(tagIDminus));
	SBB::postHandlesVector("wptsPlusPoint", tags_manager.getHandlesByTag(tagIDplus));

	getHandleManager<player_controller>()->onAll(&player_controller::Init);
	getHandleManager<player_controller_speedy>()->onAll(&player_controller_speedy::Init);
	getHandleManager<player_controller_cientifico>()->onAll(&player_controller_cientifico::Init);
	getHandleManager<player_controller_mole>()->onAll(&player_controller_mole::Init);

	getHandleManager<ai_guard>()->onAll(&ai_guard::Init);
	getHandleManager<ai_mole>()->onAll(&ai_mole::Init);
	getHandleManager<ai_scientific>()->onAll(&ai_scientific::Init);
	getHandleManager<ai_speedy>()->onAll(&ai_speedy::Init);
	getHandleManager<beacon_controller>()->onAll(&beacon_controller::Init);
	getHandleManager<workbench_controller>()->onAll(&workbench_controller::Init);

	return true;
}

void CEntitiesModule::stop() {
	// Stop input
	input.Shutdown();
}

void CEntitiesModule::update(float dt) {
	// May need here a switch to update wich player controller takes the action - possession rulez
	getHandleManager<player_controller>()->updateAll(dt);
	getHandleManager<player_controller_speedy>()->updateAll(dt);
	getHandleManager<player_controller_mole>()->updateAll(dt);
	getHandleManager<player_controller_cientifico>()->updateAll(dt);

	getHandleManager<TCompController3rdPerson>()->updateAll(dt);
	getHandleManager<TCompCamera>()->updateAll(dt);

	getHandleManager<ai_mole>()->updateAll(dt);
	getHandleManager<ai_guard>()->updateAll(dt);
	getHandleManager<ai_scientific>()->updateAll(dt);
	getHandleManager<beacon_controller>()->updateAll(dt);
	getHandleManager<workbench_controller>()->updateAll(dt);
	getHandleManager<ai_speedy>()->updateAll(dt);

	getHandleManager<CStaticBomb>()->updateAll(dt);
	getHandleManager<CMagneticBomb>()->updateAll(dt);
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