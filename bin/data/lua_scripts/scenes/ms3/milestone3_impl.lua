--------------------------- COMENTAR CODIGO TEST!!!! ---------------------------------------
function test_dbg()
  --h:get_handle_by_id(idDoor)
  --h:setLocked(0)
  --isDoorOpen = true
end
--------------------------- COMENTAR CODIGO TEST!!!! ---------------------------------------
print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle( )
h_elevator = Handle( )
hg = HandleGroup()
cam = Camera()
player = Player()

---------------------------- LEVEL Init, Load, Save ---------------------------------------------------
function auxiliarMusic()
	p:play_music("event:/OnGameMusic", 0.2)
end

function OnStart_ms3()
  p:print("OnStarted Scene MS3")
  InitFogMs3()
  
  isDoorOpen = false
  alert = false
  stateElevator = 1 -- 1= up , 0 = down
  cp_elevator = false
  cp_door_opened = false
  p:play_music("event:/OnGameMusic", 0.2)
  --p:exec_command("auxiliarMusic();",14)
  --triggerGuardFormation();
  --p:exec_command( "triggerGuardFormation();", 15 )
  p:setControlEnabled(0)
  cam:run_cinematic("CinematicIntro", 10)
  p:ai_start()
end

function OnSave_ms3()
	d = Data()
	d:put_float("elevator_state", stateElevator)
	d:put_bool("door_open", isDoorOpen)
	d:put_bool("alert", alert)
	d:put_bool("cp_elevator", cp_elevator)
	d:put_bool("cp_door_opened", cp_door_opened)
	SaveFogMs3(d)
	d:write()
end

function OnLoad_ms3()
  p:print("OnLoaded Scene MS3")
  d = Data()
  LoadFogMs3()
  isDoorOpen = d:get_bool("door_open")
  alert = d:get_bool("alert")
  stateElevator = d:get_float("elevator_state")
  cp_elevator = d:get_bool("cp_elevator")
  cp_door_opened = d:get_bool("cp_door_opened")
  p:play_music("event:/OnGameMusic", 0.2)
  p:ai_start()
  --triggerGuardFormation();
  --p:exec_command( "triggerGuardFormation();", 15 )
  --cam:run_cinematic("CinematicIntro", 10)
end
---------------------------------------------------------------------------------------------

function OnCinematicEnd_CinematicIntro()
	p:exec_command("p:setControlEnabled(1)", 0.2)
end


function OnEnter_tElevator( )
  --Nothing to do
  --p:player_talks("I'm in an elevator! \nI'm so cool!!! \nI wanna die my hair red!")
end

function OnLeave_tElevator( )
  --Nothing to do
  --p:player_talks_color("I'm out of an elevator! \nI'm not so cool!!! \nI wanna die my hair black!","scientific.dds","SCI", "#FF2222FF","#22FF22FF")
end

-- Creating Bomb
---------------------------------------------------------------------------------------------
function OnCreateBomb_ms3( )
	p:complete_tasklist(11)
end

-- Pila --
--------------------------------------------------------------------------------------------
pila = Handle()
function OnAction___pila( )
  pila:getHandleCaller()
  
  if pila:is_charged() then
	p:player_talks("::pila_1")
  else
    p:player_talks("::pila_2")
  end
  p:exec_command("pila:setActionable(1);", 4.5)
end

function OnActionSci___pila( )
  p:player_talks("::sci_pila")
end

-- Cargador --
---------------------------------------------------------------------------------------------
idCargador = 303
triggerCargador = Handle()
hCargador = Handle()
hCargadorTarget = Handle()
function activateCargadorPlayer()
  hCargador:get_handle_by_id(idCargador)

  if hCargador:has_pila() then
    if hCargador:has_pila_charged() then
		p:player_talks("::cargador_player_1")
	else
		p:player_talks("::cargador_player_2")
		
	end
  else
	p:player_talks("::cargador_player_3")
  end
  
  triggerCargador:getHandleCaller()
  p:exec_command("triggerCargador:setActionable(1);", 4.5)
 
end

