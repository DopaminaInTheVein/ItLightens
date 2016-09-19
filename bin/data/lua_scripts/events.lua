print('This is lua')
print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle()
pl = Player()
cam = Camera()
ui_cam = UiCamera()

function CallFunction(func)
	if _G[func] then _G[func]()
	else
		p:print("function "..func.." does not exist!\n")
	end
end

function CallFunctionParam(func, param)
	if _G[func] then _G[func](param)
	else
		p:print("function "..func.." does not exist!\n")
	end
end

function OnAction( param )
	p:print( "OnAction: "..param.."\n" )
	CallFunction("OnAction_"..param)
	--specialActionSettings(0.7);
	--setCameraRotationSensibility(60.5);
	--setCameraSpeed(4.0);
	--setCameraSpeedUnlocked(10.0);
	--triggerScientistBusy();
end

function OnActionSci( param )
	p:print( "OnActionSci: "..param.."\n" )
	CallFunction("OnActionSci_"..param)
	--specialActionSettings(0.7);
	--setCameraRotationSensibility(60.5);
	--setCameraSpeed(4.0);
	--setCameraSpeedUnlocked(10.0);
end

function OnActionMole( param )
	p:print( "OnActionMole: "..param.."\n" )
	CallFunction("OnActionMole_"..param)
	--specialActionSettings(0.7);
	--setCameraRotationSensibility(60.5);
	--setCameraSpeed(4.0);
	--setCameraSpeedUnlocked(10.0);
end

function OnEnter( param )
	p:print( "OnEnter: "..param.."\n" ) 
	CallFunction("OnEnter_"..param)
	--if _G["OnEnter_"..param] then _G["OnEnter_"..param](handle) 
	--else
	--	p:print("function OnEnter_"..param.." does not exist!\n")
	--end
end

function OnLeave( param )
	p:print( "OnLeave: ".. param.."\n" )
	CallFunction("OnLeave_"..param)
end

function OnGameStart( param )
	p:print( "OnGameStart: "..param.."\n" )
	p:load_entities("init")
	p:exec_command("LoadLevel(\"level_0\")", 2)
	--p:load_entities("title")
	CallFunction("test_dbg")
end

function OnGameEnd( param )
	p:print( "OnGameEnd: "..param.."\n" )
end

function OnGuardAttack( reaction_time )
	p:print( "OnGuardAttack: "..reaction_time.."\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnGuardAttack", pl:get_x(), pl:get_y(), pl:get_z(), h:get_x(), h:get_y(), h:get_z())
end

function OnGuardAttackEnd( reaction_time )
	p:print( "OnGuardAttack: "..reaction_time.."\n" )
	p:stop_sound("event:/OnGuardAttack")
end

function OnGuardOvercharged( param )
	p:print( "OnGuardOvercharged: "..param.."\n" )
end

function OnGuardBoxHit( param )
	p:print( "OnGuardBoxHit: "..param.."\n" )
end

function OnGuardRemoveBox( reaction_time )
	p:print( "OnGuardRemoveBox: "..reaction_time.."\n" )
end

function OnGuardMoving( reaction_time )
	p:print( "OnGuardMoving: "..reaction_time.."\n" )
	p:play_sound("event:/OnGuardMoving")
end

function OnGuardMovingStop( reaction_time )
	p:print( "OnGuardMovingStop: "..reaction_time.."\n" )
	p:stop_sound("event:/OnGuardMoving")
end

function OnZoneStart001( param )
	p:print( "OnZoneStart001: "..param.."\n" )
end

function OnZoneEnd001( param )
	p:print( "OnZoneEnd001: "..param.."\n" )
end

function OnTimeout( param )
	p:print( "OnTimeout: "..param.."\n" )
end

function OntTimerStart( param )
	p:print( "OntTimerStart: "..param.."\n" )
end

function OnPlayerDead( param )
	p:print( "OnPlayerDead: "..param.."\n" )
end

function OnInterruptHit( param )
	p:print( "OnInterruptHit: "..param.."\n" )
end

function OnStartReceiveHit( param )
	p:print( "OnStartReceiveHit: "..param.."\n" )
end

function OnEndReceiveHit( param )
	p:print( "OnEndReceiveHit: "..param.."\n" )
end

function OnEmitParticles( param )
	p:print( "OnEmitParticles: "..param.."\n" )
end

function OnChangePolarity( param )
	p:print( "OnChangePolarity: "..param.."\n" )
	p:play_sound("event:/OnChangePolarity")
end

