print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle( )

function OnEnter_tElevator( )
  --Nothing to do
end

function OnLeave_tElevator( )
  --Nothing to do
end


-- Elevator --
-------------------------------------------------
elevatorState = "down"
function activateElevator( )
  if elevatorState == "down" then
    p:print("Elevator Up\n")
	elevatorState = "up"
  else
    p:print("Elevator Down\n")
	elevatorState = "down"
  end
  
  h:getHandleCaller()
  p:setControlEnabled(0)
  p:exec_command( "h:setActionable(1);", 10 )
  p:exec_command( "p:setControlEnabled(1);", 10 )
end
--------------------------------------------------

-- Electric Lock --
--------------------------------------
idDoor = 300
triggerLock = Handle()
timesActivatedLock = 0
function activateLock( )
  p:print("Activate Lock\n")
  
  --Accionable a los 2 segundos
  triggerLock:getHandleCaller()
  p:exec_command( "triggerLock:setActionable(1);", 2 )
  
  --Modificamos polaridad durante 10 segundos
  h:get_handle_by_id(idDoor)
  --h:setPolarity(-1)  
  h:setLocked(0)  
  p:exec_command( "deactivateLock();", 10 )
  
  --Variable control para activaciones acumuladas
  timesActivatedLock = timesActivatedLock + 1
end

function deactivateLock( )
  p:print("Deactivate Lock\n")
  timesActivatedLock = timesActivatedLock - 1
  
  --Ha pasado el timer de la última activacion
  if timesActivatedLock < 1 then
	  --Puerta vuelve a neutral
      h:get_handle_by_id(idDoor)
      --h:setPolarity(0)
	  h:setLocked(1)
  end
end
--------------------------------------