function activateCargadorSci()
  hCargador:get_handle_by_id(idCargador)
  if hCargador:has_pila() then
    if hCargador:has_pila_charged() then
		p:player_talks("::cargador_sci_1")
	else
		hCargadorTarget:get_handle_by_name_tag("cargador_bateria_pAction", "target")
		p:player_talks("::cargador_sci_2")
	    sci = Handle()
		sci:get_player()
		sci:go_and_look_as(hCargadorTarget, "rechargeCell();")
		p:complete_tasklist(9)
		p:play_sound("event:/OnChargePila", 0.5, false)
	end
  else
	p:player_talks("::cargador_sci_3")
  end

  triggerCargador:getHandleCaller()
  p:exec_command("triggerCargador:setActionable(1);", 4.5)
end

function rechargeCell()
  p:player_talks("::recharge_cell")
  hCargador:set_charged(1)
end

-- Enchufe --
--------------------------------------------------------------------------------------------
idEnchufe = 304
triggerEnchufe = Handle()
hEnchufe = Handle()
hEnchufeTarget = Handle()

function activateEnchufePlayer()
	triggerEnchufe:getHandleCaller()
	hEnchufe:get_handle_by_id(idEnchufe)

	if hEnchufe:has_pila() then
		if hEnchufe:has_pila_charged() then
			p:player_talks("::enchufe_player_1")
		else
			p:player_talks("::enchufe_player_2")
		end
	else
		p:player_talks("::enchufe_player_3")
	end
	  
	triggerEnchufe:getHandleCaller()
	p:exec_command("triggerEnchufe:setActionable(1);", 4.5)
end

function activateEnchufeSci()
  triggerEnchufe:getHandleCaller()
  hEnchufe:get_handle_by_id(idEnchufe)

  if hEnchufe:has_pila() then
    if hEnchufe:has_pila_charged() then
		p:player_talks("::enchufe_sci_1")
	else
		p:player_talks("::enchufe_sci_2")
	end
  else
	p:player_talks("::enchufe_sci_3")
  end
  
  triggerEnchufe:getHandleCaller()
  p:exec_command("triggerEnchufe:setActionable(1);", 4.5)
end

idDoor = 300
isDoorOpen = false
function OnPutPila_enchufe()
  pila:getHandleCaller()
  if pila:is_charged() then
	--Abril
	isDoorOpen = true
	if not cp_door_opened then
		cp_door_opened = true
		SaveLevel()
		p:complete_tasklist(10)
	end
	if not alert then
		openDoorPila()
	end
  end
end

function OnPutPila_cargador_bateria()
  p:complete_tasklist(6)
end

function cineDoor( )
  p:setControlEnabled(0)
  cam:run_cinematic("CineEnchufeDoor", 10)
  p:exec_command( "ui_cam:fade_out(1);", 0.1)
  p:exec_command( "ui_cam:fade_in(1);", 3)
  p:exec_command( "ui_cam:fade_out(1);", 7)
  p:exec_command( "ui_cam:fade_in(1);", 11)
  p:exec_command( "p:setControlEnabled(1);",11.1)
end

function openDoorPila( )
  cineDoor()
  p:exec_command( "openDoorPilaEffect();", 4)
end

function closeDoorPila( )
  cineDoor()
  p:exec_command( "closeDoorPilaEffect();", 4)
end

function openDoorPilaEffect( )
  h:get_handle_by_id(idDoor)
  h:setLocked(0)
  p:play_sound("event:/OnDoorClosing", 1.0, false)
  p:play_sound("event:/OnFinalAlarm", 0.1, true)
end

function closeDoorPilaEffect( )
  h:get_handle_by_id(idDoor)
  h:setLocked(1)
  p:play_sound("event:/OnDoorClosing", 1.0, false)
  p:stop_sound("event:/OnFinalAlarm")
end

function OnRemovePila_enchufe()
  pila:getHandleCaller()
  p:complete_tasklist(5);
  if pila:is_charged() then
	--Cerral (:P)
	isDoorOpen = false
	if not alert then
		closeDoorPila()
	end
  end
end

-- On Detected Door_centinels --
--------------------------------

hDoor = Handle()
alert = false
centinel_1 = Handle()
centinel_2 = Handle()

