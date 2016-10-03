--==================
-- Level DATA
--==================
-- Texts
TXT_DEAD_ON_DREAM = "dead_on_dream"


--TASKS
TASK_HUB_MOLE = 0
TASK_HUB_SCI = 1
-------------------------------
target_seen = false
mole_done = false
sci_done = false
dead_on_dream = false
-------------------------------
function OnStart_hub( )
	target_seen = false
	hub_first_time = true
	mole_done = false
	sci_done = false
	p:setControlEnabled(1)
	p:setOnlySense(1)
	p:aim_circle("circle_1","aim_circle",-1.94,51.64,-20.08,-1.0)
	p:force_sense_vision()
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
	if dead_on_dream then
		p:player_talks(p:get_text("hub",TXT_DEAD_ON_DREAM))
	else
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
	dead_on_dream = false
end
-------------------------------
--===========================================
-- Events
--===========================================
function hub_target_seen()
	p:player_talks(p:get_text("hub","target_seen"))
	h:getHandleCaller()
	h:destroy()
	p:exec_command("p:player_talks(p:get_text(\"hub\",\"target_seen2\"))",9.5)
	p:exec_command("p:no_aim_circle(\"circle_1\")",16.0)
	p:exec_command("p:unforce_sense_vision()",18.0)

	p:exec_command("p:setOnlySense(0);", 18.5)
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
