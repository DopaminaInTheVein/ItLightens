print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle( )
hg = HandleGroup()

function OnEnter_tElevator( )
  --Nothing to do
end

function OnLeave_tElevator( )
  --Nothing to do
end


-- Elevator --
-------------------------------------------------
idElevator = 301
triggerElevator = Handle()
--elevatorState = "down"
function activateElevator( )
  --if elevatorState == "down" then
  --  p:print("Elevator Up\n")
  --  elevatorState = "up"
  --else
  --  p:print("Elevator Down\n")
  --  elevatorState = "down"
  --end
  
  triggerElevator:getHandleCaller()
  p:setControlEnabled(0)
  h:get_handle_by_id(idElevator)
  h:activate()
  
  p:exec_command( "triggerElevator:setActionable(1);", 2 )
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
  p:exec_command( "deactivateLock();", 6 )
  
  --Variable control para activaciones acumuladas
  timesActivatedLock = timesActivatedLock + 1
end

function putBattery( )
  p:print("Put Battery\n")
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

-- Wall --
--------------------------------------
idWall = 302
tagWallFragment1 = "wall1_fragment"
triggerWall_1 = Handle()
all_fragments1 = HandleGroup()
actionWallTarget = Handle()
function destroyWall( )
  p:print("Destroy Wall\n")
  triggerWall_1:getHandleCaller()
  p:exec_command( "triggerWall_1:setActionable(1);", 1 ) --test
  
  --Llevamos el player al punto de accion
  actionWallTarget:get_handle_by_name_tag("tWall_pAction", "target")
  player = Handle()
  player:get_player()
  player:go_and_look_as(actionWallTarget)
  
  --Destruimos pared
  --h:get_handle_by_id(idWall)
  --h:destroy()
  
  --Activamos fragmentos pared
  --all_fragments1:get_handles_by_tag(tagWallFragment1)
  --all_fragments1:awake()
  --p:exec_command( "all_fragments1:remove_physics();", 5 )
end
--------------------------------------