function centinelDetection( )
  if not alert then
	alert = true
	hDoor:get_handle_by_id(idDoor)
	p:exec_command("hDoor:setLocked(-10)", 0.5)
	p:exec_command("centinelRelax();", 2)
  end
end

function centinelRelax( )
  p:print("Centinel relax?")
  if alert then
	alert = false
	cent1_ok = centinel_1:is_valid()
	cent2_ok = centinel_2:is_valid()
	if cent1_ok then
		--Is patrolling ?
		patrol_1 = centinel_1:is_patrolling()
		if not patrol_1 then
			alert = true
		else
			--Guard in his position?
			come_back = centinel_1:is_come_back()
			if not come_back then
				alert = true
			end
		end
	end
	  
	if cent2_ok then
		--Is patrolling ?
		patrol_2 = centinel_2:is_patrolling()
		if not patrol_2 then
			alert = true
		else
			--Guard in his position?
			come_back = centinel_2:is_come_back()
			if not come_back then
				alert = true
			end
		end
	end
	  
	if alert then
		p:exec_command("centinelRelax();", 2)
	else
		p:exec_command("alert_finish()", 1)
	end
  end
end

function alert_finish( )
  if not alert then
    if isDoorOpen then
		hDoor:setLocked(0);
	end
  end
end

function OnDetected_guard_004( )
	centinel_1:getHandleCaller()
	centinelDetection()
end

function OnDetected_guard_005( )
	centinel_2:getHandleCaller()
	centinelDetection()
end

-- Elevator --
-------------------------------------------------
idElevator = 301
idDoorElevLeft = 305
idDoorElevRight = 306
idDoorElevLeftUp = 307
idDoorElevRightUp = 308
hDoorElevLeft = Handle()
hDoorElevRight = Handle()
hDoorElevLeftUp = Handle()
hDoorElevRightUp = Handle()
actionElevatorTarget = Handle()

triggerElevator = Handle()
stateElevator = 1 -- 1= up , 0 = down
function activateElevator( )  
  triggerElevator:getHandleCaller()
  --Llevamos el player al punto de accion
  actionElevatorTarget:get_handle_by_name_tag("elevator_pAction", "target")
  player = Handle()
  player:get_player()
  player:go_and_look_as(actionElevatorTarget, "moveElevator();")
end

function moveElevator( )
  p:setControlEnabled(0)
  
  -- Get handles
  h_elevator:get_handle_by_id(idElevator)
  hDoorElevLeftUp:get_handle_by_id(idDoorElevLeftUp)
  hDoorElevRightUp:get_handle_by_id(idDoorElevRightUp)
  hDoorElevLeft:get_handle_by_id(idDoorElevLeft)
  hDoorElevRight:get_handle_by_id(idDoorElevRight)
  
  
  if stateElevator == 1 then
	hDoorElevLeftUp:setLocked(1)
	hDoorElevRightUp:setLocked(1)
	
	p:exec_command("hDoorElevLeft:setLocked(0);", 7)
	p:exec_command("hDoorElevRight:setLocked(0);", 7)
	p:exec_command("checkPointElevator()", 9)
  else
	hDoorElevLeft:setLocked(1)
	hDoorElevRight:setLocked(1)  
	p:exec_command("hDoorElevLeftUp:setLocked(0);", 7)
	p:exec_command("hDoorElevRightUp:setLocked(0);", 7)
  end
  stateElevator = 1 - stateElevator
  
  p:exec_command("h_elevator:activate();", 2)
  p:exec_command("ui_cam:fade_out(1);", 2.5)
  p:exec_command("triggerElevator:setActionable(1);", 4 )
  p:exec_command("ui_cam:fade_in(1);", 7.0)
  p:complete_tasklist(8)
  p:exec_command( "p:setControlEnabled(1);", 9 )
  --cambio  cientifico
  --if elevatorState == 0 then
  --  p:playerRoom("3")
  --else if elevatorState == 2
  --  p:playerRoom("2")
  --end
  --Vibracion de la camara
  p:exec_command("cam:start_vibration(0.0, 0.25, 20)", 2)
  p:exec_command("cam:stop_vibration(18)", 6)
