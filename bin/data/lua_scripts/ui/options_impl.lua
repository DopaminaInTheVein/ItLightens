
TXT_OPTION_AXIS_Y_NORMAL = "Normal"
TXT_OPTION_AXIS_Y_INVERTED = "Inverted"

function OnCreateGui_opt_axisY( )
	h:getHandleCaller()
	GUI_OPTION_AXIS_Y_NORMAL = h:add_option(TXT_OPTION_AXIS_Y_NORMAL)
	GUI_OPTION_AXIS_Y_INVERTED = h:add_option(TXT_OPTION_AXIS_Y_INVERTED)
end

function OnChoose_opt_axisY(option)
	switch(option) {
		[GUI_OPTION_AXIS_Y_NORMAL] = function() val = 0 end,
		[GUI_OPTION_AXIS_Y_INVERTED] = function() val = 1 end,
		default = function() val = -1 end, -- Esto no puede pasar
	}
	p:json_edit("options.json", "controls", "y-axis_inverted", val)
end

function OnClicked_btn_opt_back( )
	destroyOptions()
end

function destroyOptions( )
	DestroyAllByTag("options")
end