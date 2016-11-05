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