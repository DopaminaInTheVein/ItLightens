print('This is lua')

SLB.using( SLB )

p = Public( )

function Teleport( param )
    p:print( "Teleport: "..param )
end

function GodMode( param )
    p:print( "God Mode: "..param )
end

--show/hide the player depending on the value of the parameter
function ShowPlayer( enabled )
    p:print( "Show Player: "..enabled )
end

function FreeCamera( enabled )
    p:print( "Free Camera: "..enabled )
end

function PauseGame( enabled )
    p:print( "Pause Game: "..enabled )
end

function AddEnergy ( points )
	p:print( "Add Energy: "..points )
end

function RefillEnergy ()
	p:print( "Refill Energy. " )
end

function Lose ()
	p:print( "Lose. " )
end

function Win ()
	p:print( "Win. " )
end

function SetTarget( target )
	p:print( "Set Target: "..target )
end



