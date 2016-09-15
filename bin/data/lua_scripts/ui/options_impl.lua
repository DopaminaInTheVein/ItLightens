
TXT_OPTION_AXIS_Y_NORMAL = "Normal"
TXT_OPTION_AXIS_Y_INVERTED = "Inverted"

function OnCreateGui_opt_axisY( )
	h:getHandleCaller()
	GUI_OPTION_AXIS_Y_NORMAL = h:add_option(TXT_OPTION_AXIS_Y_NORMAL)
	GUI_OPTION_AXIS_Y_INVERTED = h:add_option(TXT_OPTION_AXIS_Y_INVERTED)
end

function OnClicked_btn_opt_back( )
	destroyOptions()
	p:print(TXT_OPTION_AXIS_Y_NORMAL)
	p:print(GUI_OPTION_AXIS_Y_NORMAL)
	p:print(TXT_OPTION_AXIS_Y_INVERTED)
	p:print(GUI_OPTION_AXIS_Y_INVERTED)
end

function destroyOptions( )
	DestroyAllByTag("options")
end