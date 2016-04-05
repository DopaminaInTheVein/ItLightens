#ifndef INC_LOGIC_MANAGER_H_
#define	INC_LOGIC_MANAGER_H_

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "SLB\SLB.hpp"

class LogicManager
{

	SLB::Manager slb_manager;

	std::string lua_script_route = "data/lua_scripts/logic.lua";

public:

	enum EVENT {
		OnAction = 0,
		OnEnter,
		OnLeave,
		OnGameStart,
		OnGameEnd,
		OnLevelStart001,
		OnZoneStart001,
		OnZoneEnd001,
		OnTimeout,
		OntTimerStart,

		OnPlayerDead,
		OnInterruptHit,
		OnStartReceiveHit,
		OnEndReceiveHit,

		OnEmitParticles,
		OnChangePolarity,
		OnPickupBox,
		OnLeaveBox,
		OnPossess,
		OnUnpossess,
		OnDash,
		OnBlink,
		OnBreakWall,
		OnUseCable,
		OnStun,
		OnStunned,
		OnLiquid,
		OnBeingAttracted,
		OnOvercharge,
		OnDoubleJump,
		OnDetected,
		OnBeaconDetect,

		OnEnterPC,
		OnLeavePC
	};

	void init();
	void throwEvent(EVENT evt/*, params*/);
	void throwUserEvent(std::string evt/*, params*/);
	void shutDown();

	void bind(SLB::Manager& m);
};

#endif
