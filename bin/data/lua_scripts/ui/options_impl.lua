--TEXTS--
---------------------------------------
TXT_OPTION_AXIS_NORMAL = "Normal"
TXT_OPTION_AXIS_INVERTED = "Inverted"
---------------------------------------
DIFF_SIZE = 0
TXT_DIFFICULTIES = {}
function DECLARE_DIFFICULTY(name)
	TXT_DIFFICULTIES[DIFF_SIZE] = name
	DIFF_SIZE = DIFF_SIZE + 1
end
---------------------------------------
DECLARE_DIFFICULTY("Easy")
DECLARE_DIFFICULTY("Normal")
DECLARE_DIFFICULTY("Hard")
---------------------------------------

-- EVENTS --
-------------------------------------------------------------------------------

--Create Guis
function OnCreateGui_opt_diff( )
	InitDifficulty()
end
function OnCreateGui_opt_axisX( )
	InitOptAxis("x")
end
function OnCreateGui_opt_axisY( )
	InitOptAxis("y")
end

--Choose Options
function OnChoose_opt_axisX(option)
	SaveAxis(option, "x")
end
function OnChoose_opt_axisY(option)
	SaveAxis(option, "y")
end
function OnChoose_opt_diff(option)
	SaveDifficulty(option)
end

--Click go back
function OnClicked_btn_opt_back( )
	destroyOptions()
end

-- Auxiliar functions --
-------------------------------------------------------------------------------
-- Difficulty
function InitDifficulty()
	h:getHandleCaller()
	p:print(DIFF_SIZE)
	-- h:add_option(TXT_DIFFICULTIES[0])
	-- h:add_option(TXT_DIFFICULTIES[1])
	-- h:add_option(TXT_DIFFICULTIES[2])
	for i=0, DIFF_SIZE-1 do
		h:add_option(TXT_DIFFICULTIES[i])
	end
	diff = p:json_read("options.json", "game", "difficulty")
	p:print(diff)
	h:select_option(diff)--math.floor(dif))
	-- h:select_option(math.floor(dif))
end

function SaveDifficulty(option)
	p:json_edit("options.json", "game", "difficulty", option)
end

--Axis
function InitOptAxis(axis)
	h:getHandleCaller()
	GUI_OPTION_AXIS_NORMAL = h:add_option(TXT_OPTION_AXIS_NORMAL)
	GUI_OPTION_AXIS_INVERTED = h:add_option(TXT_OPTION_AXIS_INVERTED)
	axis_inverted = p:json_read("options.json", "controls", axis.."-axis_inverted")
	if axis_inverted == 1.0 then
		h:select_option(GUI_OPTION_AXIS_INVERTED)
	else
		h:select_option(GUI_OPTION_AXIS_NORMAL)
	end	
end
function SaveAxis(option, axis)
	switch(option) {
		[GUI_OPTION_AXIS_NORMAL] = function() val = 0 end,
		[GUI_OPTION_AXIS_INVERTED] = function() val = 1 end,
		default = function() val = -1 end, -- Esto no puede pasar
	}
	p:json_edit("options.json", "controls", axis.."-axis_inverted", val)
end

--Destroy Menu
function destroyOptions( )
	DestroyAllByTag("options")
end