function OnStart_title( )
	ui_cam:fade_in(2)
	p:pause_game()
	p:putText("title_text","*RIGHT_CLICK*It Lightens",0.3,0.6,"#FF0000FF",0.6,"#0000FFFF",3.5,0.2)
	--p:putText("title_text",p:get_text("title", "test"),0.1, 0.4,"#FF0000FF",0.2,"#0000FFFF",0.5,0.1)	
	p:clear_save()
	p:play_music("event:/OnMenuMusic", 0.4)
	--p:exec_command("waitAndDoTitleSmaller()",2.0)
end

function OnClicked_btn_start( )
	--ui_cam:fade_out(1)
	p:set_cursor_enabled(false)
	p:exec_command("playVideoIntro();", 1)
	p:stop_music()
end

function OnClicked_btn_options( )
	p:load_entities("options")
end

function playVideoIntro( )
	p:removeText("title_text")
	p:setup_game()
	LoadLevel("level_1")
	--p:play_video_and_do("data\\videos\\intro.avi", "LoadLevel(\"level_1\");")
end

function OnClicked_btn_exit( )
	p:exit_game()
end

function waitAndDoTitleBigger()
	p:alterText("title_text",0.2,0.6,1.0)
	p:exec_command("waitAndDoTitleSmaller()",1.5)
end

function waitAndDoTitleSmaller()
	p:alterText("title_text",0.2,0.6,0.9)
	p:exec_command("waitAndDoTitleBigger()",1.5)
end

function title_sense_pressed()
end