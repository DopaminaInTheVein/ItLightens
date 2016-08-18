p = Public()

function OnClicked_button_resume( )
	menu_handles = HandleGroup()
	menu_handles:get_handles_by_tag("menu")
	menu_handles:destroy()
	p:resume()
end

function OnClicked_button_restart( )
	LoadLevel("level_1")
end

function OnClicked_button_exit( )
	LoadLevel("level_1")
end