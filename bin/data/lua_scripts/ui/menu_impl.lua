p = Public()

function OnCreateGui_btn_resume( )
	p:pause_game()
	p:wait_escape("OnClicked_btn_resume();")
end

function OnClicked_btn_resume( )
	p:play_sound("event:/OnMenuAccept", 1.0, false)
	p:wait_escape_cancel()
	p:set_cursor_enabled(false)
	destroyMenu()
	p:resume()
end

function OnClicked_btn_restart( )
	-- (Fadeout)
	p:play_sound("event:/OnMenuAccept", 1.0, false)
	p:wait_escape_cancel()
	p:set_cursor_enabled(false)
	cam:skip_cinematic()
	p:exec_command("destroyMenu();", 1)
	p:exec_command("LoadLevel(g_current_level);", 1.1)
	g_restarting = true
end

function OnClicked_btn_back_title( )
	-- (Fadeout)
	p:play_sound("event:/OnMenuCancel", 1.0, false)
	p:wait_escape_cancel()
	p:set_cursor_enabled(false)
	p:exec_command("destroyMenu();", 1)
	p:exec_command("LoadLevel(\"level_0\");", 1.1)
end

function OnClicked_btn_opt_menu( )
	p:play_sound("event:/OnMenuAccept", 1.0, false)
	options_from = "pause"
	p:wait_escape_cancel()
	p:load_entities("options")
end

function OnClicked_btn_exit_pause( )
	p:play_sound("event:/OnMenuCancel", 1.0, false)
	p:wait_escape_cancel()
	p:print("exit..")
	p:exit_game()
end

function destroyMenu( )
	DestroyAllByTag("menu")
end