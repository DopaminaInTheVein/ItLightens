#include "mcv_platform.h"
#include "logic_manager.h"
#include "app_modules\io\io.h"
#include "slb_public_functions.h"

#include "utils\utils.h"

extern CLogicManagerModule* logic_manager = nullptr;

CLogicManagerModule::CLogicManagerModule() {}

bool CLogicManagerModule::start() {
	slb_manager = slb_script.getManager();

	assert(slb_manager);

	// Binds here, if needed
	bindPosition(*slb_manager);
	bindPlayer(*slb_manager);
	bindHandle(*slb_manager);
	bindHandleGroup(*slb_manager);
	bindCamera(*slb_manager);
	bindPublicFunctions(*slb_manager);

	// load the scripts
	std::vector<std::string> files_to_load = list_files_recursively(lua_script_folder);

	for (auto file : files_to_load)
		slb_script.doFile(file);

	return true;
}

void CLogicManagerModule::reloadFile(std::string filename) {
	slb_script.doFile(filename);
}

void CLogicManagerModule::update(float dt) {
	// update the timer of each command
	for (std::deque<command>::iterator command_it = command_queue.begin(); command_it != command_queue.end(); ) {
		command_it->execution_time -= dt;

		if (command_it->execution_time < 0.f) {
			slb_script.doString(command_it->code);
			command_it = command_queue.erase(command_it);
		}
		else {
			command_it++;
		}
	}

	for (auto c : command_queue_to_add) {
		command_queue.push_back(c);
	}
	command_queue_to_add.clear();
}

void CLogicManagerModule::resetTimers() {
	command_queue.clear();
}

