#include "mcv_platform.h"
#include "app_modules/entities.h"
#include "components/components.h"
#include "components/entity_parser.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"
#include "components/comp_workstation.h"
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
#include "logic/bt_scientist.h"
#include "windows/app.h"
#include "utils/utils.h"
#include "recast/navmesh.h"
#include "recast/navmesh_query.h"
#include <vector>
#include <future>
#include <fstream>

DECL_OBJ_MANAGER("entity", CEntity);		//need to be first

DECL_OBJ_MANAGER("name", TCompName);
DECL_OBJ_MANAGER("room", TCompRoom);
DECL_OBJ_MANAGER("transform", TCompTransform);
DECL_OBJ_MANAGER("snoozer", TCompSnoozer);
DECL_OBJ_MANAGER("camera", TCompCamera);
DECL_OBJ_MANAGER("camera_main", TCompCameraMain);
DECL_OBJ_MANAGER("controller_3rd_person", TCompController3rdPerson);
DECL_OBJ_MANAGER("render_static_mesh", TCompRenderStaticMesh);
DECL_OBJ_MANAGER("bt_scientist", bt_scientist);
DECL_OBJ_MANAGER("ai_cam", ai_cam);
DECL_OBJ_MANAGER("workbench", workbench_controller);
DECL_OBJ_MANAGER("hierarchy", TCompHierarchy);
DECL_OBJ_MANAGER("magnet_door", magnet_door);
DECL_OBJ_MANAGER("elevator", elevator);
DECL_OBJ_MANAGER("bt_guard", bt_guard);
DECL_OBJ_MANAGER("bt_mole", bt_mole);
DECL_OBJ_MANAGER("bt_speedy", bt_speedy);
DECL_OBJ_MANAGER("water", water_controller);
DECL_OBJ_MANAGER("player", player_controller);
DECL_OBJ_MANAGER("player_speedy", player_controller_speedy);
DECL_OBJ_MANAGER("player_mole", player_controller_mole);
DECL_OBJ_MANAGER("player_cientifico", player_controller_cientifico);
DECL_OBJ_MANAGER("workbench", workbench);
DECL_OBJ_MANAGER("life", TCompLife);
DECL_OBJ_MANAGER("wire", TCompWire);
DECL_OBJ_MANAGER("generator", TCompGenerator);
DECL_OBJ_MANAGER("room_switcher", TCompRoomSwitch);
DECL_OBJ_MANAGER("limit", TCompRoomLimit);
//Skeletons
DECL_OBJ_MANAGER("skeleton", TCompSkeleton);
DECL_OBJ_MANAGER("skc_player", SkelControllerPlayer);
DECL_OBJ_MANAGER("skc_guard", SkelControllerGuard);
DECL_OBJ_MANAGER("skc_scientist", SkelControllerScientist);
DECL_OBJ_MANAGER("skc_mole", SkelControllerMole);
DECL_OBJ_MANAGER("bone_tracker", TCompBoneTracker);
DECL_OBJ_MANAGER("skeleton_ik", TCompSkeletonIK);
DECL_OBJ_MANAGER("skeleton_lookat", TCompSkeletonLookAt);

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
DECL_OBJ_MANAGER("pila", TCompPila);
DECL_OBJ_MANAGER("pila_container", TCompPilaContainer);
DECL_OBJ_MANAGER("workstation", TCompWorkstation);

//Physics
DECL_OBJ_MANAGER("rigidbody", TCompPhysics);
DECL_OBJ_MANAGER("character_controller", TCompCharacterController);

//prefabs
DECL_OBJ_MANAGER("throw_bomb", CThrowBomb);
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

//particles
DECL_OBJ_MANAGER("particles_system", CParticleSystem);

/* HELPERS */
DECL_OBJ_MANAGER("helper_arrow", LogicHelperArrow);
DECL_OBJ_MANAGER("helper_message", TCompFadingMessage);
DECL_OBJ_MANAGER("character_globe", TCompFadingGlobe);

//fx
DECL_OBJ_MANAGER("FX_fade_screen", TCompFadeScreen);
DECL_OBJ_MANAGER("render_glow", TCompRenderGlow);

