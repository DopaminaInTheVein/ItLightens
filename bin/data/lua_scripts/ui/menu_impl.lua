p = Public()

function OnClicked_btn_resume( )
	destroyMenu()
	p:resume()
end

function OnClicked_btn_restart( )
	p:exec_command("destroyMenu();", 1)
	LoadLevel(g_current_level)
end

function OnClicked_btn_back_title( )
	p:exec_command("destroyMenu();", 1)
	LoadLevel("level_0")
end

function OnClicked_btn_exit_pause( )
	p:exit_game()
end

function destroyMenu( )
	menu_handles = HandleGroup()
	menu_handles:get_handles_by_tag("menu")
	menu_handles:destroy()
end