void CLogicManagerModule::throwEvent(EVENT evt, std::string params, CHandle handle) {//, uint32_t handle_id) {
	char lua_code[64];
	caller_handle = handle;

	switch (evt) {
	case (OnAction) : {
		sprintf(lua_code, "OnAction(\"%s\");", params.c_str());
		break;
	}
	case (OnEnter) : {
		sprintf(lua_code, "OnEnter(\"%s\");", params.c_str());
		break;
	}
	case (OnLeave) : {
		sprintf(lua_code, "OnLeave(\"%s\");", params.c_str());
		break;
	}
	case (OnActionSci) : {
		sprintf(lua_code, "OnActionSci(\"%s\");", params.c_str());
		break;
	}
	case (OnActionMole) : {
		sprintf(lua_code, "OnActionMole(\"%s\");", params.c_str());
		break;
	}
	case (OnActionPila) : {
		sprintf(lua_code, "OnActionPila(\"%s\");", params.c_str());
		break;
	}
	case (OnPutPila) : {
		sprintf(lua_code, "OnPutPila(\"%s\");", params.c_str());
		break;
	}
	case (OnRemovePila) : {
		sprintf(lua_code, "OnRemovePila(\"%s\");", params.c_str());
		break;
	}
	case (OnGameStart) : {
		sprintf(lua_code, "OnGameStart(%f);", 0.4f);
		/*char command_code[64];
		sprintf(command_code, "dbg('%s');", "TIMER - OGS");
		sprintf(lua_code, "execCommandTest(\"%s\", %f);", command_code, 5.f);*/
		break;
	}
	case (OnGameEnd) : {
		//sprintf(lua_code, "OnGameEnd(%f);", 0.5f);
		sprintf(lua_code, "teleportPlayer(%f, %f, %f);", 0.0f, 0.0f, -22.0f);
		break;
	}
	case (OnLevelStart) : {
		//char command_code[64];
		//sprintf(command_code, "dbg('%s');", "TIMER - OLS");
		sprintf(lua_code, "OnLevelStart(\"%s\");", params.c_str());
		break;
	}
	case (OnZoneStart001) : {
		sprintf(lua_code, "teleportSpeedy('%s', %f, %f, %f);", "speedy1", 0.0f, 0.0f, -22.0f);
		break;
	}
	case (OnZoneEnd001) : {
		sprintf(lua_code, "OnZoneEnd001(%f);", 0.5f);
		break;
	}
	case (OnTimeout) : {
		sprintf(lua_code, "OnTimeout(%f);", 0.5f);
		break;
	}
	case (OntTimerStart) : {
		sprintf(lua_code, "OntTimerStart(%f);", 0.5f);
		break;
	}

	case (OnPlayerDead) : {
		sprintf(lua_code, "OnPlayerDead(%f);", 0.5f);
		break;
	}

	case (OnGuardAttack) : {
		sprintf(lua_code, "OnGuardAttack(%f);", 0.5f);
		break;
	}

	case (OnGuardAttackEnd) : {
		sprintf(lua_code, "OnGuardAttackEnd(%f);", 0.5f);
		break;
	}

	case (OnGuardRemoveBox) : {
		sprintf(lua_code, "OnGuardRemoveBox(%f);", 0.5f);
		break;
	}

	case (OnGuardOvercharged) : {
		sprintf(lua_code, "OnGuardOvercharged(%f);", 0.5f);
		break;
	}
	case (OnGuardBoxHit) : {
		sprintf(lua_code, "OnGuardBoxHit(%f);", 0.5f);
		break;
	}
	case (OnInterruptHit) : {
		sprintf(lua_code, "OnInterruptHit(%f);", 0.5f);
		break;
	}
	case (OnStartReceiveHit) : {
		sprintf(lua_code, "OnStartReceiveHit(%f);", 0.5f);
		break;
	}
	case (OnEndReceiveHit) : {
		sprintf(lua_code, "OnEndReceiveHit(%f);", 0.5f);
		break;
	}

	case (OnEmitParticles) : {
		sprintf(lua_code, "OnEmitParticles(%f);", 0.5f);
		break;
	}
	case (OnChangePolarity) : {
		sprintf(lua_code, "OnChangePolarity(\"%s\");", params.c_str());
		break;
	}
	case (OnPickupBox) : {
		sprintf(lua_code, "OnPickupBox(%f);", 0.5f);
		break;
	}
	case (OnLeaveBox) : {
		sprintf(lua_code, "OnLeaveBox(%f);", 0.5f);
		break;
	}
	case (OnPossess) : {
		sprintf(lua_code, "OnPossess(%f);", 0.5f);
		break;
	}
	case (OnUnpossess) : {
		sprintf(lua_code, "OnUnpossess(%f);", 0.5f);
		break;
	}
	case (OnDash) : {
		sprintf(lua_code, "OnDash(%f);", 0.5f);
		break;
	}
	case (OnBlink) : {
		sprintf(lua_code, "OnBlink(%f);", 0.5f);
		break;
	}
	case (OnBreakWall) : {
		sprintf(lua_code, "OnBreakWall(%f);", 0.5f);
		break;
	}
	case (OnRechargeDrone) : {
		sprintf(lua_code, "OnRechargeDrone(%f);", 0.5);
		break;
	}
	case (OnNotRechargeDrone) : {
		sprintf(lua_code, "OnNotRechargeDrone(%f);", 0.5);
		break;
	}
	case (OnRepairDrone) : {
		sprintf(lua_code, "OnRepairDrone(%f);", 0.5);
		break;
	}
	case (OnUseCable) : {
		sprintf(lua_code, "OnUseCable(%f);", 0.5f);
		break;
	}
	case (OnUseGenerator) : {
		sprintf(lua_code, "OnUseGenerator(%f);", 0.5f);
		break;
	}
	case (OnStun) : {
		sprintf(lua_code, "OnStun(%f);", 0.5f);
		break;
	}
	case (OnStunned) : {
		sprintf(lua_code, "OnStunned(%f);", 0.5f);
		break;
	}
	case (OnStunnedEnd) : {
		sprintf(lua_code, "OnStunnedEnd(%f);", 0.5f);
		break;
	}
	case (OnLiquid) : {
		sprintf(lua_code, "OnLiquid(%f);", 0.5f);
		break;
	}
	case (OnBeingAttracted) : {
		sprintf(lua_code, "OnBeingAttracted(%f);", 0.5f);
		break;
	}
	case (OnOvercharge) : {
		sprintf(lua_code, "OnOvercharge(%f);", 0.5f);
		break;
	}
	case (OnJump) : {
		sprintf(lua_code, "OnJump(%f);", 0.5f);
		break;
	}
	case (OnDoubleJump) : {
		sprintf(lua_code, "OnDoubleJump(%f);", 0.5f);
		break;
	}
	case (OnDetected) : {
		char * pars = new char[params.size() + 1];
		std::copy(params.begin(), params.end(), pars);
		pars[params.size()] = '\0';

		char *p = strtok(pars, " ");
		std::vector<float> float_params;

		while (p) {
			float_params.push_back(atof(p));
			p = strtok(NULL, " ");
		}

		delete[] pars;

		sprintf(lua_code, "OnDetected(%f,%f,%f,%f);", float_params[0], float_params[1], float_params[2], float_params[3]);

		break;
	}
	case (OnNextPatrol) : {
		sprintf(lua_code, "OnNextPatrol(\"%s\");", params.c_str());
		break;
	}
	case (OnBeaconDetect) : {
		sprintf(lua_code, "OnBeaconDetect(%f);", 0.5f);
		break;
	}

	case (OnEnterPC) : {
		sprintf(lua_code, "OnEnterPC(%f);", 0.5f);
		break;
	}
	case (OnLeavePC) : {
		sprintf(lua_code, "OnLeavePC(%f);", 0.5f);
		break;
	}
	case (OnDoorOpening) : {
		sprintf(lua_code, "OnDoorOpening();");
		break;
	}
	case (OnDoorOpened) : {
		sprintf(lua_code, "OnDoorOpened();");
		break;
	}
	case (OnDoorClosing) : {
		sprintf(lua_code, "OnDoorClosing();");
		break;
	}
	case (OnDoorClosed) : {
		sprintf(lua_code, "OnDoorClosed();");
		break;
	}
	case (OnCinematicSkipped) : {
		sprintf(lua_code, "OnCinematicSkipped(\"%s\");", params.c_str());
		break;
	}
	case (OnCinematicEnd) : {
		sprintf(lua_code, "OnCinematicEnd(\"%s\");", params.c_str());
		break;
	}
	case (OnElevatorUp) : {
		sprintf(lua_code, "OnElevatorUp(\"%s\");", params.c_str());
		break;
	}
	case (OnElevatorDown) : {
		sprintf(lua_code, "OnElevatorDown(\"%s\");", params.c_str());
		break;
	}
	case (OnElevatorGoingUp) : {
		sprintf(lua_code, "OnElevatorGoingUp(\"%s\");", params.c_str());
		break;
	}
	case (OnElevatorGoingDown) : {
		sprintf(lua_code, "OnElevatorGoingDown(\"%s\");", params.c_str());
		break;
	}
	case (OnExplode) : {
		sprintf(lua_code, "OnExplode(\"%s\");", params.c_str());
		break;
	}
	default: {
		sprintf(lua_code, "dbg('The event %s does not exist!');", evt);
	}
	}

	try {
		// execute the string generated
		slb_script.doString(lua_code);
	}
	catch (int e) {
		dbg("Exception %d occurred!", e);
	}
}

