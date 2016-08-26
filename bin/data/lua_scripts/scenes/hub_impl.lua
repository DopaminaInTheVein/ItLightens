--==================
-- Level DATA
--==================
-------------------------------
mole_done = false
sci_done = false
-------------------------------
function OnStart_hub( )
	hub_first_time = true
	mole_done = true -- provisional
	sci_done = false
	p:player_talks("Bienvenido al hub", "scientific.dds", "SCI")
end
-------------------------------
function OnSave_hub()
	-- nothing to save
end
-------------------------------
idMoleSlept = 5
idSciSlept = 6
function OnLoad_hub()
	if mole_done and sci_done then
		end_hub_handles = HandleGroup()
		end_hub_handles:get_handles_by_tag("end_hub")
		end_hub_handles:awake()
	end
	if mole_done then
		h:get_handle_by_id(idMoleSlept)
		h:set_anim("run")
	end
	if sci_done then
		h:get_handle_by_id(idSciSlept)
		h:set_anim("run")
	end
end
-------------------------------
--===========================================
-- Events
--===========================================
function dream_mole()
	SaveLevel()
	LoadLevel("level_2")
end

function dream_sci()
	SaveLevel()
	LoadLevel("level_3")
end

function hub_end()
	SaveLevel()
	LoadLevel("level_4")
end
--=============================================

--==============================================================
-- Auxiliars
--==============================================================

--==============================================================
