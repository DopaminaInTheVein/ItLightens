try(DestroyDialog RL_Textures)catch()
rollout RL_Textures "IT Lightens!" width:541 height:440
(
	label lblTitle "SEARCH TEXTURES" pos:[84,17] width:102 height:18
	edittext edt1 "" pos:[39,43] width:479 height:15
	label lblSearch "Filter" pos:[11,43] width:29 height:12
	listbox lbx1 "ListBox" pos:[10,76] width:510 height:25
	
	local mm = MatManager()

	on edt1 entered text do
	(
		local res = mm.searchTexture edt1.text
		lbx1.items = res
	)
)
CreateDialog RL_Textures

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------------------