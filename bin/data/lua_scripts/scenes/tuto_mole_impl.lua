p = Public()

--==================
-- Level DATA
--==================
poss_done = false
box_done = false
unposs_done = false
--==================

--===========================================
-- Events
--===========================================
function OnStart_tuto_mole( )
	poss_done = false
	box_done = false
	p:exec_command("tutomole_help_possess();", 2)
end

function OnPossess_tuto_mole( )
	if not poss_done then
		poss_done = true
		tutomole_help_wall()
	end
end

function OnUnpossess_tuto_mole( )
	if box_done then
		if not unposs_done then
			unposs_done = true
			tutomole_help_djump()
		end
	end
end

function tutomole_wall_crossed( )
	tutomole_help_box()
end

function tutomole_box_placed( )
	if not box_done then
		box_done = true
		tutomole_help_unpossess()
	end
end

function tutomole_lookWall( )
  p:print("Look Wall\n")
  triggerWall_1:getHandleCaller()
  p:player_talks("This wall seems weak, I wonder if there is something behind...","scientific.dds","SCI")
  p:exec_command( "triggerWall_1:setActionable(1);", 2 )
end

function tutomole_destroyWall( )
  p:print("Destroy Wall\n")
  triggerWall_1:getHandleCaller()
  
  --Llevamos el player al punto de accion
  actionWallTarget:get_handle_by_name_tag("_tWall_pAction", "target")
  player = Handle()
  player:get_player()
  player:go_and_look_as(actionWallTarget, "tutomole_destroyWallAnim();")
end

function tutomole_destroyWallAnim( )
  p:print("Destroy Wall Animation")
  --Animacion Player
  player = Handle()
  player:get_player()
  player:set_anim("attack")
  p:exec_command("tutomole_destroyWallEffect();", 0.5)
end

function tutomole_destroyWallEffect()
  p:print("Destroy Wall Effect\n")
  
  --Destruimos pared
  h:get_handle_by_id(idWall)
  h:destroy()
  
  --Activamos fragmentos pared
  
  all_fragments1:get_handles_by_tag(tagWallFragment1)
  all_fragments1:awake()
  p:exec_command( "all_fragments1:remove_physics();", 5 )
  activeWire1();
  --p:exec_command( "activeWire1();", 0.5 )
end
--------------------------------------
--------------

function tutomole_end( )
	mole_done = true
	LoadLevel("level_1")
end
--=============================================

--==============================================================
-- Auxiliars
--==============================================================
function tutomole_help_possess( )
	p:player_talks("Acercate a la mole y pulsa SHIFT IZQUIERDO para poseer su cuerpo.", "scientific.dds", "SCI")
end
function tutomole_help_unpossess( )
	p:player_talks("Pulsa SHIFT IZQUIERDO para abandonar el cuerpo de la mole.", "scientific.dds", "SCI")
end
function tutomole_help_wall( )
	p:player_talks("Acercate al muro de enfrente con la mole y pulsa CLIC IZQUIERDO para destruirlo", "scientific.dds", "SCI")
end
function tutomole_help_box( )
	p:player_talks("Acercate a na caja y pulsa CLIC IZQUIERDO para empujar o tirar", "scientific.dds", "SCI")
end
function tutomole_help_djump( )
	p:player_talks("Pulsa ESPACIO para saltar. Puedes pulsar de nuevo para efectuar doble salto", "scientific.dds", "SCI")
end
--==============================================================
