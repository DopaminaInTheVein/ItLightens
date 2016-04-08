#include "mcv_platform.h"
#include "logic_manager.h"
#include "app_modules\io\io.h"
#include "SLB\slb_public_functions.h"

void LogicManager::init() {

	slb_manager = slb_script.getManager();

	// Binds here, if needed
	bindPublicFunctions(*slb_manager);

	//list_files_recursively(lua_script_folder);

	// Init and execute the script
	slb_script.doFile(lua_script_route);
}

void LogicManager::throwEvent(EVENT evt/*, params*/) {

	char lua_code[64];

	switch (evt) {
		case (OnAction) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnEnter) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnLeave) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnGameStart) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnGameEnd) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnLevelStart001) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnZoneStart001) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnZoneEnd001) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnTimeout) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OntTimerStart) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}

		case (OnPlayerDead) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnInterruptHit) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnStartReceiveHit) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnEndReceiveHit) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}

		case (OnEmitParticles) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnChangePolarity) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnPickupBox) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnLeaveBox) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnPossess) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnUnpossess) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnDash) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnBlink) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnBreakWall) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnUseCable) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnStun) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnStunned) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnLiquid) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnBeingAttracted) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnOvercharge) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnDoubleJump) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnDetected) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnBeaconDetect) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}

		case (OnEnterPC) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		case (OnLeavePC) : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
			break;
		}
		default : {
			sprintf(lua_code, "sampleFunction(%f);", 0.5f);
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

void LogicManager::throwUserEvent(std::string evt/*, params*/) {
	// construct the lua code using the event and the specified parameters
	std::string lua_code = evt;

	// execute the string generated
	//SLB::Script slb_script(&slb_manager);
	slb_script.doString(lua_code);
}

void LogicManager::shutDown() {
	slb_manager->destroyDefaultManager();
}

void LogicManager::bindPublicFunctions(SLB::Manager& m) {
	SLB::Class<SLBPublicFunctions>("Public", &m)
		.constructor()
		.comment("Public functions class")
		// basic print function
		.set("print", &SLBPublicFunctions::print)
		.comment("Prints via VS console")
		.param("Text to print")
		;
}
