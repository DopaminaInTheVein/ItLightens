#include "mcv_platform.h"
#include "logic_manager.h"
#include "app_modules\io\io.h"

void LogicManager::init() {

	// Binds here, if needed
	//bind(slb_manager);

	// Init and execute the script
	SLB::Script slb_script(&slb_manager);
	slb_script.doFile(lua_script_route);
}

void LogicManager::throwEvent(EVENT evt/*, params*/) {

	std::string lua_code;

	switch (evt) {
		case (OnAction) : {
			lua_code = std::string("\"sampleFunction(Jose);\"");
			break;
		}
		case (OnEnter) : {
			lua_code = "";
			break;
		}
		case (OnLeave) : {
			lua_code = "";
			break;
		}
		case (OnGameStart) : {
			lua_code = "";
			break;
		}
		case (OnGameEnd) : {
			lua_code = "";
			break;
		}
		case (OnLevelStart001) : {
			lua_code = "";
			break;
		}
		case (OnZoneStart001) : {
			lua_code = "";
			break;
		}
		case (OnZoneEnd001) : {
			lua_code = "";
			break;
		}
		case (OnTimeout) : {
			lua_code = "";
			break;
		}
		case (OntTimerStart) : {
			lua_code = "";
			break;
		}

		case (OnPlayerDead) : {
			lua_code = "";
			break;
		}
		case (OnInterruptHit) : {
			lua_code = "";
			break;
		}
		case (OnStartReceiveHit) : {
			lua_code = "";
			break;
		}
		case (OnEndReceiveHit) : {
			lua_code = "";
			break;
		}

		case (OnEmitParticles) : {
			lua_code = "";
			break;
		}
		case (OnChangePolarity) : {
			lua_code = "";
			break;
		}
		case (OnPickupBox) : {
			lua_code = "";
			break;
		}
		case (OnLeaveBox) : {
			lua_code = "";
			break;
		}
		case (OnPossess) : {
			lua_code = "";
			break;
		}
		case (OnUnpossess) : {
			lua_code = "";
			break;
		}
		case (OnDash) : {
			lua_code = "";
			break;
		}
		case (OnBlink) : {
			lua_code = "";
			break;
		}
		case (OnBreakWall) : {
			lua_code = "";
			break;
		}
		case (OnUseCable) : {
			lua_code = "";
			break;
		}
		case (OnStun) : {
			lua_code = "";
			break;
		}
		case (OnStunned) : {
			lua_code = "";
			break;
		}
		case (OnLiquid) : {
			lua_code = "";
			break;
		}
		case (OnBeingAttracted) : {
			lua_code = "";
			break;
		}
		case (OnOvercharge) : {
			lua_code = "";
			break;
		}
		case (OnDoubleJump) : {
			lua_code = "";
			break;
		}
		case (OnDetected) : {
			lua_code = "";
			break;
		}
		case (OnBeaconDetect) : {
			lua_code = "";
			break;
		}

		case (OnEnterPC) : {
			lua_code = "";
			break;
		}
		case (OnLeavePC) : {
			lua_code = "";
			break;
		}
		default : {
			lua_code = "";
		}
	}

	try {
		// execute the string generated
		SLB::Script slb_script(&slb_manager);
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
	SLB::Script slb_script(&slb_manager);
	slb_script.doString(lua_code);
}

void LogicManager::shutDown() {
	slb_manager.destroyDefaultManager();
}

void LogicManager::bind(SLB::Manager& m) {
	//Sample

	/*class Character {
	std::string name;

	public:
		int life;

		Character() {}
		void setName(const char* aname) {
			name = aname;
		}
		const char* getName() {
			return name.c_str();
		}
	};*/

	/*SLB::Class<Character>("Character", &m)
		.comment("Character game class")
		.constructor()
		.set("getName", &Character::getName)
		.set("setName", &Character::setName)
		.comment("Method to change the character name")
		.param("New name of the character")
		.property("life", &Character::life)
		;*/
}