end

function checkPointElevator( )
	if not cp_elevator then
		cp_elevator = true
		SaveLevel()
	end
end

function activateElevatorPlayer( )
  triggerElevator:getHandleCaller()
  p:player_talks("::elevator_player")
  p:exec_command( "triggerElevator:setActionable(1);", 2 )
end
--------------------------------------------------

-- Electric Lock --
--------------------------------------
-- idDoor = 300
-- triggerLock = Handle()
-- timesActivatedLock = 0
-- function activateLock( )
  -- p:print("Activate Lock\n")
  
  -- --Accionable a los 2 segundos
  -- triggerLock:getHandleCaller()
  -- p:exec_command( "triggerLock:setActionable(1);", 2 )
  
  -- --Modificamos polaridad durante 10 segundos
  -- h:get_handle_by_id(idDoor)
  -- --h:setPolarity(-1)  
  -- h:setLocked(0)  
  -- p:exec_command( "deactivateLock();", 6 )
  
  -- --Variable control para activaciones acumuladas
  -- timesActivatedLock = timesActivatedLock + 1
-- end

-- function putBattery( )
  -- p:print("Put Battery\n")
-- end

-- function deactivateLock( )
  -- p:print("Deactivate Lock\n")
  -- timesActivatedLock = timesActivatedLock - 1
  
  -- --Ha pasado el timer de la última activacion
  -- if timesActivatedLock < 1 then
	  -- --Puerta vuelve a neutral
      -- h:get_handle_by_id(idDoor)
      -- --h:setPolarity(0)
	  -- h:setLocked(1)
  -- end
-- end
--------------------------------------

-- Wall --
--------------------------------------
idWall = 302
tagWallFragment1 = "wall1_fragment"
triggerWall_1 = Handle()
all_fragments1 = HandleGroup()
actionWallTarget = Handle()
function lookWall( )
  p:print("Look Wall\n")
  triggerWall_1:getHandleCaller()
  p:player_talks("::look_wall")
  p:exec_command( "triggerWall_1:setActionable(1);", 2 )
end

function destroyWall( )
  p:print("Destroy Wall\n")
  triggerWall_1:getHandleCaller()
  
  --Llevamos el player al punto de accion
  actionWallTarget:get_handle_by_name_tag("tWall_pAction", "target")
  player = Handle()
  player:get_player()
  player:go_and_look_as(actionWallTarget, "destroyWallAnim();")
  
end

function destroyWallAnim( )
  p:print("Destroy Wall Animation")
  --Animacion Player
  player = Handle()
  player:get_player()
  player:set_anim("attack")
  p:exec_command("destroyWallEffect();", 0.5)
end

function destroyWallEffect()
  p:print("Destroy Wall Effect\n")
  
  --Destruimos pared
  h:get_handle_by_id(idWall)
  h:destroy()
  
  --Reproducimos sonido y vibracion
  p:play_sound("event:/OnBreakWall", 1.0, false)
  cam:start_vibration(0.0, 0.75, 10)
  p:exec_command("cam:stop_vibration(8)", 1.0)
  
  --Activamos fragmentos pared
  
  all_fragments1:get_handles_by_tag(tagWallFragment1)
  all_fragments1:awake()
  p:exec_command( "all_fragments1:remove_physics();", 5 )
  activeWire1();
  --p:exec_command( "activeWire1();", 0.5 )
end
--------------------------------------


-- Wires --
--------------------------------------
--Cable abajo info
triggerWire_1 = Handle()
tagWireParticle = "wire_particle"
wireName_1 = "tWireDown"
wirePart_1 = wireName_1.."_part"
wirePart_1_h = Handle()
wire_pos_up = "WirePosUp"
posParticleBefore = Pos()

--Cable arriba info
triggerWire_2 = Handle()
wire_pos_down = "WirePosDown"
----------------------------------------

--wireName_2 = "tWireUp" -- No lo necesitamos activar
--posParticleBefore = Pos() -- No hay "particulas" arriba ahora
--tagWireParticle = "wire_particle"
--wirePart_1 = wireName_1.."_part"
--wirePart_1_h = Handle()
function test()
  --Es porque testeando no activo el cable, y no tengo su handle
  triggerWire_1:get_handle_by_name_tag(wireName_1, "trigger")
  p:exec_command( "wireParticlesUp();", 2 )
