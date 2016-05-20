print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle( )

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
