--------------------------- COMENTAR CODIGO TEST!!!! ---------------------------------------
function test_dbg()
  --h:get_handle_by_id(idDoor)
  --h:setLocked(0)
  --isDoorOpen = true
end
--------------------------- COMENTAR CODIGO TEST!!!! ---------------------------------------
print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle( )
h_elevator = Handle( )
hg = HandleGroup()
cam = Camera()
player = Player()

-- Creating Bomb
---------------------------------------------------------------------------------------------
function OnCreateBomb_ms3( )
	p:complete_tasklist(11)
end

function Ms3Victory()
	ui_cam:fade_out(1)
	p:exec_command("Credits()", 1)
end

function Credits()
	p:clear_level()
	p:stop_music()
	p:stop_sound("event:/OnFinalAlarm")
	p:exec_command("PlayVideoCredits();", 3)
end

function PlayVideoCredits()
	p:pause_game()
	p:play_video_and_do("data\\videos\\end.avi", "LoadLevel(\"level_0\");")
end







