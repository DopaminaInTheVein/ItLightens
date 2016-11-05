-- On Detected Door_centinels --
--------------------------------
hDoor = Handle()
alert = false
centinel_1 = Handle()
centinel_2 = Handle()

function centinelDetection( )
  if not alert then
	alert = true
	hDoor:get_handle_by_id(idDoor)
	p:exec_command("hDoor:setLocked(-10)", 0.5)
	p:exec_command("centinelRelax();", 2)
  end
end

function centinelRelax( )
  p:print("Centinel relax?")
  if alert then
	alert = false
	cent1_ok = centinel_1:is_valid()
	cent2_ok = centinel_2:is_valid()
	if cent1_ok then
		--Is patrolling ?
		patrol_1 = centinel_1:is_patrolling()
		if not patrol_1 then
			alert = true
		else
			--Guard in his position?
			come_back = centinel_1:is_come_back()
			if not come_back then
				alert = true
			end
		end
	end
	  
	if cent2_ok then
		--Is patrolling ?
		patrol_2 = centinel_2:is_patrolling()
		if not patrol_2 then
			alert = true
		else
			--Guard in his position?
			come_back = centinel_2:is_come_back()
			if not come_back then
				alert = true
			end
		end
	end
	  
	if alert then
		p:exec_command("centinelRelax();", 2)
	else
		p:exec_command("alert_finish()", 1)
	end
  end
end

function alert_finish( )
  if not alert then
    if isDoorOpen then
		hDoor:setLocked(0);
	end
  end
end

function OnDetected_guard_004( )
	centinel_1:getHandleCaller()
	centinelDetection()
end

function OnDetected_guard_005( )
	centinel_2:getHandleCaller()
	centinelDetection()
end