void CLogicManagerModule::throwUserEvent(std::string evt, std::string params, CHandle handle) {//, uint32_t handle_id) {
  // construct the lua code using the event and the specified parameters
	std::string lua_code = evt;

	// Ojo! Params no se usa!

	// execute the string generated
	if (lua_code != "")	slb_script.doString(lua_code);
}

void CLogicManagerModule::stop() {
	slb_manager->destroyDefaultManager();
	command_queue.clear();
}

// Player LUA functions
void CLogicManagerModule::bindPosition(SLB::Manager& m) {
	SLB::Class<SLBPosition>("Pos", &m)
		.comment("Position class")
		.constructor()
		// gets position coords (x)
		.set("x", &SLBPosition::X)
		.comment("Get x pos")
		// gets position coords (y)
		.set("y", &SLBPosition::Y)
		.comment("Get y pos")
		// gets position coords (z)
		.set("z", &SLBPosition::Z)
		.comment("Get z pos")
		// set position coords
		.set("set_xyz", &SLBPosition::setXYZ)
		.comment("Set xyz pos")
		;
}

// Player LUA functions
void CLogicManagerModule::bindPlayer(SLB::Manager& m) {
	SLB::Class<SLBPlayer>("Player", &m)
		.comment("Player class")
		.constructor()
		// sets the handle pointer to the player
		.set("get_player", &SLBPlayer::getPlayer)
		.comment("Sets the handle pointer to the player")
		// set handle position function
		.set("set_position", &SLBPlayer::setPlayerPosition)
		.comment("Sets the position of the player")
		.param("float: x coordinate")
		.param("float: y coordinate")
		.param("float: z coordinate")
		// teleport and orientate player
		.set("teleport", &SLBPlayer::teleport)
		.comment("Teleport player")
		.param("Point name to teleport")

		// basic coordinates functions
		.set("get_x", &SLBPlayer::getPlayerX)
		.comment("returns the X coordinate")
		.set("get_y", &SLBPlayer::getPlayerY)
		.comment("returns the Y coordinate")
		.set("get_z", &SLBPlayer::getPlayerZ)
		.comment("returns the Z coordinate")
		;
}

