p = Public()

function OnClicked_btn_resume( )
	p:set_cursor_enabled(false)
	destroyMenu()
	p:resume()
end

function OnClicked_btn_restart( )
	-- (Fadeout)
	p:set_cursor_enabled(false)
	p:exec_command("destroyMenu();", 1)
	p:exec_command("LoadLevel(g_current_level);", 1.1)
end

function OnClicked_btn_back_title( )
	-- (Fadeout)
	p:set_cursor_enabled(false)
	p:exec_command("destroyMenu();", 1)
	p:exec_command("LoadLevel(\"level_0\");", 1.1)
end

function OnClicked_btn_opt_menu( )
	p:load_entities("options")
end

function OnClicked_btn_exit_pause( )
	p:print("exit..")
	p:exit_game()
end

function destroyMenu( )
	DestroyAllByTag("menu")
end

function menu_sense_pressed()
end