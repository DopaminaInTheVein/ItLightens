#include "mcv_platform.h"
#include "app_modules/entities.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "handle/handle_manager.h"
#include "handle/msgs.h"
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
#include "recast/navmesh.h"
#include "recast/navmesh_query.h"
#include <vector>
#include <thread>
#include <future>
#include <fstream>

DECL_OBJ_MANAGER("entity", CEntity);		//need to be first

DECL_OBJ_MANAGER("name", TCompName);
DECL_OBJ_MANAGER("transform", TCompTransform);
DECL_OBJ_MANAGER("snoozer", TCompSnoozer);
DECL_OBJ_MANAGER("camera", TCompCamera);
DECL_OBJ_MANAGER("controller_3rd_person", TCompController3rdPerson);
DECL_OBJ_MANAGER("render_static_mesh", TCompRenderStaticMesh);
DECL_OBJ_MANAGER("cientifico", ai_scientific);
DECL_OBJ_MANAGER("beacon", beacon_controller);
DECL_OBJ_MANAGER("workbench", workbench_controller);
DECL_OBJ_MANAGER("magnet_door", magnet_door);
DECL_OBJ_MANAGER("elevator", elevator);
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
DECL_OBJ_MANAGER("bone_tracker", TCompBoneTracker);
DECL_OBJ_MANAGER("abs_aabb", TCompAbsAABB);
DECL_OBJ_MANAGER("local_aabb", TCompLocalAABB);
DECL_OBJ_MANAGER("culling", TCompCulling);
DECL_OBJ_MANAGER("light_dir", TCompLightDir);
DECL_OBJ_MANAGER("light_dir_shadows", TCompLightDirShadows);
DECL_OBJ_MANAGER("tags", TCompTags);
DECL_OBJ_MANAGER("light_point", TCompLightPoint);
DECL_OBJ_MANAGER("light_fadable", TCompLightFadable);

DECL_OBJ_MANAGER("platform", TCompPlatform);
DECL_OBJ_MANAGER("drone", TCompDrone);
DECL_OBJ_MANAGER("box", TCompBox);

//Physics
DECL_OBJ_MANAGER("rigidbody", TCompPhysics);
DECL_OBJ_MANAGER("character_controller", TCompCharacterController);

//prefabs
DECL_OBJ_MANAGER("magnetic_bomb", CMagneticBomb);
DECL_OBJ_MANAGER("static_bomb", CStaticBomb);
DECL_OBJ_MANAGER("polarized", TCompPolarized);

DECL_OBJ_MANAGER("victory_point", TVictoryPoint);
DECL_OBJ_MANAGER("trigger_lua", TTriggerLua);

//Tracker
DECL_OBJ_MANAGER("tracker", TCompTracker);

DECL_OBJ_MANAGER("box_spawn", TCompBoxSpawner);
DECL_OBJ_MANAGER("box_destructor", TCompBoxDestructor);

DECL_OBJ_MANAGER("guided_camera", TCompGuidedCamera);

DECL_OBJ_MANAGER("helper_arrow", LogicHelperArrow);

CCamera * camera;

// The global dict of all msgs
MMsgSubscriptions msg_subscriptions;
TMsgID generateUniqueMsgID() {
	static TMsgID next_msg_id = 1;
	return next_msg_id++;
}