// Generic handle LUA functions
void CLogicManagerModule::bindHandle(SLB::Manager& m) {
	SLB::Class<SLBHandle>("Handle", &m)
		.comment("Handle class")
		.constructor()
		// get name
		.set("get_name", &SLBHandle::getName)
		.comment("Get the name of the entity handle")
		// sets the handle pointer to the player
		.set("get_player", &SLBHandle::getPlayer)
		.comment("Sets the handle pointer to the player")
		// sets the handle pointer to the handle with the specified id
		.set("get_handle_by_id", &SLBHandle::getHandleById)
		.comment("Finds the handle with the specified id")
		.param("int: handle id")
		// sets the handle pointer to the handle with the specified name and tag
		.set("get_handle_by_name_tag", &SLBHandle::getHandleByNameTag)
		.comment("Finds the handle with the specified name and tag")
		.param("string: handle name")
		.param("string: handle tag")
		// sets the handle to the event thrower
		.set("getHandleCaller", &SLBHandle::getHandleCaller)
		.comment("Finds the handle who called this event")
		// destroy the handler
		.set("destroy", &SLBHandle::destroy)
		.comment("Destroy this element")
		// set handle position function (coords)
		.set("set_position", &SLBHandle::setPosition)
		.comment("Sets the position of the NPC")
		.param("float: x coordinate")
		.param("float: y coordinate")
		.param("float: z coordinate")
		// set handle position function (object)
		.set("set_pos", &SLBHandle::setPos)
		.comment("Sets the position of the NPC")
		.param("position: position object")
		// get handle position function (object)
		.set("get_pos", &SLBHandle::getPos)
		.comment("Get the position of the NPC")

		// basic coordinates functions
		.set("get_x", &SLBHandle::getX)
		.comment("returns the X coordinate")
		.set("get_y", &SLBHandle::getY)
		.comment("returns the Y coordinate")
		.set("get_z", &SLBHandle::getZ)
		.comment("returns the Z coordinate")
		// go to point function
		.set("go_to_point", &SLBHandle::goToPoint)
		.comment("The NPC moves to the specified position")
		.param("float: x coordinate")
		.param("float: y coordinate")
		.param("float: z coordinate")
		// go to and orientate as another handle
		.set("go_and_look_as", &SLBHandle::goAndLookAs)
		.comment("The Player or NPC moves to h and look as h")
		.param("handle: target")
		.param("string: code executed when target was reached")
		// follow a tracker
		.set("follow_tracker", &SLBHandle::followTracker)
		.comment("This starts to follow a tracker")
		.param("handle: handle that contains the target tracker")
		.param("speed: linear velocity following the tracker")
		// toggle guards formation
		.set("toggle_guard_formation", &SLBHandle::toggleGuardFormation)
		.comment("Activates/desactivates the guard formation states.")
		// toggle scientist busy
		.set("toggle_scientist_busy", &SLBHandle::toggleScientistBusy)
		.comment("Activates/desactivates the scientist busy states.")
		// sets actionable
		.set("setActionable", &SLBHandle::setActionable)
		.comment("Set if the element is actionable (0: false, otherwise: true)")
		.param("int: enabled")
		// activate
		.set("activate", &SLBHandle::activate)
		.comment("Activate the element (send TMsgActivate)")
		// sets polarity
		.set("setPolarity", &SLBHandle::setPolarity)
		.comment("Set new polarity")
		.param("int:  (negative is minus, 0 is neutral, positive is plus)")
		// sets locked
		.set("setLocked", &SLBHandle::setLocked)
		.comment("Set if the element is locked")
		.param("int:  (0: false, otherwise: true")
		// Ask Pila Container
		.set("has_pila", &SLBHandle::hasPila)
		.comment("Get true if the handle contains a pila")
		// Ask Pila Container Charged
		.set("has_pila_charged", &SLBHandle::hasPilaCharged)
		.comment("Return true if the handle has a charged cell")
		// Charge Pila
		.set("set_charged", &SLBHandle::setCharged)
		.comment("Set its cell charged/empty")
		.param("boolean: charged")
		// Ask pila charged
		.set("is_charged", &SLBHandle::isCharged)
		.comment("return true if the handle is charged")
		;
}

