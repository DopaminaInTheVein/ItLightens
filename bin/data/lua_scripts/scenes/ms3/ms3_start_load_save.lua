---------------------------- LEVEL Init, Load, Save ---------------------------------------------------
function auxiliarMusic()
	p:play_music("event:/OnGameMusic", 0.2)
end

function OnCinematicEnd_CinematicIntro()
	p:exec_command("p:setControlEnabled(1)", 0.2)
end

function OnStart_ms3()
  p:print("OnStarted Scene MS3")
  InitFogMs3()
  InitPilaMs3()
  
  isDoorOpen = false
  alert = false
  stateElevator = 1 -- 1= up , 0 = down
  cp_elevator = false
  cp_door_opened = false
  p:play_music("event:/OnGameMusic", 0.2)
  p:setControlEnabled(0)
  cam:run_cinematic("CinematicIntro", 10)
  p:wait_button("cam:skip_cinematic()")
  p:ai_start()
end

function OnSave_ms3()
	d = Data()
	d:put_float("elevator_state", stateElevator)
	d:put_bool("door_open", isDoorOpen)
	d:put_bool("alert", alert)
	d:put_bool("cp_elevator", cp_elevator)
	d:put_bool("cp_door_opened", cp_door_opened)
	SaveFogMs3(d)
	d:write()
end

function OnLoad_ms3()
  p:print("OnLoaded Scene MS3")
  p:ai_start()
  d = Data()
  LoadFogMs3(d)
  isDoorOpen = d:get_bool("door_open")
  alert = d:get_bool("alert")
  stateElevator = d:get_float("elevator_state")
  cp_elevator = d:get_bool("cp_elevator")
  cp_door_opened = d:get_bool("cp_door_opened")
  p:play_music("event:/OnGameMusic", 0.2)
end
---------------------------------------------------------------------------------------------