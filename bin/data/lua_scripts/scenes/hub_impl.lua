--==================
-- Level DATA
--==================

--TASKS
TASK_HUB_MOLE = 0
TASK_HUB_SCI = 1
-------------------------------
target_seen = false
mole_done = false
sci_done = false
-------------------------------
function OnStart_hub( )
	target_seen = false
	hub_first_time = true
	mole_done = false
	sci_done = false
	p:setControlEnabled(1)
	p:setOnlySense(1)
	--p:exec_command("p:setPlayerEnabled(1)", 5) --test
	p:player_talks(p:get_text("hub","start_hub"))
end
-------------------------------
function OnSave_hub()
	-- nothing to save
end
-------------------------------
idMoleSlept = 5
idSciSlept = 6
function OnLoad_hub()
	if mole_done and sci_done then
		end_hub_handles = HandleGroup()
		end_hub_handles:get_handles_by_tag("end_hub")
		end_hub_handles:awake()
	end
	if mole_done then
		h:get_handle_by_id(idMoleSlept)
		h:set_anim_loop("run")
		p:complete_tasklist(TASK_HUB_MOLE)
	end
	if sci_done then
		h:get_handle_by_id(idSciSlept)
		h:set_anim_loop("run")
		p:complete_tasklist(TASK_HUB_SCI)
	end
end
-------------------------------
--===========================================
-- Events
--===========================================
function hub_target_seen()
	p:player_talks(p:get_text("hub","target_seen"))
	h:getHandleCaller()
	h:destroy()
	p:exec_command("p:setOnlySense(0);", 0.5)
end

function dream_mole()
	h:getHandleCaller()
	h:setActionable(1)
	SaveLevel()
	LoadLevel("level_2")
end

function dream_sci()
	h:getHandleCaller()
	h:setActionable(1)
	SaveLevel()
	LoadLevel("level_3")
end

function hub_end()
	SaveLevel()
	LoadLevel("level_4")
end
--=============================================

--==============================================================
-- Auxiliars
--==============================================================

--==============================================================
