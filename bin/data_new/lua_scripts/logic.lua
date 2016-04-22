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
	player = Player()
	player:set_position(x, y, z)
end

function teleportSpeedy( name, x, y, z )
	p:print( "TELEPORT SPEEDY.\n" )
	handle = Handle()
	handle:get_handle_by_name_tag(name, "AI_speedy")
	handle:set_position(x, y, z)
end

