print('This is lua')

SLB.using( SLB )

p = Public( )

function execCommandTest( code, timer )
    p:exec_command( code, timer )
end

--prints the specified text via the VS console
function dbg( text )
    p:print( "DBG: "..text.."\n" )
end

function teleportPlayer( x, y, z )
	p:print( "TELEPORT PLAYER.\n" )
	handle = Handle()
	handle:set_player_position(x, y, z)
end

