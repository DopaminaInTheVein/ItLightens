--==================
-- Level DATA
--==================
-- Texts
TXT_DEAD_ON_DREAM = "::dead_on_dream"


--TASKS
TASK_HUB_MOLE = 0
TASK_HUB_SCI = 1
-------------------------------
target_seen = false
mole_done = false
sci_done = false
sense_first_pressed = false
dead_on_dream = false
first_mole = false
first_sci = false
-------------------------------
function OnStart_hub( )
	target_seen = false
	hub_first_time = true
	mole_done = false
	sci_done = false
	
	p:setControlEnabled(1)
	p:setOnlySense(1)
	p:player_talks("::start_hub")
	p:exec_command("p:player_talks(\"::sense\")",4.0)
	sense_first_pressed = true
	p:play_music("event:/OnGameMusic", 0.2)
	
	p:setOnlySense(1)
	InitDreamingPeople()
end
-------------------------------
function OnSave_hub()
	-- nothing to save
end
-------------------------------
idMoleSlept = 5
idSciSlept = 6
idMoleZzz = 7
idSciZzz = 8
function OnLoad_hub()
	cam:fx(FX_DREAM_BORDER, 0)
	p:play_music("event:/OnGameMusic", 0.2)
	if dead_on_dream then
		p:player_talks(TXT_DEAD_ON_DREAM)
	else
		if mole_done and not sci_done then
			first_mole = true
			p:player_talks("::mole_tuto_complete")
			p:exec_command("p:player_talks(\"::do_other_tutorial_sci\")",3.5)
		end
		if not mole_done and sci_done then
			first_sci = true
			p:player_talks("::sci_tuto_complete")
			p:exec_command("p:player_talks(\"::do_other_tutorial_mole\")",3.5)
		end
		
		if first_mole and sci_done then
			p:player_talks("::sci_tuto_complete")
		end
		if first_sci and mole_done then
			p:player_talks("::mole_tuto_complete")
		end
		
		
		if mole_done and sci_done then
			end_hub_handles = HandleGroup()
			end_hub_handles:get_handles_by_tag("end_hub")
			end_hub_handles:awake()
			p:exec_command("p:player_talks(\"::tutos_completed\")",3.5)
		end
		if mole_done then
			h:get_handle_by_id(idMoleSlept)
			--h:set_anim_loop("run")
			p:complete_tasklist(TASK_HUB_MOLE)
		end
		if sci_done then
			h:get_handle_by_id(idSciSlept)
			--h:set_anim_loop("run")
			p:complete_tasklist(TASK_HUB_SCI)
		end
		
	end
	dead_on_dream = false
	InitDreamingPeople()
end

function InitDreamingPeople()
	h:get_handle_by_id(idSciZzz)
	local x = h:get_x()
	local y = h:get_y()
	local z = h:get_z()
	zzz_sci = p:create(SLEEP_1, x, y + 0.5, z)
	zzz_sci:part_loop()
	
	h:get_handle_by_id(idMoleZzz)
	local x = h:get_x()
	local y = h:get_y()
	local z = h:get_z()
	zzz_mole = p:create(SLEEP_1, x, y + 0.75, z)
	zzz_mole:part_loop()
end
-------------------------------

function hub_sense_pressed()
	--p:player_talks("::sense")
	if sense_first_pressed and not mole_done and not sci_done then
		p:setOnlySense(0)
		sense_first_pressed = false
		p:force_sense_vision()
		p:setControlEnabled(0)
		cam:run_cinematic("CineEndTarget", 2.5)
		
		p:aim_circle("circle_1","aim_circle",7.37,51.3,-30.65,-1.0)
		p:exec_command("p:player_talks(\"::target_seen_white\")",3.0)
		
		p:exec_command("p:no_aim_circle(\"circle_1\")",7.5)
		p:exec_command("p:aim_circle(\"circle_2\",\"aim_circle\",-1.94,51.64,-20.08,-1.0)",7.5)
		p:exec_command("p:player_talks(\"::target_seen_green\")",7.5)
		
		p:exec_command("p:no_aim_circle(\"circle_2\")",12.0)
		p:exec_command("p:aim_circle(\"circle_3\",\"aim_circle\",-2.2,52.5,-25.44,-1.0)",12.0)
		p:exec_command("p:player_talks(\"::target_seen_yellow\")",12.0)
		
		p:exec_command("p:no_aim_circle(\"circle_3\")",16.5)
		p:exec_command("cam:skip_cinematic()",16.5)
		p:exec_command("p:player_talks(\"::return_control\")",16.5)
		p:exec_command("p:unforce_sense_vision()",16.5)
		p:exec_command("p:setControlEnabled(1);",16.5)
	end	
	sense_first_pressed = false
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
	cam:run_cinematic("CineEndHub", 1.0)
	p:exec_command("ui_cam:fade_out(0.2)", 2.8)
	p:exec_command("EndHub();", 3.0)
end

function EndHub()
	p:pause_game()
	p:stop_music()
	p:clear_level()
	p:play_video_and_do("data\\videos\\ms3.avi", "LoadLevel(\"level_4\");")
end
