p = Public()

function openDoorTutoSci( )
	p:print( "openDoorTutoSci\n")
	cam:fade_out(1)
	p:setControlEnabled(0)
	p:exec_command("LoadLevel(\"level_1\");", 1) -- Defined in functions.lua
end