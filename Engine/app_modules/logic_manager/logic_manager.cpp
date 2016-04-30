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
	bindPublicFunctions(*slb_manager);
	bindPlayer(*slb_manager);
	bindHandle(*slb_manager);

	// load the scripts
	std::vector<std::string> files_to_load = list_files_recursively(lua_script_folder);

	for (auto file : files_to_load)
		slb_script.doFile(file);

	return true;
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
}

void CLogicManagerModule::throwEvent(EVENT evt, std::string params, uint32_t handle_id) {

	char lua_code[64];

	switch (evt) {
		case (OnAction) : {
			sprintf(lua_code, "OnAction(%f);", 0.1f);
			break;
		}
		case (OnEnter) : {
			sprintf(lua_code, "OnEnter(\"%s\", %d);", params.c_str(), handle_id);
			break;
		}
		case (OnLeave) : {
			sprintf(lua_code, "OnLeave%s(%f);", params.c_str(), 0.3f);
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
		case (OnLevelStart001) : {
			char command_code[64];
			sprintf(command_code, "dbg('%s');", "TIMER - OLS");
			sprintf(lua_code, "execCommandTest(\"%s\", %f);", command_code, 5.f);
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

		case (OnGuardRemoveBox) : {
			sprintf(lua_code, "OnGuardRemoveBox(%f);", 0.5f);
			break;
		}

		case (OnGuardOvercharged) : {
			sprintf(lua_code, "OnGuardOvercharged(%f);", 0.5f);
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
			sprintf(lua_code, "OnDetected(%f);", 0.5f);
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
		default : {
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

void CLogicManagerModule::throwUserEvent(std::string evt, std::string params, uint32_t handle_id) {
	// construct the lua code using the event and the specified parameters
	std::string lua_code = evt;

	// execute the string generated
	slb_script.doString(lua_code);
}

void CLogicManagerModule::stop() {
	slb_manager->destroyDefaultManager();
	command_queue.clear();
}

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
		.param("Route of the ambient sound")
		;
}

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
		.param("float: x coordinate")
		.param("float: x coordinate")
		// basic coordinates functions
		.set("get_x", &SLBPlayer::getPlayerX)
		.comment("returns the X coordinate")
		.set("get_y", &SLBPlayer::getPlayerY)
		.comment("returns the Y coordinate")
		.set("get_z", &SLBPlayer::getPlayerZ)
		.comment("returns the Z coordinate")
		;
}

void CLogicManagerModule::bindHandle(SLB::Manager& m) {
	SLB::Class<SLBHandle>("Handle", &m)
		.comment("Handle class")
		.constructor()
		// sets the handle pointer to the handle with the specified id
		.set("get_handle_by_id", &SLBHandle::getHandleById)
		.comment("Finds the handle with the specified id")
		.param("int: handle id")
		// sets the handle pointer to the handle with the specified name and tag
		.set("get_handle_by_name_tag", &SLBHandle::getHandleByNameTag)
		.comment("Finds the handle with the specified name and tag")
		.param("string: handle name")
		.param("string: handle tag")
		// set handle position function
		.set("set_position", &SLBHandle::setPosition)
		.comment("Sets the position of the NPC")
		.param("float: x coordinate")
		.param("float: x coordinate")
		.param("float: x coordinate")
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
		// toggle guards formation
		.set("toggle_guard_formation", &SLBHandle::toggleGuardFormation)
		.comment("Activates/desactivates the guard formation states.")
		;
}