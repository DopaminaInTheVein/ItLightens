print('This is lua')

SLB.using( SLB )

p = Public( )

function OnAction( param )
	p:print( "OnAction: "..param )
end

function OnEnter( param )
	p:print( "OnEnter: "..param )
end

function OnLeave( param )
	p:print( "OnLeave: "..param )
end

function OnGameStart( param )
	p:print( "OnGameStart: "..param )
end

function OnGameEnd( param )
	p:print( "OnGameEnd: "..param )
end

function OnLevelStart001( param )
	p:print( "OnLevelStart001: "..param )
end

function OnZoneStart001( param )
	p:print( "OnZoneStart001: "..param )
end

function OnZoneEnd001( param )
	p:print( "OnZoneEnd001: "..param )
end

function OnTimeout( param )
	p:print( "OnTimeout: "..param )
end

function OntTimerStart( param )
	p:print( "OntTimerStart: "..param )
end

function OnPlayerDead( param )
	p:print( "OnPlayerDead: "..param )
end

function OnInterruptHit( param )
	p:print( "OnInterruptHit: "..param )
end

function OnStartReceiveHit( param )
	p:print( "OnStartReceiveHit: "..param )
end

function OnEndReceiveHit( param )
	p:print( "OnEndReceiveHit: "..param )
end

function OnEmitParticles( param )
	p:print( "OnEmitParticles: "..param )
end

function OnChangePolarity( param )
	p:print( "OnChangePolarity: "..param )
end

function OnPickupBox( param )
	p:print( "OnPickupBox: "..param )
end

function OnLeaveBox( param )
	p:print( "OnLeaveBox: "..param )
end

function OnPossess( param )
	p:print( "OnPossess: "..param )
end

function OnUnpossess( param )
	p:print( "OnUnpossess: "..param )
end

function OnDash( param )
	p:print( "OnDash: "..param )
end

function OnBlink( param )
	p:print( "OnBlink: "..param )
end

function OnBreakWall( param )
	p:print( "OnBreakWall: "..param )
end

function OnUseCable( param )
	p:print( "OnUseCable: "..param )
end

function OnStun( param )
	p:print( "OnStun: "..param )
end

function OnStunned( param )
	p:print( "OnStunned: "..param )
end

function OnLiquid( param )
	p:print( "OnLiquid: "..param )
end

function OnBeingAttracted( param )
	p:print( "OnBeingAttracted: "..param )
end

function OnOvercharge( param )
	p:print( "OnOvercharge: "..param )
end

function OnDoubleJump( param )
	p:print( "OnDoubleJump: "..param )
end

function OnDetected( param )
	p:print( "OnDetected: "..param )
end

function OnBeaconDetect( param )
	p:print( "OnBeaconDetect: "..param )
end

function OnEnterPC( param )
	p:print( "OnEnterPC: "..param )
end

function OnLeavePC( param )
	p:print( "OnLeavePC: "..param )
end