// Generic handle LUA functions
void CLogicManagerModule::bindHandleGroup(SLB::Manager& m) {
	SLB::Class<SLBHandleGroup>("HandleGroup", &m)
		.comment("Handle Group class")
		.constructor()
		// sets the handle pointer to the handle with the specified id
		.set("get_handles_by_tag", &SLBHandleGroup::getHandlesByTag)
		.comment("Finds all handles with the specified tag")
		.param("string: handle tag")
		// awake elements
		.set("awake", &SLBHandleGroup::awake)
		.comment("Awake slept elements")
		// Remove Physics
		.set("remove_physics", &SLBHandleGroup::removePhysics)
		.comment("Remove their physic components")
		;
}

// Camera LUA functions
void CLogicManagerModule::bindCamera(SLB::Manager& m) {
	SLB::Class<SLBCamera>("Camera", &m)
		.comment("Camera class")
		.constructor()
		// sets the handle pointer to the player
		.set("get_camera", &SLBCamera::getCamera)
		.comment("Gets the camera handler")
		// set camera distance to player
		.set("set_distance_to_target", &SLBCamera::setDistanceToTarget)
		.comment("Sets the camera distance to the target")
		.param("float: distance")
		// sets the camera speed
		.set("set_speed", &SLBCamera::setSpeed)
		.comment("Sets the camera speed")
		.param("float: speed")
		// sets the camera speed when unlocked
		.set("set_speed_unlocked", &SLBCamera::setSpeedUnlocked)
		.comment("Sets the camera speed when unlocked")
		.param("float: speed")
		// sets the rotation sensibility of the camera
		.set("set_rotation_sensibility", &SLBCamera::setRotationSensibility)
		.comment("Sets the rotation sensibility of the camera")
		.param("float: sensibility (in degrees)")
		// sets the camera position offset
		.set("set_position_offset", &SLBCamera::setPositionOffset)
		.comment("Sets the position offset of the camera")
		.param("float: x coord offset")
		.param("float: y coord offset")
		.param("float: z coord offset")
		// run cinematic
		.set("run_cinematic", &SLBCamera::runCinematic)
		.comment("Run cinematic defined in the specified guided camera")
		.param("string: guided camera name")
		.param("speed: speed of camera movement (0 means default speed)")
		// Fade In
		.set("fade_in", &SLBCamera::fadeIn)
		.comment("Start fade in")
		.param("float: time fade, if time <= 0 set default")
		// Fade Out
		.set("fade_out", &SLBCamera::fadeOut)
		.comment("Start fade out")
		.param("float: time fade, if time <= 0 set default")
		;
}

