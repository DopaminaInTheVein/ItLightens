function OnLoading_title( )

end

function OnStart_title( )
	--ui_cam:fade_in(2)
	p:pause_game()
	--p:putText("title_text","It Lightens",0.3,0.6,"#FF0000FF",0.6,"#0000FFFF",3.5,0.2)
	p:clear_save()
	p:play_music("event:/OnMenuMusic", 0.4)
end

function OnClicked_btn_start( )
	p:play_sound("event:/OnMenuAccept", 1.0, false)
	p:set_cursor_enabled(false)
	p:exec_command("playVideoIntro();", 1)
	p:stop_music()
end

function OnClicked_btn_options( )
	options_from = "title"
	p:play_sound("event:/OnMenuAccept", 1.0, false)
	p:load_entities("options")
end

function playVideoIntro( )
	p:removeText("title_text")
	p:setup_game()
	--LoadLevel("level_1")
	ui_cam:fade_out(0.1)
	p:exec_command("StartTheGame();", 0.1)
end

function StartTheGame()
	LoadLevel("level_1")
	--p:play_video_and_do("data\\videos\\intro.avi", "LoadLevel(\"level_1\");")
end

function OnClicked_btn_exit( )
	p:play_sound("event:/OnMenuCancel", 1.0, false)
	p:exit_game()
end

function title_sense_pressed()
end