bool CEntitiesModule::start() {
	SBB::init();
	Damage::init();

	getHandleManager<CEntity>()->init(MAX_ENTITIES);

	getHandleManager<TVictoryPoint>()->init(20);
	getHandleManager<TTriggerLua>()->init(100);
	//	getHandleManager<TCompHierarchy>()->init(nmax);
	getHandleManager<TCompAbsAABB>()->init(MAX_ENTITIES);
	getHandleManager<TCompLocalAABB>()->init(MAX_ENTITIES);
	getHandleManager<TCompCulling>()->init(8);
	getHandleManager<TCompLightDir>()->init(8);
	getHandleManager<TCompLightDirShadows>()->init(8);
	getHandleManager<player_controller>()->init(8);
	getHandleManager<player_controller_speedy>()->init(8);
	getHandleManager<player_controller_mole>()->init(8);
	getHandleManager<player_controller_cientifico>()->init(8);
	getHandleManager<TCompRenderStaticMesh>()->init(MAX_ENTITIES);
	getHandleManager<TCompSkeleton>()->init(MAX_ENTITIES);
	getHandleManager<TCompName>()->init(MAX_ENTITIES);
	getHandleManager<TCompTransform>()->init(MAX_ENTITIES);
	getHandleManager<TCompSnoozer>()->init(MAX_ENTITIES);
	getHandleManager<TCompRenderStaticMesh>()->init(MAX_ENTITIES);
	getHandleManager<TCompCamera>()->init(4);
	getHandleManager<TCompController3rdPerson>()->init(4);
	getHandleManager<TCompLife>()->init(MAX_ENTITIES);
	getHandleManager<TCompWire>()->init(10);
	getHandleManager<TCompGenerator>()->init(10);
	getHandleManager<TCompPolarized>()->init(MAX_ENTITIES);
	getHandleManager<TCompBoneTracker>()->init(MAX_ENTITIES);
	getHandleManager<TCompTags>()->init(MAX_ENTITIES);
	getHandleManager<TCompBox>()->init(MAX_ENTITIES);
	getHandleManager<TCompGuidedCamera>()->init(16);
	getHandleManager<LogicHelperArrow>()->init(4);
	//lights
	getHandleManager<TCompLightDir>()->init(4);
	getHandleManager<TCompLightFadable>()->init(4);
	getHandleManager<TCompLightPoint>()->init(32);

	getHandleManager<bt_guard>()->init(MAX_ENTITIES);
	getHandleManager<bt_mole>()->init(MAX_ENTITIES);
	getHandleManager<bt_speedy>()->init(MAX_ENTITIES);
	getHandleManager<ai_scientific>()->init(MAX_ENTITIES);
	getHandleManager<beacon_controller>()->init(MAX_ENTITIES);
	getHandleManager<workbench_controller>()->init(MAX_ENTITIES);
	getHandleManager<water_controller>()->init(MAX_ENTITIES);
	getHandleManager<magnet_door>()->init(MAX_ENTITIES);
	getHandleManager<elevator>()->init(4);

	getHandleManager<TCompPlatform>()->init(MAX_ENTITIES);
	getHandleManager<TCompDrone>()->init(MAX_ENTITIES);

	getHandleManager<CStaticBomb>()->init(MAX_ENTITIES);
	getHandleManager<CMagneticBomb>()->init(MAX_ENTITIES);

	getHandleManager<TCompBoxSpawner>()->init(MAX_ENTITIES);
	getHandleManager<TCompBoxDestructor>()->init(MAX_ENTITIES);

	//getHandleManager<TCompTriggerStandar>()->init(MAX_ENTITIES);

	//colliders
	getHandleManager<TCompPhysics>()->init(MAX_ENTITIES);
	getHandleManager<TCompCharacterController>()->init(MAX_ENTITIES);

	//Trackers
	getHandleManager<TCompTracker>()->init(100);

	//SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(TCompSnoozer, TMsgPreload, onPreload);
	SUBSCRIBE(TCompSnoozer, TMsgAwake, onAwake);
	SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompPhysics, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompPlatform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompDrone, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTags, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompCharacterController, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(TCompController3rdPerson, TMsgEntityCreated, onCreate);
	SUBSCRIBE(LogicHelperArrow, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(player_controller, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller, TMsgDamageSpecific, onSetDamage);
	SUBSCRIBE(player_controller_speedy, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller_mole, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(ai_speedy, TMsgSetPlayer, onSetPlayer);
	SUBSCRIBE(bt_speedy, TMsgSetPlayer, onSetPlayer);
	SUBSCRIBE(ai_scientific, TMsgBeaconToRemove, onRemoveBeacon);			//Beacon to remove
	SUBSCRIBE(ai_scientific, TMsgBeaconEmpty, onEmptyBeacon);				//Beacon empty
	SUBSCRIBE(ai_scientific, TMsgWBEmpty, onEmptyWB);						//Workbench empty
	SUBSCRIBE(TCompRenderStaticMesh, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompRenderStaticMesh, TMsgGetLocalAABB, onGetLocalAABB);
	//  SUBSCRIBE(TCompHierarchy, TMsgEntityGroupCreated, onGroupCreated);
	SUBSCRIBE(TCompBoneTracker, TMsgEntityGroupCreated, onGroupCreated);
	SUBSCRIBE(TCompAbsAABB, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompLocalAABB, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTags, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTags, TMsgAddTag, onTagAdded);

	//Trackers
	SUBSCRIBE(TCompTracker, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTracker, TMsgFollow, setFollower);
	SUBSCRIBE(TCompCamera, TMsgGuidedCamera, onGuidedCamera);
	SUBSCRIBE(TCompGuidedCamera, TMsgGuidedCamera, onGuidedCamera);

	SUBSCRIBE(beacon_controller, TMsgBeaconBusy, onPlayerAction);
	SUBSCRIBE(ai_scientific, TMsgBeaconTakenByPlayer, onTakenBeacon);
	SUBSCRIBE(ai_scientific, TMsgWBTakenByPlayer, onTakenWB);
	SUBSCRIBE(magnet_door, TMsgSetLocked, onSetLocked);
	SUBSCRIBE(magnet_door, TMsgSetPolarity, onSetPolarity);
	SUBSCRIBE(magnet_door, TMsgEntityCreated, onCreate);
	SUBSCRIBE(elevator, TMsgActivate, onElevatorAction);
	SUBSCRIBE(elevator, TMsgEntityCreated, onCreate);

	//box
	SUBSCRIBE(TCompBox, TMsgLeaveBox, onUnLeaveBox);

	//water
	SUBSCRIBE(water_controller, TMsgEntityCreated, onCreate);

	//bombs
	SUBSCRIBE(ai_scientific, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_guard, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_mole, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_speedy, TMsgStaticBomb, onStaticBomb);
	SUBSCRIBE(bt_guard, TMsgMagneticBomb, onMagneticBomb);
	SUBSCRIBE(bt_guard, TMsgNoise, noise);
	SUBSCRIBE(bt_guard, TMsgOverCharge, onOverCharged);
	SUBSCRIBE(bt_guard, TMsgBoxHit, onBoxHit);

	//WIRES
	SUBSCRIBE(TCompWire, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgWirePass, onWirePass);

	//generator
	SUBSCRIBE(TCompGenerator, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgCanRec, onCanRec);
	SUBSCRIBE(TCompGenerator, TMsgTriggerIn, onTriggerEnterCall);
	SUBSCRIBE(TCompGenerator, TMsgTriggerOut, onTriggerExitCall);

	SUBSCRIBE(TCompBoxDestructor, TMsgTriggerIn, onTriggerEnterCall);

	//triggers
	//SUBSCRIBE(TCompTriggerStandar, TMsgTriggerIn, onTriggerEnterCall);
	//SUBSCRIBE(TCompTriggerStandar, TMsgTriggerOut, onTriggerExitCall);

	//victory point
	SUBSCRIBE(TVictoryPoint, TMsgTriggerIn, onTriggerEnterCall);

	//trigger_lua
	SUBSCRIBE(TTriggerLua, TMsgTriggerIn, onTriggerEnterCall);
	SUBSCRIBE(TTriggerLua, TMsgTriggerOut, onTriggerExitCall);
	SUBSCRIBE(TTriggerLua, TMsgSetActivable, onSetActionable);

	//Animations
	SUBSCRIBE(TCompSkeleton, TMsgSetAnim, onSetAnim);

	//polarized
	SUBSCRIBE(TCompPolarized, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgPolarize, onPolarize);
	SUBSCRIBE(player_controller, TMsgGetPolarity, onGetPolarity);
	SUBSCRIBE(TCompPolarized, TMsgPlayerPolarize, onPolarize);

	//Posesiones Mensajes
	//..Cientifico
	SUBSCRIBE(ai_scientific, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(ai_scientific, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_cientifico, TMsgControllerSetEnable, onSetEnable);
	SUBSCRIBE(player_controller_cientifico, TMsgGetWhoAmI, onGetWhoAmI);
	//..Speedy
	SUBSCRIBE(bt_speedy, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(bt_speedy, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_speedy, TMsgControllerSetEnable, onSetEnable);
	//SUBSCRIBE(player_controller_speedy, TMsgGetWhoAmI, onGetWhoAmI);
	//..Mole
	SUBSCRIBE(bt_mole, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(bt_mole, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_mole, TMsgControllerSetEnable, onSetEnable);
	SUBSCRIBE(player_controller_mole, TMsgGetWhoAmI, onGetWhoAmI);

	//..PJ Principal
	SUBSCRIBE(player_controller, TMsgPossessionLeave, onLeaveFromPossession);
	SUBSCRIBE(player_controller, TMsgGetWhoAmI, onGetWhoAmI);

	//Dead
	//anything for now
	/*SUBSCRIBE(player_controller, TMsgDie, onDie);
	SUBSCRIBE(player_controller_cientifico, TMsgDie, onDie);
	SUBSCRIBE(player_controller_speedy, TMsgDie, onDie);
	SUBSCRIBE(player_controller_mole, TMsgDie, onDie);*/

	//Damage
	SUBSCRIBE(TCompLife, TMsgEntityCreated, onCreate);		//init damage scales
	SUBSCRIBE(TCompLife, TMsgDamageSave, onSetSaveDamage);
	SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(TCompLife, TMsgSetDamage, onReciveDamage);
	SUBSCRIBE(TCompLife, TMsgStopDamage, onStopDamage);
	SUBSCRIBE(player_controller_cientifico, TMsgUnpossesDamage, onForceUnPosses);
	SUBSCRIBE(player_controller_speedy, TMsgUnpossesDamage, onForceUnPosses);
	SUBSCRIBE(player_controller_mole, TMsgUnpossesDamage, onForceUnPosses);

	SUBSCRIBE(TCompCamera, TMsgGetCullingViewProj, onGetViewProj);

	//Control
	SUBSCRIBE(player_controller, TMsgSetControllable, onSetControllable);
	SUBSCRIBE(player_controller_cientifico, TMsgSetControllable, onSetControllable);
	SUBSCRIBE(player_controller_mole, TMsgSetControllable, onSetControllable);
	SUBSCRIBE(player_controller_speedy, TMsgSetControllable, onSetControllable);

	initLevel("room_one");

	return true;
}

void CEntitiesModule::initLevel(string level) {
	CApp &app = CApp::get();
	std::string file_options = app.file_options_json;
	map<std::string, std::string> fields = readIniAtrDataStr(file_options, "scenes");

	//sala = "tiling";
	sala = fields[level];
	//sala = "drones";
	//sala = "boxes";
	//sala = "milestone2";
	//sala = "scene_milestone_1";
	//sala = "scene_test_recast";
	//sala = "pruebaExportador";
	//sala = "scene_basic_lights";
	//sala = "test_simple";
	//sala = "test_guard";
	//sala = "test_pol";
	//sala = "test_guard";
	//sala = "test_anim";
	//sala = "test_column_navmesh";

	SBB::postSala(sala);
	salaloc = "data/navmeshes/" + sala + ".data";

	CEntityParser ep;

	dbg("Loading scene... (%d entities)\n", size());
	bool is_ok = ep.xmlParseFile("data/scenes/" + sala + ".xml");
	assert(is_ok);
	dbg("Scene Loaded! (%d entities)\n", size());

	// GENERATE NAVMESH
	collisionables = ep.getCollisionables();
	SBB::postHandlesVector("collisionables", collisionables);
	CNavmesh nav;
	nav.m_input.clearInput();
	for (CHandle han : collisionables) {
		CEntity * e = han;
		if (e) {
			TCompPhysics * p = e->get<TCompPhysics>();
			const PxBounds3 bounds = p->getActor()->getWorldBounds();
			VEC3 min, max;
			min.x = bounds.minimum.x;
			min.y = bounds.minimum.y;
			min.z = bounds.minimum.z;
			max.x = bounds.maximum.x;
			max.y = bounds.maximum.y;
			max.z = bounds.maximum.z;
			nav.m_input.addInput(min, max);
			/*
			PxGeometryHolder geo = p->getShape()->getGeometry();
			PxTriangleMeshGeometry mesh = geo.triangleMesh();
			nav.m_input.addInput(mesh);
			*/
		}
	}
	nav.m_input.computeBoundaries();
	SBB::postNavmesh(nav);
	std::ifstream is(salaloc.c_str());
	bool recalc = !is.is_open();
	is.close();
	SBB::postBool(sala, false);
	if (!recalc) {
		// restore the navmesh from the archive
		//std::thread thre(&CEntitiesModule::readNavmesh, this);
		//thre.detach();
	}
	else {
		// make mesh on a separate thread
		//std::thread thre(&CEntitiesModule::recalcNavmesh, this);
		//thre.detach();
	}
	TTagID tagIDcamera = getID("camera_main");
	TTagID tagIDwall = getID("breakable_wall");
	TTagID tagIDminus = getID("minus_wall");
	TTagID tagIDplus = getID("plus_wall");
	TTagID tagIDrec = getID("recover_point");

	// Camara del player
	CHandle camera = tags_manager.getFirstHavingTag("camera_main");
	CEntity * camera_e = camera;
	if (!camera_e) {
		//main camera needed
		fatal("main camera needed!!\n");
		assert(false);
	}
	TCompCamera * pcam = camera_e->get<TCompCamera>();

	CHandle t = tags_manager.getFirstHavingTag("player");
	CEntity * target_e = t;

	CHandle helper_arrow = tags_manager.getFirstHavingTag("helper_arrow");
	CEntity * helper_arrow_e = helper_arrow;

	// Set the player in the 3rdPersonController
	if (camera_e && t.isValid()) {
		TMsgSetTarget msg;
		msg.target = t;
		msg.who = PLAYER;
		camera_e->sendMsg(msg);	//set camera
		if (helper_arrow.isValid()) helper_arrow_e->sendMsg(msg);

		TMsgSetCamera msg_camera;
		msg_camera.camera = camera;
		target_e->sendMsg(msg_camera); //set target camera
	}
	//}
	TTagID generators = getID("generator");
	VHandles generatorsHandles = tags_manager.getHandlesByTag(generators);
	SBB::postHandlesVector("generatorsHandles", generatorsHandles);

	// Set the player in the Speedy AIs
	TTagID tagIDSpeedy = getID("AI_speedy");
	VHandles speedyHandles = tags_manager.getHandlesByTag(tagIDSpeedy);

	for (CHandle speedyHandle : speedyHandles) {
		CEntity * speedy_e = speedyHandle;
		TMsgSetPlayer msg_player;
		msg_player.player = t;
		speedy_e->sendMsg(msg_player);
	}

	SBB::postHandlesVector("wptsBreakableWall", tags_manager.getHandlesByTag(tagIDwall));
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
	//getHandleManager<water_controller>()->onAll(&water_controller::Init); --> Se hace en el onCreated!
	getHandleManager<beacon_controller>()->onAll(&beacon_controller::Init);
	getHandleManager<workbench_controller>()->onAll(&workbench_controller::Init);
	getHandleManager<TCompGenerator>()->onAll(&TCompGenerator::init);
	getHandleManager<TCompWire>()->onAll(&TCompWire::init);
	getHandleManager<TCompPolarized>()->onAll(&TCompPolarized::init);
	getHandleManager<TCompBox>()->onAll(&TCompBox::init);
}

void CEntitiesModule::destroyAllEntities() {
	getHandleManager<CEntity>()->each([](CEntity * e) {
		CHandle(e).destroy();
	});
}

void CEntitiesModule::destroyRandomEntity(float percent) {
	float currentSize = (float)size();
	getHandleManager<CEntity>()->each([currentSize, percent](CEntity * e) {
		int r = rand() % (int)currentSize;
		if ((float)r / currentSize < percent) {
			dbg("Elimino entidad %s\n", e->getName());
			CHandle(e).destroy();
		}
	});
}

void CEntitiesModule::stop() {
}

void CEntitiesModule::update(float dt) {
	//Test
	//if (io->keys['Z'].becomesPressed()) {
	   // for (int i = 0; i < 1; i++) {
		  //  if (size() == 0) break;
		  //  destroyRandomEntity(0.1f);
	   // }
	//}

	static float ia_wait = 0.0f;
	ia_wait += getDeltaTime();

	//physx objects
	getHandleManager<TCompCharacterController>()->updateAll(dt);
	getHandleManager<TCompPhysics>()->updateAll(dt);

	getHandleManager<TCompLightDir>()->updateAll(dt);
	getHandleManager<TCompLightDirShadows>()->updateAll(dt);
	getHandleManager<TCompLocalAABB>()->onAll(&TCompLocalAABB::updateAbs);
	getHandleManager<TCompCulling>()->onAll(&TCompCulling::update);

	if (GameController->GetGameState() == CGameController::STOPPED || GameController->GetGameState() == CGameController::STOPPED_INTRO) {
		if (!GameController->IsCinematic()) {
			getHandleManager<TCompController3rdPerson>()->updateAll(dt);
		}
		getHandleManager<TCompCamera>()->updateAll(dt);
	}

	if (GameController->GetGameState() == CGameController::RUNNING) {
		// May need here a switch to update wich player controller takes the action - possession rulez
		if (!GameController->IsCinematic()) {
			getHandleManager<player_controller>()->updateAll(dt);
			getHandleManager<player_controller_speedy>()->updateAll(dt);
			getHandleManager<player_controller_mole>()->updateAll(dt);
			getHandleManager<player_controller_cientifico>()->updateAll(dt);
			getHandleManager<TCompController3rdPerson>()->updateAll(dt);
			getHandleManager<LogicHelperArrow>()->updateAll(dt);
		}

		getHandleManager<TCompCamera>()->updateAll(dt);
		getHandleManager<TCompLightDir>()->updateAll(dt);

		if (use_parallel)
			getHandleManager<TCompSkeleton>()->updateAllInParallel(dt);
		else
			getHandleManager<TCompSkeleton>()->updateAll(dt);

		getHandleManager<TCompBoneTracker>()->updateAll(dt);

		if (SBB::readBool(sala) && ia_wait > 1.0f) {
			getHandleManager<bt_guard>()->updateAll(dt);
			getHandleManager<bt_mole>()->updateAll(dt);
			getHandleManager<ai_scientific>()->updateAll(dt);
			getHandleManager<beacon_controller>()->updateAll(dt);
			getHandleManager<workbench_controller>()->updateAll(dt);
			getHandleManager<bt_speedy>()->updateAll(dt);
			getHandleManager<water_controller>()->updateAll(dt);
		}
		getHandleManager<CStaticBomb>()->updateAll(dt);
		getHandleManager<CMagneticBomb>()->updateAll(dt);

		getHandleManager<TCompWire>()->updateAll(dt);
		getHandleManager<TCompGenerator>()->updateAll(dt);
		getHandleManager<TCompPolarized>()->updateAll(dt);

		getHandleManager<TCompLife>()->updateAll(dt);

		getHandleManager<TCompPlatform>()->updateAll(dt);
		getHandleManager<TCompDrone>()->updateAll(dt);
		getHandleManager<TCompBox>()->updateAll(dt);
		getHandleManager<magnet_door>()->updateAll(dt);
		getHandleManager<elevator>()->updateAll(dt);
		//getHandleManager<TCompTracker>()->updateAll(dt);

		getHandleManager<TCompBoxSpawner>()->updateAll(dt);
		getHandleManager<TCompBoxDestructor>()->updateAll(dt);

		getHandleManager<TCompLightPoint>()->updateAll(dt);
		getHandleManager<TCompLightFadable>()->updateAll(dt);

		//Triggers
		getHandleManager<TTriggerLua>()->updateAll(dt);

		SBB::update(dt);
	}
	// In this mode, only the animation of the player is updated
	else if (GameController->GetGameState() == CGameController::STOPPED_INTRO) {
		VHandles targets = tags_manager.getHandlesByTag(getID("player"));
		CHandle player_handle = targets[targets.size() - 1];
		CEntity* player_entity = player_handle;

		TCompSkeleton* player_skeleton = player_entity->get<TCompSkeleton>();
		player_skeleton->update(dt);
	}
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
	getHandleManager<TCompLightDir>()->onAll(&TCompLightDir::render);

	getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::render);
	getHandleManager<TCompAbsAABB>()->onAll(&TCompAbsAABB::render);
	getHandleManager<TCompLocalAABB>()->onAll(&TCompLocalAABB::render);
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

	if (ImGui::TreeNode("Entities by Tag...")) {
		tags_manager.renderInMenu();
		// Show all defined tags
		ImGui::TreePop();
	}
	ImGui::End();
}

void CEntitiesModule::recalcNavmesh() {
	// GENERATE NAVMESH
	CNavmesh nav = SBB::readNavmesh();
	nav.build(salaloc);
	SBB::postNavmesh(nav);
	SBB::postBool(sala, true);
}

void CEntitiesModule::readNavmesh() {
	// GENERATE NAVMESH
	CNavmesh nav = SBB::readNavmesh();
	bool recalc = !nav.reload(salaloc);
	if (recalc) {
		recalcNavmesh();
	}
	else {
		SBB::postNavmesh(nav);
		SBB::postBool(sala, true);
	}
}

void CEntitiesModule::fixedUpdate(float elapsed)
{
	getHandleManager<TCompDrone>()->fixedUpdateAll(elapsed);
}

int CEntitiesModule::size() {
	return getHandleManager<CEntity>()->size();
}