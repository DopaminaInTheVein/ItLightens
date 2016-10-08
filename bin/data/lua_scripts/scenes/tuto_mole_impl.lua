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
box_contacted = false
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
	box_contacted = false
	box_moved_done = false
	box_placed_done = false
	portal_done = false
	idWall_out = 100
	idWall_in = 101
	
	IntroTutoMole()
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
	TutoMoleSala()
	-- tutomole_help_box()
	-- h:getHandleCaller()
	-- h:destroy()
end

function OnBoxMode_tuto_mole( )
	TutoMoleUseBox()
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
  -- Hide Message
  p:hide_message()

  -- Guardamos trigger
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
  p:exec_command("TutoMoleWallDestroyed();", 0.5)
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
-- Intro mole
------------------------------------------------------------------------------------
function IntroTutoMole( )
	cam:start_cinematic("CineIntro", 7)
	p:exec_command("IntroTutoMole2();", 5)
end

function IntroTutoMole2( )
	p:show_message("Mensaje mole sueño", "mole")--p:get_text("hub","target_seen"))
	p:exec_command("IntroTutoMole3();", 1)
end

function IntroTutoMole3( )
	p:wait_button("IntroTutoMole4();")
end

function IntroTutoMole4( )
	cam:skip_cinematic()
	p:hide_message()
	p:exec_command("tutomole_help_possess();", 0.5)
end
----------------------------------------------------------------------------------
-- Wall destroyed
function TutoMoleWallDestroyed()
	p:show_message("¡Muy bien! Ahora busquemos la salida de este sueño", "raijin")
	p:exec_command("p:hide_message()", 4)
end
----------------------------------------------------------------------------------
function TutoMoleSala()
	cam:run_cinematic("CineSala", 5)
	p:exec_command("TutoMoleSala2();", 5)
end

function TutoMoleSala2()
	p:show_message("¡Mira! Ese portal debe ser la salida.", "raijin")
	p:exec_command("TutoMoleSala3();", 0.5)
end

function TutoMoleSala3()
	p:wait_button("TutoMoleSala4();")
end

function TutoMoleSala4()
	cam:run_cinematic("CinePortalWay", 5)
	p:exec_command("TutoMoleSala5();", 2)
end

function TutoMoleSala5()
	p:show_message("Necesitaré algo para llegar tan alto", "raijin")
	p:exec_command("TutoMoleSala6();", 0.5)
end

function TutoMoleSala6()
	p:wait_button("TutoMoleSala7();")
end

function TutoMoleSala7()
	p:hide_message()
	cam:run_cinematic("CineBox", 5)
	p:exec_command("TutoMoleSala8();", 2)
end

function TutoMoleSala8()
	p:show_message("Qué tal con esta caja?", "raijin")
	p:exec_command("TutoMoleSala9();", 0.5)
end

function TutoMoleSala9()
	p:wait_button("TutoMoleSala10();")
end

function TutoMoleSala10()
	cam:skip_cinematic()
	p:hide_message()
	p:exec_command("tutomole_help_box();", 1)
end
---------------------------------------------------------------------------------------
function TutoMoleUseBox()
	if not box_contacted then
		box_contacted = true
		p:show_message("W pa empujar, S pa estirar", "raijin")
	end
end

function tutomole_help_possess( )
	p:show_message("Ayuda posesión", "raijin")
end

function tutomole_help_unpossess( )
	p:player_talks(p:get_text("tuto_mole", "help_unpossess"))
end
function tutomole_help_place_box( )
	p:player_talks(p:get_text("tuto_mole", "help_place_box"))
end
function tutomole_help_wall( )
	p:show_message(p:get_text("tuto_mole", "help_wall"), "raijin")
end
function tutomole_help_box( )
	p:show_message(p:get_text("tuto_mole", "help_box"), "raijin")
end
function tutomole_help_djump( )
	p:player_talks(p:get_text("tuto_mole", "help_djump"))
end
--==============================================================
