print('This is lua')
print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle()
pl = Player()
cam = Camera()
ui_cam = UiCamera()

function CallFunction(func)
	called = true
	if _G[func] then _G[func]()
	else
		p:print("function "..func.." does not exist!\n")
		called = false
	end
	return called
end

function CallFunctionParam(func, param)
	called = true
	if _G[func] then _G[func](param)
	else
		p:print("function "..func.." does not exist!\n")
		called = false
	end
	--local called = pcall(func, param)
	return called
end

function OnAction( param )
	p:print( "OnAction: "..param.."\n" )
	CallFunction("OnAction_"..param)
end

function OnActionSci( param )
	p:print( "OnActionSci: "..param.."\n" )
	CallFunction("OnActionSci_"..param)
end

function OnActionMole( param )
	p:print( "OnActionMole: "..param.."\n" )
	CallFunction("OnActionMole_"..param)
end

function OnEnter( param )
	p:print( "OnEnter: "..param.."\n" )
	CallFunction("OnEnter_"..param)
end

function OnLeave( param )
	p:print( "OnLeave: ".. param.."\n" )
	CallFunction("OnLeave_"..param)
end

function OnGameStart( param )
	p:print( "OnGameStart: "..param.."\n" )
	cam:fx(FX_FOG, 0)
	cam:fx(FX_DREAM_BORDER, 0)
	p:load_entities("init")
	ui_cam:fade_out(0)
	p:exec_command("LoadLevel(\"level_0\")", 2)
	--Sound
	val = p:json_read(FILE_OPTIONS, "sound", "music")
	p:set_music_volume(val)
	val = p:json_read(FILE_OPTIONS, "sound", "sfx")
	p:set_sfx_volume(val)
end

function OnGameEnd( param )
	p:print( "OnGameEnd: "..param.."\n" )
end

function OnGuardChase( volume )
	p:print( "OnGuardChase: "..volume.."\n" )
	p:play_music("event:/OnChaseMusic", volume)
end

function OnGuardChaseEnd( volume )
	p:print( "OnGuardChaseEnd: "..volume.."\n" )
	p:play_music("event:/OnGameMusic", volume)
end