function OnPickupBox( param )
	p:print( "OnPickupBox: "..param.."\n" )
end

function OnPushBox( param )
	p:print( "OnPushBox: "..param.."\n" )
  	p:play_sound("event:/OnPushPullBox")
end

function OnPushBoxIdle( param )
	p:print( "OnPushBoxIdle: "..param.."\n" )
  	p:stop_sound("event:/OnPushPullBox")
end

function OnLeaveBox( param )
	p:print( "OnLeaveBox: "..param.."\n" )
	p:stop_sound("event:/OnPushPullBox")
end

function OnPossess( level, pj )
	p:print( "OnPossess: "..level..", "..pj.."\n" )
	CallFunction("OnPossess_"..level)
	CallFunction("OnPossess_"..pj)
end

function OnUnpossess( level, pj )
	p:print( "OnUnpossess: "..level..", "..pj.."\n" )
	CallFunction("OnUnpossess_"..level)
	CallFunction("OnUnpossess_"..pj)
end

function OnDash( param )
	p:print( "OnDash: "..param.."\n" )
end

function OnBlink( param )
	p:print( "OnBlink: "..param.."\n" )
end

function OnBreakWall( param )
	p:print( "OnBreakWall: "..param.."\n" )
	p:play_sound("event:/OnBreakWall")
end

function OnRechargeDrone( param )
	p:print( "OnRechargeDrone: "..param.."\n" )
	p:play_sound("event:/OnUseGenerator")
end

function OnRepairDrone( level, drone )
	p:print( "OnRepairDrone: "..level..", "..drone.."\n" )
	p:play_sound("event:/OnUseGenerator")
	CallFunction("OnRepairDrone_"..level)
	CallFunction("OnRepairDrone_"..drone)
	--p:player_talks("I just repaired that useful thing to make a full working one...\nbut battery may fail as well, that is unreparable....")
end

function OnCreateBomb( level )
	p:print( "OnCreateBomb: "..level)
	CallFunction( "OnCreateBomb_"..level)
end

function OnNotRechargeDrone( param )
	p:print( "OnRechargeDrone: "..param.."\n" )
	p:player_talks("I hope a scientific may get this repaired...")
end

function OnUseCable( param )
	p:print( "OnUseCable: "..param.."\n" )
end

function OnUseGenerator( param )
	p:print( "OnUseGenerator: "..param.."\n" )
	CallFunction("OnUseGenerator_"..param)
	p:play_sound("event:/OnUseGenerator")
end

function OnStun( param )
	p:print( "OnStun: "..param.."\n" )
end

function OnStunned( pj )
	p:print( "OnStunned: "..pj.."\n" )
	p:play_ambient("event:/OnStunned")
	CallFunction( "OnStunned_"..pj)
end

function OnStunnedEnd( param )
	p:print( "OnStunnedEnd: "..param.."\n" )
end

function OnLiquid( param )
	p:print( "OnLiquid: "..param.."\n" )
end

function OnBeingAttracted( param )
	p:print( "OnBeingAttracted: "..param.."\n" )
end

function OnOvercharge( param )
	p:print( "OnOvercharge: "..param.."\n" )
	p:play_sound("event:/OnOvercharge")
	specialActionSettings(0.7);
end

function OnJump( param )
	p:print( "OnDoubleJump: "..param.."\n" )
	p:play_sound("event:/OnJump")
end

function OnDoubleJump( param )
	p:print( "OnDoubleJump: "..param.."\n" )
	p:play_sound("event:/OnDoubleJump")
end

function OnDetected( distance )
	p:print( "OnDetected: "..distance.."\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnDetected", pl:get_x(), pl:get_y(), pl:get_z(), h:get_x(), h:get_y(), h:get_z())
	name_guard = h:get_name()
	CallFunction("OnDetected_"..name_guard)
	p:character_globe(distance, h:get_x(), h:get_y(), h:get_z())
end

function OnNextPatrol( guard_name )
	p:print ("OnNextPatrol: "..guard_name.."\n")
	CallFunction("OnNextPatrol_"..guard_name)
end

function OnBeaconDetect( param )
	p:print( "OnBeaconDetect: "..param.."\n" )
end

function OnEnterPC( param )
	p:print( "OnEnterPC: "..param.."\n" )
end

function OnLeavePC( param )
	p:print( "OnLeavePC: "..param.."\n" )
end

function OnDoorClosed()
	p:print( "OnDoorClosed\n")
end

function OnDoorOpened()
	p:print( "OnDoorOpened\n")