using namespace std;

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
	getHandleManager<TCompCulling>()->init(40);
	getHandleManager<TCompLightDir>()->init(8);
	getHandleManager<TCompLightDirShadows>()->init(MAX_ENTITIES);
	getHandleManager<player_controller>()->init(8);
	getHandleManager<player_controller_speedy>()->init(8);
	getHandleManager<player_controller_mole>()->init(16);
	getHandleManager<player_controller_cientifico>()->init(8);
	getHandleManager<TCompRenderStaticMesh>()->init(MAX_ENTITIES);
	getHandleManager<TCompSkeleton>()->init(MAX_ENTITIES);
	getHandleManager<TCompSkeletonIK>()->init(MAX_ENTITIES);
	getHandleManager<TCompSkeletonLookAt>()->init(MAX_ENTITIES);
	getHandleManager<SkelControllerPlayer>()->init(MAX_ENTITIES);
	getHandleManager<SkelControllerGuard>()->init(MAX_ENTITIES);
	getHandleManager<SkelControllerScientist>()->init(MAX_ENTITIES);
	getHandleManager<SkelControllerMole>()->init(MAX_ENTITIES);
	getHandleManager<TCompName>()->init(MAX_ENTITIES);
	getHandleManager<TCompRoom>()->init(MAX_ENTITIES);
	getHandleManager<TCompRoomSwitch>()->init(4);
	getHandleManager<TCompRoomLimit>()->init(MAX_ENTITIES);
	getHandleManager<TCompTransform>()->init(MAX_ENTITIES);
	getHandleManager<TCompSnoozer>()->init(MAX_ENTITIES);
	getHandleManager<TCompRenderStaticMesh>()->init(MAX_ENTITIES);
	getHandleManager<TCompCamera>()->init(32);
	getHandleManager<TCompCameraMain>()->init(4);
	getHandleManager<TCompController3rdPerson>()->init(4);
	getHandleManager<TCompLife>()->init(MAX_ENTITIES);
	getHandleManager<TCompWire>()->init(10);
	getHandleManager<TCompGenerator>()->init(10);
	getHandleManager<TCompPolarized>()->init(MAX_ENTITIES);
	getHandleManager<TCompBoneTracker>()->init(MAX_ENTITIES);
	getHandleManager<TCompTags>()->init(MAX_ENTITIES);
	getHandleManager<TCompBox>()->init(MAX_ENTITIES);
	getHandleManager<TCompPila>()->init(MAX_ENTITIES);
	getHandleManager<TCompPilaContainer>()->init(MAX_ENTITIES);
	getHandleManager<TCompWorkstation>()->init(MAX_ENTITIES);
	getHandleManager<TCompGuidedCamera>()->init(16);
	//helpers
	getHandleManager<TCompFadingMessage>()->init(32);
	getHandleManager<TCompFadingGlobe>()->init(32);
	getHandleManager<LogicHelperArrow>()->init(4);
	//lights
	getHandleManager<TCompLightDir>()->init(MAX_ENTITIES);
	getHandleManager<TCompLightFadable>()->init(4);
	getHandleManager<TCompLightPoint>()->init(MAX_ENTITIES);

	getHandleManager<bt_guard>()->init(MAX_ENTITIES);
	getHandleManager<bt_mole>()->init(MAX_ENTITIES);
	getHandleManager<bt_speedy>()->init(MAX_ENTITIES);
	getHandleManager<bt_scientist>()->init(MAX_ENTITIES);
	getHandleManager<ai_cam>()->init(MAX_ENTITIES);
	getHandleManager<workbench_controller>()->init(MAX_ENTITIES);
	getHandleManager<workbench>()->init(MAX_ENTITIES);
	getHandleManager<water_controller>()->init(MAX_ENTITIES);
	getHandleManager<magnet_door>()->init(MAX_ENTITIES);
	getHandleManager<elevator>()->init(4);
	getHandleManager<TCompRenderGlow>()->init(4);

	getHandleManager<TCompPlatform>()->init(MAX_ENTITIES);
	getHandleManager<TCompDrone>()->init(MAX_ENTITIES);

	getHandleManager<CStaticBomb>()->init(MAX_ENTITIES);
	getHandleManager<CMagneticBomb>()->init(MAX_ENTITIES);
	getHandleManager<CThrowBomb>()->init(MAX_ENTITIES);

	getHandleManager<TCompBoxSpawner>()->init(MAX_ENTITIES);
	getHandleManager<TCompBoxDestructor>()->init(MAX_ENTITIES);

	//getHandleManager<TCompTriggerStandar>()->init(MAX_ENTITIES);

	//colliders
	getHandleManager<TCompPhysics>()->init(MAX_ENTITIES);
	getHandleManager<TCompCharacterController>()->init(MAX_ENTITIES);

	//Trackers
	getHandleManager<TCompTracker>()->init(100);

	//particles
	getHandleManager<CParticleSystem>()->init(MAX_ENTITIES);

	//fx
	getHandleManager<TCompFadeScreen>()->init(4);

	//SUBSCRIBE(TCompLife, TMsgDamage, onDamage);
	SUBSCRIBE(TCompSnoozer, TMsgPreload, onPreload);
	SUBSCRIBE(TCompSnoozer, TMsgAwake, onAwake);
	SUBSCRIBE(TCompTransform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompPhysics, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompPlatform, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompDrone, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompDrone, TMsgActivate, onRecharge);
	SUBSCRIBE(TCompPila, TMsgIsCharged, isCharged);
	SUBSCRIBE(TCompPila, TMsgActivate, onRecharge);
	SUBSCRIBE(TCompPilaContainer, TMsgSetCharged, onRecharge);
	SUBSCRIBE(TCompDrone, TMsgRepair, onRepair);
	SUBSCRIBE(TCompTags, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompPila, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompPilaContainer, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompCharacterController, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompController3rdPerson, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(TCompController3rdPerson, TMsgEntityCreated, onCreate);
	SUBSCRIBE(LogicHelperArrow, TMsgSetTarget, onSetTarget);
	SUBSCRIBE(player_controller, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller, TMsgDamageSpecific, onSetDamage);
	SUBSCRIBE(player_controller_speedy, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(player_controller_mole, TMsgSetCamera, onSetCamera);
	SUBSCRIBE(bt_speedy, TMsgSetPlayer, onSetPlayer);
	SUBSCRIBE(bt_scientist, TMsgWBEmpty, onEmptyWB);						//Workbench empty
	SUBSCRIBE(TCompRenderStaticMesh, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompRenderStaticMesh, TMsgGetLocalAABB, onGetLocalAABB);
	SUBSCRIBE(TCompCharacterController, TMsgGetLocalAABB, onGetLocalAABB);
	//  SUBSCRIBE(TCompHierarchy, TMsgEntityGroupCreated, onGroupCreated);
	SUBSCRIBE(TCompBoneTracker, TMsgEntityGroupCreated, onGroupCreated);
	SUBSCRIBE(TCompAbsAABB, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompLocalAABB, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTags, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTags, TMsgSetTag, onSetTag);

	//Trackers
	SUBSCRIBE(TCompTracker, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompTracker, TMsgFollow, setFollower);
	SUBSCRIBE(TCompCameraMain, TMsgGuidedCamera, onGuidedCamera);
	SUBSCRIBE(TCompGuidedCamera, TMsgGuidedCamera, onGuidedCamera);
	SUBSCRIBE(TCompBoneTracker, TMsgAttach, onAttach);

	//Skeleton IK
	SUBSCRIBE(TCompSkeletonIK, TMsgSetIKSolver, onSetIKSolver);
	SUBSCRIBE(TCompSkeletonLookAt, TMsgEntityCreated, onCreate);

	//Grab Objects Hit
	SUBSCRIBE(player_controller_mole, TMsgGrabHit, onGrabHit);

	//On contact
	SUBSCRIBE(TCompPila, TMsgContact, onContact);

	SUBSCRIBE(bt_scientist, TMsgWBTakenByPlayer, onTakenWB);
	SUBSCRIBE(magnet_door, TMsgSetLocked, onSetLocked);
	SUBSCRIBE(magnet_door, TMsgSetPolarity, onSetPolarity);
	SUBSCRIBE(magnet_door, TMsgEntityCreated, onCreate);
	SUBSCRIBE(elevator, TMsgActivate, onElevatorAction);
	SUBSCRIBE(elevator, TMsgEntityCreated, onCreate);
	SUBSCRIBE(workbench, TMsgEntityCreated, onCreate);

	//box
	SUBSCRIBE(TCompBox, TMsgLeaveBox, onUnLeaveBox);

	//water
	SUBSCRIBE(water_controller, TMsgEntityCreated, onCreate);

	//bombs
	SUBSCRIBE(CThrowBomb, TMsgActivate, onNextState);
	SUBSCRIBE(CThrowBomb, TMsgThrow, onThrow);
	SUBSCRIBE(CThrowBomb, TMsgEntityCreated, onCreate);
	SUBSCRIBE(bt_scientist, TMsgStaticBomb, onStaticBomb);
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

	// RECHARGING DRONES
	SUBSCRIBE(player_controller, TMsgCanRechargeDrone, onCanRechargeDrone);
	SUBSCRIBE(player_controller, TMsgCanNotRechargeDrone, onCanNotRechargeDrone);
	SUBSCRIBE(TCompGenerator, TMsgTriggerIn, onTriggerEnterCall);
	SUBSCRIBE(TCompGenerator, TMsgTriggerOut, onTriggerExitCall);

	//generator
	SUBSCRIBE(TCompGenerator, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgCanRec, onCanRec);
	SUBSCRIBE(TCompGenerator, TMsgTriggerIn, onTriggerEnterCall);
	SUBSCRIBE(TCompGenerator, TMsgTriggerOut, onTriggerExitCall);

	// room switcher
	SUBSCRIBE(TCompRoomSwitch, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompRoomSwitch, TMsgTriggerIn, onTriggerEnterCall);
	SUBSCRIBE(TCompRoomSwitch, TMsgTriggerOut, onTriggerExitCall);
	// room limiter
	SUBSCRIBE(TCompRoomLimit, TMsgEntityCreated, onCreate);
	SUBSCRIBE(TCompRoomLimit, TMsgTriggerIn, onTriggerEnterCall);
	SUBSCRIBE(TCompRoomLimit, TMsgTriggerOut, onTriggerExitCall);

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
	SUBSCRIBE(TCompPila, TMsgSetActivable, onSetActionable);

	//Animations
	SUBSCRIBE(TCompSkeleton, TMsgSetAnim, onSetAnim);

	//polarized
	SUBSCRIBE(TCompPolarized, TMsgEntityCreated, onCreate);
	SUBSCRIBE(player_controller, TMsgPolarize, onPolarize);
	SUBSCRIBE(player_controller, TMsgGetPolarity, onGetPolarity);
	SUBSCRIBE(TCompPolarized, TMsgPlayerPolarize, onPolarize);

	//Posesiones Mensajes
	//..Cientifico
	SUBSCRIBE(bt_scientist, TMsgAISetPossessed, onSetPossessed);
	SUBSCRIBE(bt_scientist, TMsgAISetStunned, onSetStunned);
	SUBSCRIBE(player_controller_cientifico, TMsgControllerSetEnable, onSetEnable);
	SUBSCRIBE(player_controller_cientifico, TMsgGetWhoAmI, onGetWhoAmI);
	SUBSCRIBE(player_controller_cientifico, TMsgCanRechargeDrone, onCanRepairDrone);
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

	SUBSCRIBE(TCompCameraMain, TMsgGetCullingViewProj, onGetViewProj);
	SUBSCRIBE(TCompLightDirShadows, TMsgGetCullingViewProj, onGetViewProj);
	SUBSCRIBE(TCompCamera, TMsgGetCullingViewProj, onGetViewProj);

	//Control
	SUBSCRIBE(player_controller, TMsgSetControllable, onSetControllable);
	SUBSCRIBE(player_controller_cientifico, TMsgSetControllable, onSetControllable);
	SUBSCRIBE(player_controller_mole, TMsgSetControllable, onSetControllable);
	SUBSCRIBE(player_controller_speedy, TMsgSetControllable, onSetControllable);
	SUBSCRIBE(TCompController3rdPerson, TMsgSetControllable, onSetControllable);

	//Go And Look
	SUBSCRIBE(player_controller, TMsgGoAndLook, onGoAndLook);
	SUBSCRIBE(player_controller_cientifico, TMsgGoAndLook, onGoAndLook);
	SUBSCRIBE(player_controller_mole, TMsgGoAndLook, onGoAndLook);
	//SUBSCRIBE(player_controller_speedy, TMsgGoAndLook, onGoAndLook);
	//SUBSCRIBE(bt_guard, TMsgGoAndLookAs, onGoAndLook);
	//SUBSCRIBE(bt_mole, TMsgGoAndLookAs, onGoAndLook);

	initLevel(CApp::get().sceneToLoad, false);

	return true;
}

void CEntitiesModule::loadXML(CEntitiesModule::ParsingInfo& info)
{
	//TODO init IdEntities!
	CEntityParser ep(info.reload);
}

void CEntitiesModule::initLevel(string level, bool check_point) {
	bool reload = level == current_level;
	// Restart Timers LUA
	logic_manager->resetTimers();

	map<std::string, std::string> fields = readIniAtrDataStr(CApp::get().file_options_json, "scenes");
	sala = fields[level];

	SBB::postBool("navmesh", false);
	salaloc = "data/navmeshes/" + sala + ".data";

	CEntityParser ep(reload);

	dbg("Loading scene... (%d entities)\n", size());

	bool is_ok;
	// Parte inmutable de la escena
	if (!reload) {
		is_ok = ep.xmlParseFile("data/scenes/" + sala + ".xml");
		assert(is_ok);
	}

	// Parte cambiante
	if (reload && check_point) is_ok = ep.xmlParseFile("data/scenes/" + sala + "_save.xml");
	else is_ok = ep.xmlParseFile("data/scenes/" + sala + "_init.xml");
	assert(is_ok);

	{
		CEntityParser ep(reload);
		is_ok = ep.xmlParseFile("data/scenes/test_lights.xml");
		assert(is_ok);
	}

	dbg("Scene Loaded! (%d entities)\n", size());

	// GENERATE NAVMESH
	collisionables = ep.getCollisionables();
	SBB::postHandlesVector("collisionables", collisionables);
	CNavmesh nav;
	nav.m_input.clearInput();
	dbg(" GENERANDO NAVMESH INPUT...\n");
	for (CHandle han : collisionables) {
		CEntity * e = han;
		dbg("Navmesh, collisionable: %s\n", e->getName());
		if (e) {
			TCompTransform * trans = e->get<TCompTransform>();
			TCompPhysics * p = e->get<TCompPhysics>();
			if (!p) continue;
			auto actor = p->getActor();
			if (!actor) continue;
			PxBounds3 bounds = actor->getWorldBounds();
			VEC3 min, max;
			min.x = bounds.minimum.x;
			min.y = bounds.minimum.y;
			min.z = bounds.minimum.z;
			max.x = bounds.maximum.x;
			max.y = bounds.maximum.y;
			max.z = bounds.maximum.z;

			auto rb = p->getActor()->isRigidStatic();
			if (rb) {
				int nBShapes = rb->getNbShapes();
				PxShape **ptr;
				ptr = new PxShape*[nBShapes];
				rb->getShapes(ptr, 1);
				for (int i = 0; i < nBShapes; i++) {
					PxTriangleMeshGeometry meshGeom;
					assert(ptr[i]);
					if (ptr[i]->getTriangleMeshGeometry(meshGeom)) {
						nav.m_input.addInput(meshGeom.triangleMesh, PhysxConversion::PxVec3ToVec3(rb->getGlobalPose().p), min, max, trans->getRotation());
					}
				}
			}
			else {
				nav.m_input.addInput(min, max);
			}
		}
	}
	nav.m_input.computeBoundaries();
	SBB::postNavmesh(nav);
	std::ifstream is(salaloc.c_str());
	bool recalc = !is.is_open();
	is.close();
	SBB::postBool(sala, false);
	//------------------------------
	if (!recalc) {
		// restore the navmesh from the archive
		//std::thread thre(&CEntitiesModule::readNavmesh, this);
		//thre.detach();
		readNavmesh();
	}
	else {
		// make mesh on a separate thread
		//std::thread thre(&CEntitiesModule::recalcNavmesh, this);
		//thre.detach();
		recalcNavmesh();
	}
	//------------------------------

	TTagID tagIDcamera = getID("camera_main");
	TTagID tagIDwall = getID("breakable_wall");
	TTagID tagIDminus = getID("minus_wall");
	TTagID tagIDplus = getID("plus_wall");
	TTagID tagIDrec = getID("recover_point");

	// Camara del player
	CHandle camera = tags_manager.getFirstHavingTag("camera_main");
	CEntity * camera_e = camera;
	CHandle t = tags_manager.getFirstHavingTag("player");
	CEntity * target_e = t;
	if (camera_e) {
		TCompCameraMain * pcam = camera_e->get<TCompCameraMain>();

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
	}
	// SET PLAYER INITIAL ROOM
	int room_name = -1;
	if (target_e) {
		TCompRoom * player_room = target_e->get<TCompRoom>();
		if (player_room) {
			room_name = player_room->name[0];
		}
	}
	SBB::postSala(room_name);

	//}
	TTagID generators = getID("generator");
	VHandles generatorsHandles = tags_manager.getHandlesByTag(generators);
	SBB::postHandlesVector("generatorsHandles", generatorsHandles);

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
	getHandleManager<bt_scientist>()->onAll(&bt_scientist::Init);
	//getHandleManager<water_controller>()->onAll(&water_controller::Init); --> Se hace en el onCreated!
	getHandleManager<ai_cam>()->onAll(&ai_cam::Init);
	getHandleManager<workbench_controller>()->onAll(&workbench_controller::Init);
	getHandleManager<TCompGenerator>()->onAll(&TCompGenerator::init);
	getHandleManager<TCompWire>()->onAll(&TCompWire::init);
	getHandleManager<TCompPolarized>()->onAll(&TCompPolarized::init);
	getHandleManager<TCompBox>()->onAll(&TCompBox::init);
	getHandleManager<TCompPila>()->onAll(&TCompPila::init);
	getHandleManager<TCompWorkstation>()->onAll(&TCompWorkstation::init);

	//fx
	getHandleManager<TCompFadeScreen>()->onAll(&TCompFadeScreen::init);

	//TODO: Message LevelStart
	GameController->SetGameState(CGameController::RUNNING);
	current_level = level;
	CApp::get().sceneToLoad = "";
	CApp::get().loadedLevelNotify();
}

void CEntitiesModule::saveLevel() {
	string file_name = "data/scenes/" + sala + "_save.xml";
	std::ofstream os(file_name.c_str());
	if (!os.is_open()) {
		assert(false);
	}
	MKeyValue atts;
	atts.writeStartElement(os, "entities");
	getHandleManager<CEntity>()->each([&atts, &os](CEntity * e) {
		if (e->needReload()) {
			e->save(os, atts);
		}
	});
	atts.writeEndElement(os, "entities");
	os.close();
}

void CEntitiesModule::clear(string new_next_level) {
	next_level = new_next_level;
	bool level_changed = (next_level != "" && next_level != current_level);
	static int entities_destroyed = 0;
	getHandleManager<CEntity>()->each([level_changed](CEntity * e) {
		if (!e->isPermanent()) {
			if (level_changed || e->needReload()) {
				CHandle(e).destroy();
				entities_destroyed++;
			}
		}
	});
	dbg("Entities destroyed = %d\n", entities_destroyed);
}

bool CEntitiesModule::isCleared() {
	bool level_changed = (next_level != "" && next_level != current_level);
	getHandleManager<CEntity>()->each([level_changed](CEntity * e) {
		if (!e->isPermanent()) {
			if (level_changed || e->needReload()) {
				return false;
			}
		}
	});
	return true;
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
		getHandleManager<TCompCameraMain>()->updateAll(dt);
		getHandleManager<TCompFadeScreen>()->updateAll(dt);
	}

	if (GameController->GetGameState() == CGameController::RUNNING) {
		// May need here a switch to update wich player controller takes the action - possession rulez
		if (!GameController->IsCinematic()) {
			getHandleManager<player_controller>()->updateAll(dt);
			getHandleManager<player_controller_speedy>()->updateAll(dt);
			getHandleManager<player_controller_mole>()->updateAll(dt);
			getHandleManager<player_controller_cientifico>()->updateAll(dt);
			getHandleManager<TCompController3rdPerson>()->updateAll(dt);
			getHandleManager<TCompFadingMessage>()->updateAll(dt);
			getHandleManager<TCompFadingGlobe>()->updateAll(dt);
			getHandleManager<LogicHelperArrow>()->updateAll(dt);
			getHandleManager<TCompFadeScreen>()->updateAll(dt);
		}

		getHandleManager<TCompCamera>()->updateAll(dt);
		getHandleManager<TCompCameraMain>()->updateAll(dt);
		getHandleManager<TCompLightDir>()->updateAll(dt);

		getHandleManager<SkelControllerGuard>()->updateAll(dt);
		getHandleManager<SkelControllerPlayer>()->updateAll(dt);
		getHandleManager<SkelControllerScientist>()->updateAll(dt);
		getHandleManager<SkelControllerMole>()->updateAll(dt);

		if (use_parallel) {
			getHandleManager<TCompSkeleton>()->updateAllInParallel(dt);
			getHandleManager<TCompBoneTracker>()->updateAllInParallel(dt);
			getHandleManager<TCompSkeletonIK>()->updateAllInParallel(dt);
			getHandleManager<TCompSkeletonLookAt>()->updateAllInParallel(dt);
		}
		else {
			getHandleManager<TCompSkeleton>()->updateAll(dt);
			getHandleManager<TCompBoneTracker>()->updateAll(dt);
			getHandleManager<TCompSkeletonLookAt>()->updateAll(dt);
		}

		if (SBB::readBool("navmesh") && ia_wait > 1.0f) {
			getHandleManager<bt_mole>()->updateAll(dt);
			getHandleManager<bt_scientist>()->updateAll(dt);
			getHandleManager<ai_cam>()->updateAll(dt);
			getHandleManager<workbench_controller>()->updateAll(dt);
			getHandleManager<bt_speedy>()->updateAll(dt);
			getHandleManager<water_controller>()->updateAll(dt);
			getHandleManager<bt_guard>()->updateAll(dt);
		}
		getHandleManager<CStaticBomb>()->updateAll(dt);
		getHandleManager<CThrowBomb>()->updateAll(dt);

		getHandleManager<TCompWire>()->updateAll(dt);
		getHandleManager<TCompGenerator>()->updateAll(dt);
		getHandleManager<TCompPolarized>()->updateAll(dt);

		getHandleManager<TCompLife>()->updateAll(dt);

		getHandleManager<TCompPlatform>()->updateAll(dt);
		getHandleManager<TCompDrone>()->updateAll(dt);
		getHandleManager<TCompBox>()->updateAll(dt);
		getHandleManager<TCompPila>()->updateAll(dt);
		getHandleManager<TCompWorkstation>()->updateAll(dt);
		getHandleManager<magnet_door>()->updateAll(dt);
		getHandleManager<elevator>()->updateAll(dt);
		getHandleManager<TCompTracker>()->updateAll(dt);

		//particles
		getHandleManager<CParticleSystem>()->updateAll(dt);

		getHandleManager<TCompBoxSpawner>()->updateAll(dt);
		getHandleManager<TCompBoxDestructor>()->updateAll(dt);

		getHandleManager<TCompLightPoint>()->updateAll(dt);
		getHandleManager<TCompLightFadable>()->updateAll(dt);

		//Triggers
		getHandleManager<TTriggerLua>()->updateAll(dt);

		//Fx
		getHandleManager<TCompFadeScreen>()->updateAll(dt);

		SBB::update(dt);
	}
	// In this mode, only the animation of the player is updated
	else if (GameController->GetGameState() == CGameController::STOPPED_INTRO) {
		CHandle player_handle = tags_manager.getFirstHavingTag("player");
		CEntity * player_entity = player_handle;

		TCompSkeleton* player_skeleton = player_entity->get<TCompSkeleton>();
		player_skeleton->update(dt);
	}
}

