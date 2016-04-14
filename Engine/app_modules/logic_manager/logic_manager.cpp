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
	bindHandle(*slb_manager);

	// load the scripts
	std::vector<std::string> files_to_load = list_files_recursively(lua_script_folder);

	for (auto file : files_to_load)
		slb_script.doFile(file);

	return true;
}

void CLogicManagerModule::update(float dt) {

	std::vector<int> to_erase;

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

void CLogicManagerModule::throwEvent(EVENT evt, std::string params) {

	char lua_code[64];

	switch (evt) {
		case (OnAction) : {
			sprintf(lua_code, "OnAction(%f);", 0.1f);
			break;
		}
		case (OnEnter) : {
			sprintf(lua_code, "OnEnter(%f);", 0.2f);
			break;
		}
		case (OnLeave) : {
			sprintf(lua_code, "OnLeave(%f);", 0.3f);
			break;
		}
		case (OnGameStart) : {
			//sprintf(lua_code, "OnGameStart(%f);", 0.4f);
			char command_code[64];
			sprintf(command_code, "dbg('%s');", "TIMER - OGS");
			sprintf(lua_code, "execCommandTest(\"%s\", %f);", command_code, 5.f);
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
			sprintf(lua_code, "OnZoneStart001(%f);", 0.5f);
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
			sprintf(lua_code, "OnChangePolarity(%f);", 0.5f);
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

void CLogicManagerModule::throwUserEvent(std::string evt, std::string params) {
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
		;
}

void CLogicManagerModule::bindHandle(SLB::Manager& m) {
	SLB::Class<SLBHandle>("Handle", &m)
		.comment("Handle class")
		.constructor()
		// sets the handle pointer to the player
		.set("get_player", &SLBHandle::getPlayer)
		.comment("Sets the handle pointer to the player")
		// set handle position function
		.set("set_player_position", &SLBHandle::setPlayerPosition)
		.comment("Sets the position of the handle")
		.param("float: x coordinate")
		.param("float: x coordinate")
		.param("float: x coordinate")
		// basic coordinates functions
		.set("get_player_x", &SLBHandle::getPlayerX)
		.comment("returns the X coordinate")
		.set("get_player_y", &SLBHandle::getPlayerY)
		.comment("returns the Y coordinate")
		.set("get_player_z", &SLBHandle::getPlayerZ)
		.comment("returns the Z coordinate")
		;
}