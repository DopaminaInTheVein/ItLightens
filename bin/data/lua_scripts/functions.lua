print('This is lua')

SLB.using( SLB )

p = Public( )
cam = Camera()

function nothing( )
	--Default function doesnt do anything
end

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

g_current_level = "level_0"
g_is_menu = false
function LoadLevel( logic_level )
	g_current_level = logic_level
	if g_current_level == "level_0" then 
		g_is_menu = true
	else
		g_is_menu = false
	end
	p:print("Load Level")
	--ui_cam:fade_out(1)
	p:setControlEnabled(0)
	p:load_entities("loading")
	--p:exec_command("p:load_entites(\"loading\")", 0.1)
	p:exec_command("p:load_level(\""..logic_level.."\")", 1)
end
function LoadLevelSaving( logic_level )
g_current_level = logic_level
	p:print("Load Level")
	ui_cam:fade_out(1)
	p:setControlEnabled(0)
	p:exec_command("SaveLevel()", 1)
	p:exec_command("p:load_level(\""..logic_level.."\")", 1.2)
end

function DestroyAllByTag(tag)
	handles = HandleGroup()
	handles:get_handles_by_tag(tag)
	handles:destroy()
end