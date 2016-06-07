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
};

class CLogicManagerModule : public IAppModule
{
  std::string lua_script_folder = "data/lua_scripts";

  std::deque<command> command_queue;

  CHandle caller_handle;

public:

  SLB::Manager* slb_manager;
  SLB::Script slb_script;

  enum EVENT {
    OnAction = 0,
    OnActionSci,
    OnActionMole,
    OnEnter,
    OnLeave,
    OnGameStart,
    OnGameEnd,
    OnLevelStart,
    OnZoneStart001,
    OnZoneEnd001,
    OnTimeout,
    OntTimerStart,

    OnPlayerDead,
    OnGuardAttack,
    OnGuardAttackEnd,
    OnGuardRemoveBox,
    OnGuardOvercharged,
    OnGuardBoxHit,
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
    OnRechargeDrone,
    OnNotRechargeDrone,
    OnUseCable,
    OnUseGenerator,
    OnStun,
    OnStunned,
    OnStunnedEnd,
    OnLiquid,
    OnBeingAttracted,
    OnOvercharge,
    OnJump,
    OnDoubleJump,
    OnDetected,
    OnBeaconDetect,

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

  // module specific functions
  void throwEvent(EVENT evt, std::string params, CHandle handle = CHandle());//, uint32_t handle_id = 0);
  void throwUserEvent(std::string evt, std::string params = "", CHandle handle = CHandle());//, uint32_t handle_id = 0);
  void bindPlayer(SLB::Manager& m);
  void bindHandle(SLB::Manager& m);
  void bindHandleGroup(SLB::Manager& m);
  void bindCamera(SLB::Manager& m);
  void bindPublicFunctions(SLB::Manager& m);
  CHandle getCaller() { return caller_handle; }
};

extern CLogicManagerModule* logic_manager;

#endif
