p = Public()

function OnClicked_btn_retry( )
	p:play_sound("event:/OnMenuAccept", 1.0, false)
	p:exec_command("destroyMenuDead();", 1)
	LoadLevel(g_current_level)
end

function OnClicked_btn_back_title_dead( )
	p:play_sound("event:/OnMenuCancel", 1.0, false)
	p:exec_command("destroyMenuDead();", 1)
	LoadLevel("level_0")
end

function OnClicked_btn_exit_dead( )
	p:play_sound("event:/OnMenuCancel", 1.0, false)
	p:exit_game()
end

function destroyMenuDead( )
	menu_handles = HandleGroup()
	menu_handles:get_handles_by_tag("menu")
	menu_handles:destroy()
end

function dead_sense_pressed()
end