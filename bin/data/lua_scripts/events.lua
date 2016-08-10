print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle()
cam = Camera()

function CallFunction(func)
	if _G[func] then _G[func]()
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
	p:load_level("level_0")
	CallFunction("test_dbg")
end

function OnGameEnd( param )
	p:print( "OnGameEnd: "..param.."\n" )
end

function OnGuardAttack( reaction_time )
	p:print( "OnGuardAttack: "..reaction_time.."\n" )
	p:play_voice("event:/OnGuardAttack")
end

function OnGuardAttackEnd( reaction_time )
	p:print( "OnGuardAttack: "..reaction_time.."\n" )
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
end

function OnLeaveBox( param )
	p:print( "OnLeaveBox: "..param.."\n" )
end

function OnPossess( param )
	p:print( "OnPossess: "..param.."\n" )
end

function OnUnpossess( param )
	p:print( "OnUnpossess: "..param.."\n" )
end

function OnDash( param )
	p:print( "OnDash: "..param.."\n" )
end

function OnBlink( param )
	p:print( "OnBlink: "..param.."\n" )
end

function OnBreakWall( param )
	p:print( "OnBreakWall: "..param.."\n" )
end

function OnRechargeDrone( param )
	p:print( "OnRechargeDrone: "..param.."\n" )
	p:play_sound("event:/OnUseGenerator")
end

function OnRepairDrone( param )
	p:print( "OnRepairDrone: "..param.."\n" )
	p:play_sound("event:/OnUseGenerator")
	p:player_talks("I just repaired that useful thing to make a full working one...\nbut battery may fail as well, that is unreparable....","scientific.dds","SCI")
end

function OnNotRechargeDrone( param )
	p:print( "OnRechargeDrone: "..param.."\n" )
	p:player_talks("I hope a scientific may get this repaired...","scientific.dds","SCI")
end

function OnUseCable( param )
	p:print( "OnUseCable: "..param.."\n" )
end

function OnUseGenerator( param )
	p:print( "OnUseGenerator: "..param.."\n" )
	p:play_sound("event:/OnUseGenerator")
end

function OnStun( param )
	p:print( "OnStun: "..param.."\n" )
end

function OnStunned( param )
	p:print( "OnStunned: "..param.."\n" )
	p:play_ambient("event:/OnStunned")
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

function OnDetected( distance, posx, posy, posz )
	p:print( "OnDetected: "..distance.." "..posx.." "..posy.." "..posz.."\n" )
	p:play_sound("event:/OnDetected")
	h:getHandleCaller()
	name_guard = h:get_name()
	CallFunction("OnDetected_"..name_guard)
	--p:character_globe("Intruder detected!", distance, posx, posy, posz)
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
	cam:fade_out(1)
	p:setControlEnabled(0)
	p:exec_command("LoadLevel(\""..logic_level.."\");", 1) -- Defined in functions.lua
end

--Levels
---------------------------------------------------
function OnLevelStart( logic_level, real_level )
	p:print("OnLevelStart\n")
	p:exec_command("cam:fade_in(1)", 1)
	p:exec_command("p:setControlEnabled(1);", 1)
	CallFunction("OnStart_"..real_level)
end

function OnSavedLevel( logic_level, real_level )
	p:print("OnSavedLevel")
	CallFunction("OnSave_"..real_level)	
end

function OnLoadedLevel( logic_level, real_level )
	p:print("OnLoadedLevel")
	p:exec_command("cam:fade_in(1)", 1)
	p:exec_command("p:setControlEnabled(1);", 1)
	CallFunction("OnLoad_"..real_level)
end

--Game Ending
---------------------------------------------------
function OnVictory( )
	p:print( "OnVictory\n")
	launchVictoryState();
end