void CEntitiesModule::render() {
	// for each manager
	// if manager has debug render active
	// manager->renderAll()
	if (io->keys['N'].isPressed() && io->keys[VK_CONTROL].isPressed()) {
		SBB::readNavmesh().render();
	}
	auto tech = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
	tech->activate();

#ifdef _DEBUG
	//getHandleManager<TCompTransform>()->onAll(&TCompTransform::render);
#endif

	getHandleManager<TCompSkeleton>()->onAll(&TCompSkeleton::render);
	getHandleManager<TCompCamera>()->onAll(&TCompCamera::render);
	getHandleManager<TCompCameraMain>()->onAll(&TCompCamera::render);
	getHandleManager<TCompLightDir>()->onAll(&TCompLightDir::render);

	getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::render);
	getHandleManager<TCompAbsAABB>()->onAll(&TCompAbsAABB::render);
	getHandleManager<TCompLocalAABB>()->onAll(&TCompLocalAABB::render);
	getHandleManager<TCompFadingMessage>()->onAll(&TCompFadingMessage::render);
	getHandleManager<TCompFadingGlobe>()->onAll(&TCompFadingGlobe::render);

#ifndef NDEBUG
	getHandleManager<TCompBox>()->onAll(&TCompBox::render);
#endif

	RenderManager.renderAll(CHandle(), CRenderTechnique::DBG_OBJS);
	RenderManager.renderAll(CHandle(), CRenderTechnique::UI_OBJS);
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
		SBB::postBool("navmesh", true);
	}
}

void CEntitiesModule::fixedUpdate(float elapsed)
{
	getHandleManager<TCompDrone>()->fixedUpdateAll(elapsed);
}

int CEntitiesModule::size() {
	return getHandleManager<CEntity>()->size();
}