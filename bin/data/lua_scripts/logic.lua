print('This is lua!')

SLB.using( SLB )

h = Handle( )
c = Camera( )
p = Public( )

function execCommandTest( code, timer )
    p:exec_command( code, timer )
end

function triggerGuardFormation()
	h:toggle_guard_formation()
end

function triggerScientistBusy()
	h:toggle_scientist_busy()
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

function setCameraDistanteToTarget( distance )
	c:get_camera()
	c:set_distance_to_target(distance)
end

function setCameraSpeed( speed )
	c:get_camera()
	c:set_speed(speed)
end

function setCameraSpeedUnlocked( speed )
	c:get_camera()
	c:set_speed_unlocked(speed)
end

function setCameraRotationSensibility( sensibility )
	c:get_camera()
	c:set_rotation_sensibility(sensibility)
end

function setCameraPositionOffset( x, y, z )
	c:get_camera()
	c:set_position_offset(x,y,z)
end

function toggleIntroState( )
	p:toggle_intro_state()
end

function launchVictoryState( )
	p:launch_victory_state()
end

function specialActionSettings( action_time )
	setCameraDistanteToTarget(1.0);
	setCameraPositionOffset(0.0, -1.5, 0.0);
	p:exec_command("toggleIntroState();", 0.2)
	p:exec_command("toggleIntroState();", action_time + 0.2)
	p:exec_command("setCameraDistanteToTarget(2.0); setCameraPositionOffset(0.0, -0.5, 0.0);", action_time + 0.4)
end