end

function activeWire1( )
  wirePart_1_h:get_handle_by_name_tag(wirePart_1, tagWireParticle)
  posParticleBefore = wirePart_1_h:get_pos()
  triggerWire_1:get_handle_by_name_tag(wireName_1, "trigger")
  triggerWire_1:setActionable(1);
end

function wireGoUp( )
--  wirePart_1_h:follow_tracker(triggerWire_1, 15*toogle_test)
--  cam:run_cinematic("CineWireGoUp", 10*toogle_test)
--  toogle_test = -toogle_test
  p:print("Wire Go Up")
  factorWireGoUp = 2
  player = Player()
  player:get_player()
  cmd_teleport = "player:teleport(\""..wire_pos_up.."\")"
  cam:run_cinematic("CineWireGoUp", factorWireGoUp * 5)
  p:complete_tasklist(7)
  p:exec_command( "player:set_position(0,-1000,0);", 2 )
  p:exec_command( "wireParticlesUp();", 1.5 / factorWireGoUp )
  p:exec_command( "ui_cam:fade_out(0.5);", 7.5)
  p:exec_command( "ui_cam:fade_in(0.5);", 9.5)
  p:exec_command( "triggerWire_1:setActionable(1);", 3)
  p:exec_command( cmd_teleport, 7.5 )
end

function wireGoDown( )
  p:print("Wire Go Down")
  factorWireGoDown = 2
  player = Player()
  player:get_player()
  triggerWire_2:getHandleCaller()
  cmd_teleport = "player:teleport(\""..wire_pos_down.."\")"
  cam:run_cinematic("CineWireGoUp", factorWireGoDown * -5)

  p:exec_command( cmd_teleport, 1.9 )
  p:exec_command( "player:set_position(0,-1000,0);", 5.0 )
  --p:exec_command( "wireParticlesUp();", 2.5 / factorWireGoDown )
  ui_cam:fade_out(0.2)
  p:exec_command( "ui_cam:fade_in(1);", 3.0)
  p:exec_command( "triggerWire_2:setActionable(1);", 3)
  p:exec_command( cmd_teleport, 9.5 )
end

function wireParticlesUp( )
  --wirePart_1_h:set_pos(posParticleBefore)
  wirePart_1_h:follow_tracker(triggerWire_1, 15)
end

---------------- Andamio ----------------------------------------
ontablon1 = false
ontablon2 = false
ontablon3 = false
ontablon4 = false
ontablon5 = false
ontablon6 = false

function noTablon()
	if not ontablon1 and not ontablon2 and not ontablon3 and not ontablon4 and not ontablon5 and not ontablon6 then
		cam:skip_cinematic();
	end
end

function ms3_outTablon()
	ontablon1 = false
	ontablon2 = false
	ontablon3 = false
	ontablon4 = false
	ontablon5 = false
	ontablon6 = false
	p:exec_command("noTablon();", 1.5)
end

function ms3_onTablon1()
	ontablon1 = true
	cam:run_cinematic("CineTablon1", 5)
end

function ms3_onTablon2()
	ontablon2 = true
	cam:run_cinematic("CineTablon2", 5)
end

function ms3_onTablon3()
	ontablon3 = true
	cam:run_cinematic("CineTablon3", 5)
end

function ms3_onTablon4()
	ontablon4 = true
	cam:run_cinematic("CineTablon4", 5)
end

function ms3_onTablon5()
	ontablon5 = true
	cam:run_cinematic("CineTablon5", 5)
end

function ms3_onTablon6()
	--Mejor sin
	--cam:run_cinematic("CineTablon6", 5)
end

function Ms3Victory()
	ui_cam:fade_out(0.2)
	p:exec_command("Credits()", 0.2)
end

function Credits()
	p:clear_level()
	p:pause_game()
	p:play_video_and_do("data\\videos\\end.avi", "LoadLevel(\"level_0\");")
end