function OnGuardAttack( reaction_time )
	p:print( "OnGuardAttack: "..reaction_time.."\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnGuardAttack", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnGuardAttackPrep( reaction_time )
	p:print( "OnGuardAttackPrep: "..reaction_time.."\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnGuardAttackPrep", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
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
	p:play_sound("event:/OnGuardMoving", 1.0, false)
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
	h:getHandleCaller()
	p:play_3d_sound("event:/OnChangePolarity", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 8)
end

function OnPickupBox( param )
	p:print( "OnPickupBox: "..param.."\n" )
end

function OnPushBox( param )
	p:print( "OnPushBox: "..param.."\n" )
	h:getHandleCaller()
	p:play_3d_sound("event:/OnPushPullBox", h:get_x(), h:get_y(), h:get_z(), 1.0, true, 1)
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
	OnStunnedEnd(pj)
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
	p:play_sound("event:/OnBreakWall", 1.0, false)
	cam:start_vibration(0.0, 0.75, 10)
	p:exec_command("cam:stop_vibration(8)", 1.0)
end

function OnDroneMoving( sound_name )
	p:print( "OnDroneMoving: "..sound_name.."\n" )
	h:getHandleCaller()	
	p:play_fixed_3d_sound("event:/OnDroneMoving", sound_name, h:get_x(), h:get_y(), h:get_z(), 1.0, true)
end

function OnDroneStatic( sound_name )
	p:print( "OnDroneStatic: "..sound_name.."\n" )
	h:getHandleCaller()	
	p:play_fixed_3d_sound("event:/OnDroneStatic", sound_name, h:get_x(), h:get_y(), h:get_z(), 1.0, true)
end

function OnUseWorkbench( param )
	p:print( "OnUseWorkbench: "..param.."\n" )
	p:play_sound("event:/OnLaboratory", 1.0, false)
end

function OnRechargeDrone( param )
	p:print( "OnRechargeDrone: "..param.."\n" )
	p:play_sound("event:/OnUseGenerator", 1.0, false)
end

function OnRepairDrone( level, drone )
	p:print( "OnRepairDrone: "..level..", "..drone.."\n" )
	p:play_sound("event:/OnUseGenerator", 1.0, false)
	CallFunction("OnRepairDrone_"..level)
	CallFunction("OnRepairDrone_"..drone)
end

function OnCreateBomb( level )
	p:print( "OnCreateBomb: "..level)
	CallFunction( "OnCreateBomb_"..level)
end

function OnNotRechargeDrone( param )
	p:print( "OnNotRechargeDrone: "..param.."\n" )
	p:player_talks("I hope a scientific may get this repaired...")
end

function OnUseCable( param )
	p:print( "OnUseCable: "..param.."\n" )
end

function OnSenseVision( level )
	p:print( "OnSenseVision: "..level.."\n" )
	CallFunction(level.."_sense_pressed")
end

function OnUseGenerator( param )
	p:print( "OnUseGenerator: "..param.."\n" )
	CallFunction("OnUseGenerator_"..param)
	p:play_sound("event:/OnRaijinRecharge", 1.0, false)
end

function OnStun( param )
	p:print( "OnStun: "..param.."\n" )
end

--STUNNED_SIZE = 30
STUNNED_PART = {}
--STUNNED_NEXT = 1
function OnStunned( pj )
	p:print( "OnStunned: "..pj.."\n" )
	p:play_sound("event:/OnStunned", 1.0, false)
	CallFunction( "OnStunned_"..pj)
	local npc = Handle()
	npc:getHandleCaller()
	local x = npc:get_x()
	local y = npc:get_y()
	local z = npc:get_z()
	
	--Offset depends on npc
	local offset_y = 0.6
	local offset_front = 0.1
	if string.match(pj, "mole") then
		offset_y = 0
		offset_front = 0.8
	else
		if string.match(pj, "sci") then
			offset_y = 0.3
			offset_front = 0.4
		end
	end
	
	--Apply Offset
	y = y + offset_y
	local front = npc:get_front()
	x = x + (front:x() * offset_front)
	y = y + (front:y() * offset_front)
	z = z + (front:z() * offset_front)
	
	-- Destroy and create particle
	if STUNNED_PART[pj] ~= nil then
		STUNNED_PART[pj]:destroy() -- Por si acaso ya existia
	end
	STUNNED_PART[pj] = p:create(STUNT_1, x, y, z)
	p:exec_command("STUNNED_PART[\""..pj.."\"]:part_on()", 1)
end

function OnStunnedEnd( pj )
	p:print( "OnStunnedEnd: "..pj.."\n" )
	if STUNNED_PART[pj] ~= nil then
		STUNNED_PART[pj]:destroy()
	end
end

function OnLiquid( param )
	p:print( "OnLiquid: "..param.."\n" )
end

function OnBeingAttracted( param )
	p:print( "OnBeingAttracted: "..param.."\n" )
end

function OnOvercharge( param )
	p:print( "OnOvercharge: "..param.."\n" )
	p:play_sound("event:/OnOvercharge", 1.0, false)
	specialActionSettings(0.7);
	local raijin = Player()
	raijin:get_player()
	local x = raijin:get_x()
	local y = raijin:get_y() - 0.5
	local z = raijin:get_z()
	oc_part = p:create(OVERCHARGE_1, x, y, z)
	--oc_part = p:create(STUNT_1, x, y, z)
	oc_part:part_on()
	p:exec_command("oc_part:destroy()", 5)
end

function OnJump( param )
	p:print( "OnJump: "..param.."\n" )
	p:play_sound("event:/OnJump", 1.0, false)
end

function OnDoubleJump( param )
	p:print( "OnDoubleJump: "..param.."\n" )
	p:play_sound("event:/OnDoubleJump", 1.0, false)
end

function OnJumpLand( param )
	p:print( "OnJump: "..param.."\n" )
	p:stop_sound("event:/OnJump")
	p:stop_sound("event:/OnDoubleJump")
end

function OnMoleJump( param )
	p:print( "OnMoleJump: "..param.."\n" )
	p:play_sound("event:/OnMoleJumpVoice", 1.0, false)
end

function OnJumpLandMoleBaldosa( param )
	p:print( "OnJumpLandMoleBaldosa: "..param.."\n" )
	p:play_sound("event:/OnMoleJump", 1.0, false)
	cam:start_vibration(0.0, 0.75, 15)
	p:exec_command("cam:stop_vibration(10)", 0.3)
end

function OnJumpLandMoleParquet( param )
	p:print( "OnJumpLandScientistParquet: "..param.."\n" )
	p:play_sound("event:/OnMoleJumpParquet", 1.0, false)
	cam:start_vibration(0.0, 0.75, 15)
	p:exec_command("cam:stop_vibration(10)", 0.3)
end

function OnScientistJump( param )
	p:print( "OnScientistJump: "..param.."\n" )
	p:play_sound("event:/OnScientistJumpVoice", 1.0, false)
end

function OnJumpLandScientistBaldosa( param )
	p:print( "OnJumpLandScientistBaldosa: "..param.."\n" )
	p:play_sound("event:/OnScientistJumpBaldosa", 1.0, false)
end

function OnJumpLandScientistParquet( param )
	p:print( "OnJumpLandScientistParquet: "..param.."\n" )
	p:play_sound("event:/OnScientistJumpParquet", 1.0, false)
end

function OnDetected( distance )
	p:print( "OnDetected: "..distance.."\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnDetected", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 32)
	name_guard = h:get_name()
	CallFunction("OnDetected_"..name_guard)
	p:character_globe("ui/effects/bafarada", distance, h:get_x(), h:get_y() + 1.5, h:get_z(), 2.0, -1.0)
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

--Lights
function OnSetLight( volume )
	h:getHandleCaller()
	p:play_3d_sound("event:/OnFluoriscent", h:get_x(), h:get_y(), h:get_z(), volume, false, 32)
end

--Bombs
--------------------------------
function OnExplode( param )
	p:print( "OnExplode\n")
	h:getHandleCaller()
	CallFunction("OnExplode_"..param)
end

BOMB_PART = {}
BOMB_PART_NEXT = 1
BOMB_PART_MAX = 5
function OnExplode_throw_bomb()
	p:print( "OnExplode_throw_bomb\n")
	local bomb = Handle()
	bomb:getHandleCaller()
	local x = bomb:get_x()
	local y = bomb:get_y()
	local z = bomb:get_z()
	BOMB_PART[BOMB_PART_NEXT] = p:create(SMOKE_1, x, y, z)
	BOMB_PART[BOMB_PART_NEXT]:part_on()
	p:exec_command("BOMB_PART["..BOMB_PART_NEXT.."]:destroy()", 2)
	boom_effect = p:character_globe(BOOM, "1.0", x, y, z, 0.5, 20.0)
	boom_effect:set_size(3.0)
	--p:exec_command("explosion_particle:part_on()", 1)
	--explosion_particle:part_on()
	if BOMB_PART_NEXT > BOMB_PART_MAX - 1 then
		BOMB_PART_NEXT = 1
	else
		BOMB_PART_NEXT = BOMB_PART_NEXT + 1
	end
	p:play_3d_sound("event:/OnBombExplodes", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 32)
	cam:start_vibration(0.5, 2.0, 20)
	p:exec_command("cam:stop_vibration(10)", 1.0)
end


--Elevator
--------------------------------
function OnElevatorUp( param )
	p:print( "OnElevatorUp\n")
	p:stop_sound("event:/OnElevatorMoving")
	p:play_sound("event:/OnElevatorArrived", 1.0, false)
	CallFunction("OnElevatorUp_"..param)
end

function OnElevatorDown( param )
	p:print( "OnElevatorDown\n")
	p:stop_sound("event:/OnElevatorMoving")
	p:play_sound("event:/OnElevatorArrived", 1.0, false)
	CallFunction("OnElevatorDown_"..param)
end

function OnElevatorGoingUp( param )
	p:print( "OnElevatorGoingUp\n")
	p:play_sound("event:/OnElevatorMoving", 1.0, false)
	CallFunction("OnElevatorGoingUp_"..param)
end

function OnElevatorGoingDown( param )
	p:print( "OnElevatorGoingDown\n")
	p:play_sound("event:/OnElevatorMoving", 1.0, false)
	CallFunction("OnElevatorGoingDown_"..param)
end

--Pila
--------------------------------
function OnPutPila( param )
	p:print( "OnPutPila\n")
	CallFunction("OnPutPila_"..param)
	p:play_sound("event:/OnPutPila", 1.0, false)
end

function OnBoxMode( level )
	p:print( "OnBoxMode\n")
	CallFunction("OnBoxMode_"..level)
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

--Game Ending
---------------------------------------------------
function OnVictory( )
	p:print( "OnVictory\n")
	launchVictoryState();
end

function OnDead(level)
	p:print( "OnDead\n")
	g_dead = true
	special_death = CallFunction("OnDead_"..level)
	if not special_death then
		pl:unpossess()
		cam:orbit(true)
		p:exec_command("p:load_entities(\"dead_menu\");", 2.0)
		p:exec_command("cam:orbit(false)", 3.0)
	end
end

-- Others
-------------------------------------------
function OnStepGuardBaldosa( step, x, y, z )
	h:getHandleCaller()
	sound_route = ""
	
	if step == 0 then
		sound_route = "event:/OnGuardStepLeft1"
	elseif step == 1 then
		sound_route = "event:/OnGuardStepRight1"
	elseif step == 2 then
		sound_route = "event:/OnGuardStepLeft2"
	elseif step == 4 then
		sound_route = "event:/OnGuardStepRight2"
	else
		sound_route = "event:/OnGuardStepLeft1"
	end
	
	p:play_3d_sound(sound_route, x, y, z, 1.0, false, 32)
	p:character_globe("ui/effects/tap", "1.0", x, y, z, 0.5, 20.0)
end

function OnStepMoleBaldosa( step, x, y, z )
	h:getHandleCaller()
	sound_route = ""
	
	if step == 0 then
		sound_route = "event:/OnMoleStepLeft1"
	elseif step == 1 then
		sound_route = "event:/OnMoleStepRight1"
	elseif step == 2 then
		sound_route = "event:/OnMoleStepLeft2"
	elseif step == 4 then
		sound_route = "event:/OnMoleStepRight2"
	else
		sound_route = "event:/OnMoleStepLeft1"
	end
	
	p:play_3d_sound(sound_route, x, y, z, 1.0, false, 32)
end

function OnStepScientistBaldosa( step, x, y, z )
	h:getHandleCaller()
	sound_route = ""
	
	if step == 0 then
		sound_route = "event:/OnScientistStepBaldosaL1"
	elseif step == 1 then
		sound_route = "event:/OnScientistStepBaldosaR1"
	elseif step == 2 then
		sound_route = "event:/OnScientistStepBaldosaL2"
	elseif step == 4 then
		sound_route = "event:/OnScientistStepBaldosaR1"
	else
		sound_route = "event:/OnScientistStepBaldosaL1"
	end
	
	p:play_3d_sound(sound_route, x, y, z, 1.0, false, 32)
end

function OnStepGuardParquet( step, x, y, z )
	h:getHandleCaller()
	sound_route = ""
	
	if step == 0 then
		sound_route = "event:/OnGuardStepParquetL1"
	elseif step == 1 then
		sound_route = "event:/OnGuardStepParquetR1"
	elseif step == 2 then
		sound_route = "event:/OnGuardStepParquetL2"
	elseif step == 4 then
		sound_route = "event:/OnGuardStepParquetR2"
	else
		sound_route = "event:/OnGuardStepParquetL1"
	end
	
	p:play_3d_sound(sound_route, x, y, z, 1.0, false, 32)
	p:character_globe("ui/effects/tap", "1.0", x, y, z, 0.5, 20.0)
end

function OnStepMoleParquet( step, x, y, z )
	h:getHandleCaller()
	sound_route = ""
	
	if step == 0 then
		sound_route = "event:/OnMoleStepParquetL1"
	elseif step == 1 then
		sound_route = "event:/OnMoleStepParquetR1"
	elseif step == 2 then
		sound_route = "event:/OnMoleStepParquetL2"
	elseif step == 4 then
		sound_route = "event:/OnMoleStepParquetR2"
	else
		sound_route = "event:/OnMoleStepParquetL1"
	end
	
	p:play_3d_sound(sound_route, x, y, z, 1.0, false, 32)
end

function OnStepScientistParquet( step, x, y, z )
	h:getHandleCaller()
	sound_route = ""
	
	if step == 0 then
		sound_route = "event:/OnScientistStepParquetL1"
	elseif step == 1 then
		sound_route = "event:/OnScientistStepParquetR1"
	elseif step == 2 then
		sound_route = "event:/OnScientistStepParquetL2"
	elseif step == 4 then
		sound_route = "event:/OnScientistStepParquetR1"
	else
		sound_route = "event:/OnScientistStepParquetL1"
	end
	
	p:play_3d_sound(sound_route, x, y, z, 1.0, false, 32)
end

function OnStepOutGuardBaldosa( step )
	--h:getHandleCaller()
	--sound_route = ""
	
	--if step == 0 then
		--sound_route = "event:/OnGuardStepLeft1"
	--elseif step == 1 then
		--sound_route = "event:/OnGuardStepRight1"
	--elseif step == 2 then
		--sound_route = "event:/OnGuardStepLeft2"
	--elseif step == 4 then
		--sound_route = "event:/OnGuardStepRight2"
	--else
		--sound_route = "event:/OnGuardStepLeft1"
	--end
	
	--p:stop_sound(sound_route)
end

function OnStepOutGuardParquet( step )
	-- p:print("StepOutGuardParquet")
end

function OnStepOutMoleBaldosa( step )
	-- p:print("StepOutMole")
end

function OnStepOutMoleParquet( step )
	-- p:print("StepOutMole")
end

function OnStepOutScientistBaldosa( step )
	-- p:print("StepOutScientist")
end

function OnStepOutScientistParquet( step )
	-- p:print("StepOutScientist")
end

function OnStartVibration( param )
	cam:start_vibration(0.0, 0.75, 9)
end

function OnStopVibration( param )
	cam:stop_vibration(7.5)
end

-- Voices
function OnScientistVoice1( )
	p:print( "OnScientistVoice1\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnScientistVoice1", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnScientistVoice2( )
	p:print( "OnScientistVoice2\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnScientistVoice2", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnScientistVoice3( )
	p:print( "OnScientistVoice3\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnScientistVoice3", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnScientistVoice4( )
	p:print( "OnScientistVoice4\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnScientistVoice4", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnMoleVoice1( )
	p:print( "OnMoleVoice1\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnMoleVoice1", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnMoleVoice2( )
	p:print( "OnMoleVoice2\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnMoleVoice2", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnMoleVoice3( )
	p:print( "OnMoleVoice3\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnMoleVoice3", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnMoleVoice4( )
	p:print( "OnMoleVoice4\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnMoleVoice4", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnGuardVoice1( )
	p:print( "OnGuardVoice1\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnGuardVoice1", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnGuardVoice2( )
	p:print( "OnGuardVoice2\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnGuardVoice2", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnGuardVoice3( )
	p:print( "OnGuardVoice3\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnGuardVoice3", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnGuardVoice4( )
	p:print( "OnGuardVoice4\n" )
	h:getHandleCaller()	
	p:play_3d_sound("event:/OnGuardVoice4", h:get_x(), h:get_y(), h:get_z(), 1.0, false, 1)
end

function OnTest( )
	p:print("Hola")
	explosion_particle = p:create(SMOKE_1, pl:get_x(), pl:get_y(), pl:get_z())
	--p:exec_command("explosion_particle:destroy()", 5)
end