
function OnStart_title( )
	p:putText("title_text","It Lightens",1.25,0.3,"#FF0000FF",3.5,"#0000FFFF",0.5,0.1)
	p:clear_save()
	p:exec_command("waitAndDoTitleSmaller()",2.0)
end

function OnClicked_btn_start( )
	--ui_cam:fade_out(1)
	p:exec_command("playVideoIntro();", 1)
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
	p:alterText("title_text",1.25,0.3,3.5)
	p:exec_command("waitAndDoTitleSmaller()",1.5)
end

function waitAndDoTitleSmaller()
	p:alterText("title_text",1.25,0.3,3.3)
	p:exec_command("waitAndDoTitleBigger()",1.5)
end