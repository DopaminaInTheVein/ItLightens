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