end

function OnDoorClosing()
	p:print( "OnDoorClosing\n")
end

function OnDoorOpening()
	p:print( "OnDoorOpening\n")
end

--Bombs
--------------------------------
function OnExplode( param )
	p:print( "OnExplode\n")
	CallFunction("OnExplode_"..param)
end

function OnExplode_throw_bomb()
	p:print( "OnExplode_throw_bomb\n")
	p:play_sound("event:/OnChangePolarity")
end


--Elevator
--------------------------------
function OnElevatorUp( param )
	p:print( "OnElevatorUp\n")
	CallFunction("OnElevatorUp_"..param)
end

function OnElevatorDown( param )
	p:print( "OnElevatorDown\n")
	CallFunction("OnElevatorDown_"..param)
end

function OnElevatorGoingUp( param )
	p:print( "OnElevatorGoingUp\n")
	CallFunction("OnElevatorGoingUp_"..param)
end

function OnElevatorGoingDown( param )
	p:print( "OnElevatorGoingDown\n")
	CallFunction("OnElevatorGoingDown_"..param)
end

--Pila
--------------------------------
function OnPutPila( param )
	p:print( "OnPutPila\n")
	CallFunction("OnPutPila_"..param)
	p:play_sound("event:/OnPutPila")
end

function OnRemovePila( param )
	p:print( "OnRemovePila\n")
	CallFunction("OnRemovePila_"..param)
end

--Cinematics
---------------------------------------------------
function OnCinematicSkipped( param )
	p:print( "OnCinematicSkipped\n")
	CallFunction("OnCinematicSkipped_"..param)
end

function OnCinematicEnd( param )
	p:print( "OnCinematicEnd\n")
	CallFunction("OnCinematicEnd_"..param)
end

--Game Restart
---------------------------------------------------
function OnRestartLevel( logic_level, real_level )
	p:print( "OnRestartLevel\n")
	LoadLevel(logic_level) -- Defined in functions.lua
end

--Levels
---------------------------------------------------
function OnLevelStart( logic_level, real_level )
	p:print("OnLevelStart\n")
	InitScene()
	p:exec_command("CallFunction(\"OnStart_"..real_level.."\");", 1.1)
end

function OnSavedLevel( logic_level, real_level )
	p:print("OnSavedLevel")
	CallFunction("OnSave_"..real_level)	
end

function OnLoadedLevel( logic_level, real_level )
	p:print("OnLoadedLevel")
	InitScene()
	p:exec_command("CallFunction(\"OnLoad_"..real_level.."\");", 1.1)
end

function InitScene()
	cam:reset_camera()
	p:exec_command("ui_cam:fade_in(1)", 1)
	if not real_level == "hub" then
		p:exec_command("p:setControlEnabled(1);", 1)
	end
	if not g_is_menu then
		p:load_entities("player_hud")
	end
end

function OnLoadingLevel()
	p:print("OnLoadingLevel")
	p:show_loading_screen()
end

--Game Ending
---------------------------------------------------
function OnVictory( )
	p:print( "OnVictory\n")
	launchVictoryState();
end

function OnDead( )
	p:print( "OnDead\n")
	p:load_entities("dead_menu")
end

-- Others
-------------------------------------------
function OnStepGuard( )
	--p:print("StepGuard")
end
function OnStepMole( )
	--p:print("StepMole")
end
function OnStepScientist( )
	--p:print("StepScientist")
end
function OnStepOutGuard( )
	-- p:print("StepOutGuard")
end
function OnStepOutMole( )
	-- p:print("StepOutMole")
end
function OnStepOutScientist( )
	-- p:print("StepOutScientist")
end

-- GUI
---------------------------------------------------
function OnCreateGui( param )
	p:print("OnCreateGui: "..param)
	CallFunction("OnCreateGui_"..param)
end
function OnMouseOver( param )
	p:print("OnMouseOver")
	CallFunction("OnMouseOver_"..param)
end

function OnMouseUnover( param )
	p:print("OnMouseUnover")
	CallFunction("OnMouseUnover_"..param)
end

function OnPressed( param )
	p:print("OnPressed")
	CallFunction("OnPressed_"..param)
end

function OnClicked( param )
	p:print("OnClicked")
	CallFunction("OnClicked_"..param)
end

function OnChoose( name, option )
	p:print("OnChoose: "..name.." "..option)
	CallFunctionParam("OnChoose_"..name, option)
end

function OnPause( )
	p:print("OnPause")
	p:load_entities("menu")
end