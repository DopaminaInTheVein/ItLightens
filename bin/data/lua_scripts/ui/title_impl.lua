
function OnStart_title( )
	p:clear_save()
end

function OnClicked_btn_start( )
	cam:fade_out(1)
	p:exec_command("playVideoIntro();", 1)
end

function playVideoIntro( )
	LoadLevel("level_1")
	p:play_video_and_do("data\\videos\\intro.avi", "LoadLevel(\"level_1\");")
end

function OnClicked_btn_exit( )
	p:exit_game()
end