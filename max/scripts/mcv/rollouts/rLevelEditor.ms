-- Rollout Level Editor
try(DestroyDialog RL_LevelEditor)catch()
rollout RL_LevelEditor "IT Lightens! Editor" width:250 height:542
(
	local selectedObj = undefined
	
	label 'lblTitle' "LEVEL EDITOR" pos:[85,8] width:79 height:16 align:#left
	button 'btnCreate' "Create" pos:[23,92] width:212 height:24 align:#left
	
	dropdownList 'ddlType' "Object type" pos:[23,47] width:212 height:40 align:#left
	GroupBox 'grpNewObject' "New Object" pos:[14,29] width:228 height:97 align:#left

	GroupBox 'gprHelper' "Add Helper (to selected)" pos:[10,328] width:226 height:95 align:#left
	label 'lblSelection' "Selected:" pos:[28,156] width:44 height:16 align:#left
	pickbutton 'pckObj' "-- None --" pos:[80,156] width:152 height:16 align:#left
	dropdownList 'ddlHelper' "Helper type" pos:[23,348] width:201 height:40 align:#left
	button 'btnHelper' "Add Helper" pos:[20,394] width:204 height:20 align:#left
	button 'btnExport' "Export Level" pos:[33,495] width:184 height:31 align:#left

	
	GroupBox 'grpProps' "Edit Properties (from selected)" pos:[10,197] width:228 height:117 align:#left
	dropdownList 'ddlProperties' "Property" pos:[20,215] width:203 height:40 align:#left
	label 'lblValue' "Value" pos:[24,262] width:33 height:19 align:#left
	edittext 'txtValue' "" pos:[55,260] width:166 height:17 align:#left
	button 'btnModify' "Modify" pos:[59,284] width:161 height:20 align:#left
	GroupBox 'grpSelection' "Select Object / Helper" pos:[11,134] width:228 height:49 align:#left
	
	fn updateProperties = (
		txtValue.text = ""
		local selProp = ddlProperties.selected
		if selProp != undefined do (
			local prop = getUserProp selectedObj ddlProperties.selected
			if prop != undefined do (
				txtValue.text = (prop as string)
			)
		)
	)	
	
	GroupBox 'grpExport' "Export Level (XML)" pos:[11,436] width:228 height:97 align:#left
	label 'lblExport' "Name" pos:[26,465] width:33 height:19 align:#left
	edittext 'txtExport' "" pos:[57,464] width:166 height:17 align:#left
	on RL_LevelEditor open do
	(
		ddlType.items = (getAllMaxIn objMax_dir)
		ddlHelper.items = (getAllMaxIn helperMax_dir)
	)
	on btnCreate pressed do
	(
		drawObj objMax_dir ddlType.selected
	)
	on pckObj picked obj do
	(
		selectedObj = obj
		pckObj.text = selectedObj.name
		ddlProperties.items = getEditableProps obj
		--
		updateProperties()
	)
	on btnHelper pressed do
	(
		if selectedObj == undefined then (
			messageBox "Debes seleccionar un objeto!"
		) else (
			local obj = drawObj helperMax_dir ddlHelper.selected
			if obj != undefined then ( 
				obj.parent = selectedObj
			)
		)
	)
	on btnExport pressed do
	(
		local xml = _parseLevel()
		messagebox (xml.parse())
		local fsm = createFile (scenes_dir + txtExport.text + ".xml")
		format (xml.parse()) to:fsm
		close fsm
	)
	on ddlProperties selected sel do
	(
		updateProperties()
	)
	on btnModify pressed do
	(
		if selectedObj == undefined then (
			messageBox "Debes seleccionar un objeto y una propiedad!"
		) else (
			if ddlProperties.selected == undefined then (
				messageBox "Debes seleccionar una propiedad!"
			) else (
				setUserProp selectedObj ddlProperties.selected txtValue.text 
			)
		)
	)
)
CreateDialog RL_LevelEditor

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------