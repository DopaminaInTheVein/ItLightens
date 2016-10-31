function OnEnter__tColumn( )
tutomole_reach_column()
end

function OnLeave__tColumn( )
tutomole_out_column()
end

function OnEnter__ttutomole_end( )
tutomole_end()
end

function OnEnter__tCrossWall( )
tutomole_wall_crossed()
end

function OnAction__tWall( )
tutomole_lookWall()
end

function OnActionMole__tWall( )
tutomole_destroyWall()
end

