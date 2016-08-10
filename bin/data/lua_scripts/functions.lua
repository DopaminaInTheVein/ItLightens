print('This is lua')

SLB.using( SLB )

p = Public( )

function Teleport( param )
    p:print( "Teleport: "..param.."\n" )
end

function GodMode( param )
    p:print( "God Mode: "..param.."\n" )
end

--show/hide the player depending on the value of the parameter
function ShowPlayer( enabled )
    p:print( "Show Player: "..enabled.."\n" )
end

function FreeCamera( enabled )
    p:print( "Free Camera: "..enabled.."\n" )
end

function PauseGame( enabled )
    p:print( "Pause Game: "..enabled.."\n" )
end

function AddEnergy ( points )
	p:print( "Add Energy: "..points.."\n" )
end

function RefillEnergy ()
	p:print( "Refill Energy.\n" )
end

function Lose ()
	p:print( "Lose.\n" )
end

function Win ()
	p:print( "Win.\n" )
end

function SetTarget( target )
	p:print( "Set Target: "..target.."\n" )
end

function SaveLevel( )
	p:save_level()
end

function LoadLevel( logic_level )
	p:print("Load Level")
	p:load_level(logic_level)
end