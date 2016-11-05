-- GUI
---------------------------------------------------
function OnCreateGui( param )
	p:print("OnCreateGui: "..param)
	CallFunction("OnCreateGui_"..param)
end
function OnMouseOver( param )
	p:print("OnMouseOver")
	CallFunction("OnMouseOver_"..param)
end

function OnMouseUnover( param )
	p:print("OnMouseUnover")
	CallFunction("OnMouseUnover_"..param)
end

function OnPressed( param )
	p:print("OnPressed")
	p:print("OnPressed")
	CallFunction("OnPressed_"..param)
end

function OnClicked( param )
	p:print("OnClicked")
	CallFunction("OnClicked_"..param)
end

function OnChoose( name, option )
	p:print("OnChoose: "..name.." "..option)
	CallFunctionParam("OnChoose_"..name, option)
end

function OnValueChanged( name, value )
	p:print("OnChoose: "..name.." "..value)
	CallFunctionParam("OnValueChanged_"..name, value)
end

function OnPause( )
	p:print("OnPause")
	if not g_dead then
		p:load_entities("menu")
	end
end