p = Public()

--==================
-- Level DATA
--==================
TASK_TUTOS_POSS = 0
TASK_TUTOS_BOMB = 1
TASK_TUTOS_STUN = 2
TASK_TUTOS_DRONE = 3
TASK_TUTOS_GENERATOR = 4
-------------------------
poss_done = false
bomb_done = false
stunned_1 = false
stunned_2 = false
repaired_done = false
unposs_done = false
reach_done = false
gen_done = false
--==================

--===========================================
-- Events
--===========================================
function OnDead_tuto_sci( )
	dead_on_dream = true
	LoadLevel("level_1")
end

function OnStart_tuto_sci( )
	poss_done = false
	bomb_done = false
	stunned_1 = false
	stunned_2 = false
	repaired_done = false
	unposs_done = false
	reach_done = false
	gen_done = false
	tutosci_help_possess();
end

-- function opendoortutosci( )
	-- p:print( "opendoortutosci\n")
	-- loadlevel("level_1") -- defined in functions.lua
-- end

function OnUseGenerator_gen_tuto_sci( )
	if not gen_done then
		gen_done = true
		openDoorTutoSci()
		tutosci_help_exit()
		p:complete_tasklist(TASK_TUTOS_GENERATOR)
	end
end

function OnPossess_tuto_sci( )
	if not poss_done then
		poss_done = true
		tutosci_help_wb()
		p:complete_tasklist(TASK_TUTOS_POSS)
	end
end

function OnUnpossess_tuto_sci( )
	if repaired_done then
		if not unposs_done then
			unposs_done = true
			tutosci_help_polarity()
			p:complete_tasklist(TASK_TUTOS_DRONE)
		end
	end
end

function OnCreateBomb_tuto_sci( )
	if not bomb_done then
		bomb_done = true
		tutosci_help_bomb()
		p:complete_tasklist(TASK_TUTOS_BOMB)
	end
end

function OnStunned_guard_tuto_sci_1( )
	if not stunned_1 then
		stunned_1 = true
		if stunned_2 then
			tutosci_stunned_guards()
		else
			tutosci_stunned_guard_remain()
		end
	end
end

function OnStunned_guard_tuto_sci_2( )
	if not stunned_2 then
		stunned_2 = true
		if stunned_1 then
			tutosci_stunned_guards()
		else
			tutosci_stunned_guard_remain()
		end
	end
end

function OnRepairDrone_tuto_sci( )
	if not repaired_done then
		repaired_done = true
		tutosci_help_unpossess()
	end
end

function tutosci_reach( )
	if not reach_done then
		reach_done = true
		tutosci_help_generator()
	end
end

function tutosci_end( )
	sci_done = true
	LoadLevel("level_1")
end
--=============================================

--==============================================================
-- Auxiliars
--==============================================================
function tutosci_help_possess( )
	p:player_talks(p:get_text("tuto_sci","help_possess"))
end
function tutosci_help_unpossess( )
	p:player_talks(p:get_text("tuto_sci","help_unpossess"))
end
function tutosci_help_wb( )
	p:player_talks(p:get_text("tuto_sci","help_wb"))
end
function tutosci_help_bomb( )
	p:player_talks(p:get_text("tuto_sci","help_bomb"))
end
function tutosci_help_exit( )
	p:player_talks(p:get_text("tuto_sci","help_exit"))
end
function tutosci_stunned_guard_remain()
	p:player_talks(p:get_text("tuto_sci","stunned_guard_remain"))
end
function tutosci_stunned_guards( )
	tutosci_help_repair()
	p:complete_tasklist(TASK_TUTOS_STUN)
end
function tutosci_help_repair( )
	p:player_talks(p:get_text("tuto_sci","help_repair"))
end
function tutosci_help_polarity( )
	p:player_talks(p:get_text("tuto_sci","help_polarity"))
end
function tutosci_help_generator( )
	p:player_talks(p:get_text("tuto_sci","help_generator"))
end

-- Door
idDoorTutoSci = 50
hDoorTutoSci = Handle()
function openDoorTutoSci( )
	hDoorTutoSci:get_handle_by_id(idDoorTutoSci)
	hDoorTutoSci:setLocked(0)
	p:play_sound("event:/OnDoorClosing", 1.0, false)
end
--==============================================================
