---------------- Andamio ----------------------------------------
ontablon1 = false
ontablon2 = false
ontablon3 = false
ontablon4 = false
ontablon5 = false
ontablon6 = false

function noTablon()
	p:print("noTablon...")
	if not ontablon1 and not ontablon2 and not ontablon3 and not ontablon4 and not ontablon5 and not ontablon6 then
		p:print("...noTablon: skipCinematic")
		cam:skip_cinematic();
	end
end

function ms3_outTablon1()
	p:print("Out Tablon")
	ontablon1 = false
	p:exec_command("noTablon();", 0.3)
end

function ms3_outTablon2()
	p:print("Out Tablon")
	ontablon2 = false
	p:exec_command("noTablon();", 0.3)
end

function ms3_outTablon3()
	p:print("Out Tablon")
	ontablon3 = false
	p:exec_command("noTablon();", 0.3)
end

function ms3_outTablon4()
	p:print("Out Tablon")
	ontablon4 = false
	p:exec_command("noTablon();", 0.3)
end

function ms3_outTablon5()
	-- p:print("Out Tablon")
	-- ontablon5 = false
	-- p:exec_command("noTablon();", 0.5)
end

function ms3_outTablon6()
	-- p:print("out Tablon6")
	-- ontablon1 = false
	-- --cam:run_cinematic("CineTablon1", 5)
end

function ms3_onTablon1()
	p:print("onTablon2")
	ontablon1 = true
	cam:run_cinematic("CineTablon1", 5)
end

function ms3_onTablon2()
	p:print("onTablon2")
	ontablon2 = true
	cam:run_cinematic("CineTablon2", 5)
end

function ms3_onTablon3()
	p:print("onTablon3")
	ontablon3 = true
	cam:run_cinematic("CineTablon3", 5)
end

function ms3_onTablon4()
	p:print("onTablon4")
	ontablon4 = true
	cam:run_cinematic("CineTablon4", 5)
end

function ms3_onTablon5()
	p:print("onTablon5")
	ms3_outTablon4()
	--cam:run_cinematic("CineTablon5", 5)
	cam:skip_cinematic()
end

function ms3_onTablon6()
	--p:print("onTablon6")
	--Mejor sin
	--cam:run_cinematic("CineTablon6", 5)
end