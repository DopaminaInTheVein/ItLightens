print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle( )

function prueba( )
  p:print("prueba\n")
end

function pruebaAction()
 p:print ("pruebaAction\n")
 h:getHandleCaller()
 --h:setActionable(1)
 p:exec_command( "h:setActionable(1);", 10 )
end

