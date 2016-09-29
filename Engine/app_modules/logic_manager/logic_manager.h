#ifndef INC_LOGIC_MANAGER_H_
#define	INC_LOGIC_MANAGER_H_

#include "app_modules/app_module.h"
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "SLB\SLB.hpp"

struct command {
	const char* code;
	float execution_time;
	bool only_runtime;
};

class CLogicManagerModule : public IAppModule
{
	std::string lua_script_folder = "data/lua_scripts";

	std::deque<command> command_queue;
	std::deque<command> command_queue_to_add;

	CHandle caller_handle;

public:

	SLB::Manager* slb_manager;
	SLB::Script slb_script;

	enum EVENT {
		OnAction = 0,
		OnActionSci,
		OnActionMole,
		OnActionPila,
		OnEnter,
		OnLeave,
		OnGameStart,
		OnGameEnd,
		OnLevelStart,
		OnZoneStart001,
		OnZoneEnd001,
		OnTimeout,
		OntTimerStart,
		OnSetLight,

		OnGuardChase,
		OnGuardChaseEnd,
		OnGuardAttack,
		OnGuardAttackEnd,
		OnGuardRemoveBox,
		OnGuardOvercharged,
		OnGuardBoxHit,
		OnGuardMoving,
		OnGuardMovingStop,
		OnInterruptHit,
		OnStartReceiveHit,
		OnEndReceiveHit,

		OnEmitParticles,
		OnChangePolarity,
		OnPickupBox,
		OnPushBox,
		OnPushBoxIdle,
		OnLeaveBox,
		OnPossess,
		OnUnpossess,
		OnDash,
		OnBlink,
		OnBreakWall,
		OnDroneMoving,
		OnDroneStatic,
		OnRechargeDrone,
		OnNotRechargeDrone,
		OnUseWorkbench,
		OnRepairDrone,
		OnCreateBomb,
		OnUseCable,
		OnUseGenerator,
		OnStun,
		OnStunned,
		OnStunnedEnd,
		OnLiquid,
		OnBeingAttracted,
		OnOvercharge,
		OnJump,
		OnJumpLand,
		OnDoubleJump,
		OnDetected,
		OnNextPatrol,
		OnBeaconDetect,
		OnExplode,

		OnDoorOpening,
		OnDoorOpened,
		OnDoorClosing,
		OnDoorClosed,

		OnEnterPC,
		OnLeavePC,

		OnCinematicEnd,
		OnCinematicSkipped,

		OnElevatorUp,
		OnElevatorDown,
		OnElevatorGoingUp,
		OnElevatorGoingDown,

		OnPutPila,
		OnRemovePila,

		OnVictory,
		OnDead,
		OnRestartLevel,
		OnLoadingLevel,
		OnLoadedLevel,
		OnSavedLevel,

		//Others
		OnStep,
		OnStepOut,

		//GUI
		OnButtonPressed,
		OnClicked,
		OnMouseOver,
		OnMouseUnover,
		OnPause,
		OnCreateGui,
		OnChoose,
	};

	CLogicManagerModule();
	bool start() override;
	void update(float dt) override;
	void resetTimers();
	void reloadFile(std::string filename);
	void stop() override;
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "logic_manager";
	}

	// internal functions
	std::deque<command>* getCommandQueue() {
		return &command_queue;
	}

	void addCommand(command com) {
		command_queue_to_add.push_back(com);
	}

	// module specific functions
	void throwEvent(EVENT evt, std::string params, CHandle handle = CHandle());//, uint32_t handle_id = 0);
	void throwUserEvent(std::string evt, std::string params = "", CHandle handle = CHandle());//, uint32_t handle_id = 0);
	void bindPosition(SLB::Manager& m);
	void bindPlayer(SLB::Manager& m);
	void bindHandle(SLB::Manager& m);
	void bindHandleGroup(SLB::Manager& m);
	void bindCamera(SLB::Manager& m);
	void bindUiCamera(SLB::Manager& m);
	void bindData(SLB::Manager& m);
	void bindPublicFunctions(SLB::Manager& m);
	CHandle getCaller() { return caller_handle; }
};

extern CLogicManagerModule* logic_manager;

#endif