// General LUA functions
void CLogicManagerModule::bindPublicFunctions(SLB::Manager& m) {
	SLB::Class<SLBPublicFunctions>("Public", &m)
		.comment("Public functions class")
		.constructor()
		// execute command function
		.set("exec_command", &SLBPublicFunctions::execCommand)
		.comment("Executes the specified command after a given time")
		.param("string: code to execute")
		.param("float: time until execution")
		// basic print function
		.set("print", &SLBPublicFunctions::print)
		.comment("Prints via VS console")
		.param("Text to print")
		// Enable and disable controls
		.set("setControlEnabled", &SLBPublicFunctions::setControlEnabled)
		.comment("Enable or disable controls\n")
		.param("int: 0 disabled, otherwise enabled")
		.param("float: time until execution")
		// play sound function
		.set("play_sound", &SLBPublicFunctions::playSound)
		.comment("Executes the specified sound effect")
		.param("Route of the sound")
		// play music function
		.set("play_music", &SLBPublicFunctions::playMusic)
		.comment("Executes the specified music")
		.param("Route of the music")
		// play voice function
		.set("play_voice", &SLBPublicFunctions::playVoice)
		.comment("Executes the specified voice")
		.param("Route of the voice")
		// play ambient function
		.set("play_ambient", &SLBPublicFunctions::playAmbient)
		.comment("Executes the specified ambient sound")
		// Change Player Room
		.set("player_room", &SLBPublicFunctions::playerRoom)
		.comment("Change Player Room")
		.param("int: new room")
		// launch text span related to npc talks
		.set("player_talks", &SLBPublicFunctions::playerTalks)
		.comment("Shows the specified text for aq limited time")
		.param("string: text to show")
		.param("string: icon to show")
		.param("string: text to show if icon not loaded")
		// launch text span related to npc talks with colors
		.set("player_talks_color", &SLBPublicFunctions::playerTalksWithColor)
		.comment("Shows the specified text for aq limited time")
		.param("string: text to show")
		.param("string: icon to show")
		.param("string: text to show if icon not loaded")
		.param("string: HEX BACKGROUND COLOR -> #RRGGBBAA")
		.param("string: HEX TEXT COLOR -> #RRGGBBAA")
		// launch text span related to npc talks with colors
		.set("character_globe", &SLBPublicFunctions::characterGlobe)
		.comment("Shows the specified globe for a limited time")
		.param("string: text to show")
		.param("float: distance to the player")
		.param("float: x coord of the character")
		.param("float: y coord of the character")
		.param("float: z coord of the character")
		// launch text span related to npc talks with colors
		.set("character_globe_color", &SLBPublicFunctions::characterGlobeWithColor)
		.comment("Shows the specified globe for a limited time")
		.param("string: text to show")
		.param("float: distance to the player")
		.param("float: x coord of the character")
		.param("float: y coord of the character")
		.param("float: z coord of the character")
		.param("string: HEX BACKGROUND COLOR -> #RRGGBBAA")
		.param("string: HEX TEXT COLOR -> #RRGGBBAA")
		// launch intro state
		.set("toggle_intro_state", &SLBPublicFunctions::toggleIntroState)
		.comment("Toggles the intro game state")
		;
}