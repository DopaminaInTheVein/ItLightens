-- Rollout Level Editor
try(DestroyDialog RL_LevelEditor)catch()
rollout RL_LevelEditor "IT Lightens! Editor" width:250 height:120
(
	local selectedObj
	local tObj
	
	label 'lblTitle' "LEVEL EDITOR" pos:[85,8] width:79 height:16 align:#left
	button 'btnCreate' "Create" pos:[23,77] width:212 height:24 align:#left
	
	dropdownList 'ddlType' "Object type" pos:[23,33] width:212 height:40 align:#left
	on RL_LevelEditor open  do
	(
		getAllObjectTypes()
		ddlType.items = getAllObjectTypes()
	)
	on btnCreate pressed do
	(
		drawObj ddlType.selected
	)
	on ddlType selected sel do
	(
	
	)

)
CreateDialog RL_LevelEditor

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------