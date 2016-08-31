p = Public()

function OnClicked_btn_retry( )
	p:exec_command("destroyMenuDead();", 1)
	LoadLevel(g_current_level)
end

function OnClicked_btn_back_title_dead( )
	p:exec_command("destroyMenuDead();", 1)
	LoadLevel("level_0")
end

function OnClicked_btn_exit_dead( )
	p:exit_game()
end

function destroyMenuDead( )
	menu_handles = HandleGroup()
	menu_handles:get_handles_by_tag("menu")
	menu_handles:destroy()
end