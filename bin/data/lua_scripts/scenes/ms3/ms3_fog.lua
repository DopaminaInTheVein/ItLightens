--FOG_STATE = 0 -- (Sala 1)
--FOG_STATE = 1 -- (Sala 2)

MS3_FOG_1 = -22
MS3_FOG_2 = -1
MS3_FOG_FADE_POS_X = 0
MS3_FOG_FADE_POS_Y = 0
MS3_FOG_FADE_POS_Z = 0

function InitFogMs3()
	FOG_STATE = 0
	cam:fog_y(MS3_FOG_1)
	cam:fx(FX_FOG, 1)
	GetMs3FogConstants()
end

function SaveFogMs3(data)
	data:put_bool("fog_state", FOG_STATE)
end

function LoadFogMs3(data)
	FOG_STATE = data:get_bool("fog_state")
	local hfog = MS3_FOG_1
	if FOG_STATE == 1 then hfog = MS3_FOG_2 end
	cam:fx(FX_FOG, 1)
	cam:fog_y(hfog)
	GetMs3FogConstants()
end

function GetMs3FogConstants()
	h:get_handle_by_name_tag("_Fog1", "fog")
	MS3_FOG_1 = h:get_y()
	h:get_handle_by_name_tag("_Fog2", "fog")
	MS3_FOG_2 = h:get_y()
	MS3_FOG_FADE_POS_X = h:get_x()
	MS3_FOG_FADE_POS_Y = h:get_y()
	MS3_FOG_FADE_POS_Z = h:get_z()
end

--- FOG Events ---
function FogCorridorIn()
	GetMs3FogConstants()
	p:breakpoint(1)
	--cam:fog_y(MS3_FOG_2)
	cam:fog_fade(MS3_FOG_FADE_POS_X, MS3_FOG_FADE_POS_Y, MS3_FOG_FADE_POS_Z, MS3_FOG_1, MS3_FOG_2, 0.2, 20)
end

function FogCorridorOut1()
	GetMs3FogConstants()
	p:breakpoint(2)
	cam:fog_unfade()
	cam:fog_y(MS3_FOG_1)
	h:get_handle_caller()
end

function FogCorridorOut2()
	GetMs3FogConstants()
	p:breakpoint(3)
	cam:fog_unfade()
	cam:fog_y(MS3_FOG_2)
end