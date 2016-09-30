p = Public()
--==================
-- Constants level
--==================
idWall_out = 0
idWall_in = 0

--==================
-- Level DATA
--==================
TASK_TUTOM_POSS = 0
TASK_TUTOM_WALL = 1
TASK_TUTOM_BOX_MOVE = 2
TASK_TUTOM_BOX_PLACED = 3
--TASK_TUTOM_PORTAL = 4

---------------------------------------------
poss_done = false
box_moved_done = false
box_placed_done = false
unposs_done = false
portal_done = false
--==================

--===========================================
-- Events
--===========================================
function OnDead_tuto_mole( )
	dead_on_dream = true
	LoadLevel("level_1")
end

function OnStart_tuto_mole( )
	poss_done = false
	box_moved_done = false
	box_placed_done = false
	portal_done = false
	idWall_out = 100
	idWall_in = 101
	p:exec_command("tutomole_help_possess();", 2)
end

function OnPossess_tuto_mole( )
	if not poss_done then
		poss_done = true
		tutomole_help_wall()
		--Tasklist
		p:complete_tasklist(TASK_TUTOM_POSS)
	end
end

function OnUnpossess_tuto_mole( )
	if box__placed_done then
		if not unposs_done then
			unposs_done = true
			tutomole_help_djump()
		end
	end
end

function tutomole_wall_crossed( )
	tutomole_help_box()
	h:getHandleCaller()
	h:destroy()
end

function tutomole_box_placed( )
	if not box_placed_done then
		box_placed_done = true
		tutomole_help_unpossess()
		
		--Tasklist
		p:complete_tasklist(TASK_TUTOM_BOX_PLACED)
	end
end

function tutomole_box_moved( )
	if not box_moved_done then
		box_moved_done = true
		tutomole_help_place_box()
		
		--Tasklist
		p:complete_tasklist(TASK_TUTOM_BOX_MOVE)
	end
end

function tutomole_lookWall( )
  p:print("Look Wall\n")
  triggerWall_1:getHandleCaller()
  p:player_talks(p:get_text("tuto_mole", "look_wall"))
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
  h:get_handle_by_id(idWall_in)
  h:destroy()
  h:get_handle_by_id(idWall_out)
  h:destroy()
  
  --Reproducimos sonido
  p:play_sound("event:/OnBreakWall", 1.0, false)
  
  --Activamos fragmentos pared
  all_fragments1:get_handles_by_tag(tagWallFragment1)
  all_fragments1:awake()
  p:exec_command( "all_fragments1:remove_physics();", 4 )
  
  --Tasklist
  p:complete_tasklist(TASK_TUTOM_WALL)
end
--------------------------------------
--------------

function tutomole_end( )
	if not portal_done then
		portal_done = true
		mole_done = true
		LoadLevel("level_1")
	end
end
--=============================================

--==============================================================
-- Auxiliars
--==============================================================
function tutomole_help_possess( )
	p:player_talks(p:get_text("tuto_mole", "help_possess"))
end
function tutomole_help_unpossess( )
	p:player_talks(p:get_text("tuto_mole", "help_unpossess"))
end
function tutomole_help_place_box( )
	p:player_talks(p:get_text("tuto_mole", "help_place_box"))
end
function tutomole_help_wall( )
	p:player_talks(p:get_text("tuto_mole", "help_wall"))
end
function tutomole_help_box( )
	p:player_talks(p:get_text("tuto_mole", "help_box"))
end
function tutomole_help_djump( )
	p:player_talks(p:get_text("tuto_mole", "help_djump"))
end
--==============================================================
