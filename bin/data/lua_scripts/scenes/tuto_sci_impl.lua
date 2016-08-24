p = Public()

function openDoorTutoSci( )
	p:print( "openDoorTutoSci\n")
	LoadLevel("level_1") -- Defined in functions.lua
end

function OnUseGenerator_gen_tuto_sci( )
	p:player_talks("gen tuto sci", "scientific.dds", "SCI")
end