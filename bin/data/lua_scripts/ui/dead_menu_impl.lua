p = Public()

function OnCreateGui_btn_retry()
	hud_elements:get_handles_by_tag("hiddeable_hud")
	hud_elements:set_visible(0)
end

function OnClicked_btn_retry( )
	p:play_sound("event:/OnMenuAccept", 1.0, false)
	p:exec_command("destroyMenuDead();", 1)
	g_restarting = true
	LoadLevel(g_current_level)
end

function OnClicked_btn_back_title_dead( )
	p:play_sound("event:/OnMenuCancel", 1.0, false)
	p:exec_command("destroyMenuDead();", 1)
	LoadLevel("level_0")
end

function OnClicked_btn_exit_dead( )
	OnClicked_btn_back_title_dead()
	-- Exit --> Back to menu!
	-- p:play_sound("event:/OnMenuCancel", 1.0, false)
	-- p:exit_game()
end

function destroyMenuDead( )
	menu_handles = HandleGroup()
	menu_handles:get_handles_by_tag("menu")
	menu_handles:destroy()
end

function dead_sense_pressed()
end