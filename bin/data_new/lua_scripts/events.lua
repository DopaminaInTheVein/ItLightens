print('This is lua')

SLB.using( SLB )

p = Public( )

function OnAction( param )
	p:print( "OnAction: "..param.."\n" )
end

function OnEnter( param, handle )
	p:print( "OnEnter: "..param.."\n" ) 
	if _G["OnEnter_"..param] then _G["OnEnter_"..param](handle) 
	else
		p:print("function OnEnter_"..param.." does not exist!\n")
	end
end

function OnLeave( param )
	p:print( "OnLeave: ".. param.."\n" )
end

function OnGameStart( param )
	p:print( "OnGameStart: "..param.."\n" )
end

function OnGameEnd( param )
	p:print( "OnGameEnd: "..param.."\n" )
end

function OnGuardAttack( reaction_time )
	p:print( "OnGuardAttack: "..reaction_time.."\n" )
end

function OnGuardRemoveBox( reaction_time )
	p:print( "OnGuardRemoveBox: "..reaction_time.."\n" )
end

function OnGuardOvercharged( param )
	p:print( "OnGuardOvercharged: "..param.."\n" )
end

function OnLevelStart001( param )
	p:print( "OnLevelStart001: "..param.."\n" )
end

function OnZoneStart001( param )
	p:print( "OnZoneStart001: "..param.."\n" )
end

function OnZoneEnd001( param )
	p:print( "OnZoneEnd001: "..param.."\n" )
end

function OnTimeout( param )
	p:print( "OnTimeout: "..param.."\n" )
end

function OntTimerStart( param )
	p:print( "OntTimerStart: "..param.."\n" )
end

function OnPlayerDead( param )
	p:print( "OnPlayerDead: "..param.."\n" )
end

function OnInterruptHit( param )
	p:print( "OnInterruptHit: "..param.."\n" )
end

function OnStartReceiveHit( param )
	p:print( "OnStartReceiveHit: "..param.."\n" )
end

function OnEndReceiveHit( param )
	p:print( "OnEndReceiveHit: "..param.."\n" )
end

function OnEmitParticles( param )
	p:print( "OnEmitParticles: "..param.."\n" )
end

function OnChangePolarity( param )
	p:print( "OnChangePolarity: "..param.."\n" )
end

function OnPickupBox( param )
	p:print( "OnPickupBox: "..param.."\n" )
end

function OnLeaveBox( param )
	p:print( "OnLeaveBox: "..param.."\n" )
end

function OnPossess( param )
	p:print( "OnPossess: "..param.."\n" )
end

function OnUnpossess( param )
	p:print( "OnUnpossess: "..param.."\n" )
end

function OnDash( param )
	p:print( "OnDash: "..param.."\n" )
end

function OnBlink( param )
	p:print( "OnBlink: "..param.."\n" )
end

function OnBreakWall( param )
	p:print( "OnBreakWall: "..param.."\n" )
end

function OnUseCable( param )
	p:print( "OnUseCable: "..param.."\n" )
end

function OnUseGenerator( param )
	p:print( "OnUseGenerator: "..param.."\n" )
end

function OnStun( param )
	p:print( "OnStun: "..param.."\n" )
end

function OnStunned( param )
	p:print( "OnStunned: "..param.."\n" )
end

function OnLiquid( param )
	p:print( "OnLiquid: "..param.."\n" )
end

function OnBeingAttracted( param )
	p:print( "OnBeingAttracted: "..param.."\n" )
end

function OnOvercharge( param )
	p:print( "OnOvercharge: "..param.."\n" )
end

function OnDoubleJump( param )
	p:print( "OnDoubleJump: "..param.."\n" )
end

function OnDetected( param )
	p:print( "OnDetected: "..param.."\n" )
end

function OnBeaconDetect( param )
	p:print( "OnBeaconDetect: "..param.."\n" )
end

function OnEnterPC( param )
	p:print( "OnEnterPC: "..param.."\n" )
end

function OnLeavePC( param )
	p:print( "OnLeavePC: "..param.."\n" )
end