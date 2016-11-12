--Levels
---------------------------------------------------
function OnRestartLevel( logic_level, real_level )
	p:print( "OnRestartLevel\n")
	LoadLevel(logic_level) -- Defined in functions.lua
end

function OnLevelStart( logic_level, real_level )
	p:print("OnLevelStart\n")
	g_new_level = true
	SceneLoaded(real_level)
end

function OnSavedLevel( logic_level, real_level )
	p:print("OnSavedLevel")
	CallFunction("OnSave_"..real_level)	
end

function OnLoadedLevel( logic_level, real_level )
	p:print("OnLoadedLevel")
	g_new_level = false
	SceneLoaded(real_level)
end

function SceneLoaded(real_level)
	if g_loading_screen then
		--p:exit_game()
		p:pause_game()
		p:putTextUi("loading_skip", "::loading_skip", 0.475, 0.08, "#555599EE", 0.4)
		local hloading_text = Handle()
		hloading_text:get_handle_by_name_tag("loadingpalabra", "loading_text")
		hloading_text:destroy()
		p:wait_action("InitScene(\""..real_level.."\");");
		--p:exec_command("InitScene()", 5.0)
	else
		InitScene(real_level)
	end
end

loading_handles = HandleGroup()
function InitScene(real_level)
	g_restarting = false
	g_dead = false
	cam:reset_camera()
	ui_cam:fade_out(0.5)
	p:exec_command("PrepareScene(\""..real_level.."\");", 0.5)
end

function PrepareScene(real_level)
	p:removeText("loading_skip")
	p:exec_command("ui_cam:fade_in(1);", 1)
	p:unforce_sense_vision()
	if real_level ~= "hub" then
		p:exec_command("p:setControlEnabled(1);", 1.5)
	end
	if not g_is_menu then
		p:load_entities("player_hud")
	end
	loading_handles:get_handles_by_tag("loading")
	loading_handles:destroy()
	p:resume_game()
	if g_new_level then
		p:exec_command("CallFunction(\"OnStart_"..real_level.."\");", 1.1)
	else
		p:exec_command("CallFunction(\"OnLoad_"..real_level.."\");", 1.1)
	end
end

function OnLoadingLevel(level)
	p:print("OnLoadingLevel")
	g_loading_screen = false
	-- Capo no pasar por loading screen en el restart!
	g_restarting = false
	--
	if not g_restarting  then
		local ok = CallFunction("OnLoading_"..level)
		if not ok then 
			ui_cam:fade_in(0.1)
			p:load_entities("loading")
			g_loading_screen = true
		end
	end
end