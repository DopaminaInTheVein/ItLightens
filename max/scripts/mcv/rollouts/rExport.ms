try(DestroyDialog RL_Test)catch()
rollout RL_Export "IT Lightens!" width:296 height:900
(
	local selectedObj
	local tObj
	
	label 'lblObject' "OBJECT" pos:[124,12] width:48 height:16 align:#left
	GroupBox 'grpSelObj' "Select Object" pos:[16,25] width:232 height:48 align:#left
	label 'lblSelObj' "Selected Object:" pos:[32,46] width:88 height:16 align:#left
	pickbutton 'pckObj' "-- None --" pos:[128,45] width:112 height:16 align:#left
	label 'lblPrefabVal' "Prefab Value:" pos:[38,313] width:93 height:16 align:#left
	button 'btnExportFromObj' "Export From This Object" pos:[80,681] width:135 height:16 align:#left
	button 'btnModify' "Modify this" pos:[37,635] width:89 height:16 align:#left
	combobox 'cbx2' "ComboBox" pos:[229,414] width:0 height:1 align:#left
	label 'lbl29' "Label" pos:[32,4956] width:160 height:32 align:#left
	button 'btnModifyAll' "Modify [ $ ]" pos:[162,634] width:87 height:16 align:#left
	label 'lbl39' "SCENE" pos:[124,752] width:48 height:16 align:#left
	button 'btnExportScene' "Export Scene" pos:[13,775] width:269 height:24 align:#left
	GroupBox 'grpComponents' "Edit Components" pos:[15,219] width:259 height:492 align:#left
	dropdownList 'ddlComponents' "Select a component" pos:[40,239] width:213 height:40 align:#left
	label 'lblCurVal' "Current Value:" pos:[38,412] width:117 height:16 align:#left
	edittext 'txtPrefabVal' "" pos:[33,332] width:225 height:66 enabled:true align:#left
	checkbox 'chkComponent' "Enabled" pos:[44,284] width:69 height:19 align:#left
	
	edittext 'txtCurVal' "" pos:[33,431] width:225 height:66 enabled:true align:#left
	combobox 'cbx11' "New Value" pos:[227,546] width:0 height:1 align:#left
	label 'lbl82' "New Value:" pos:[36,544] width:117 height:16 align:#left
	edittext 'txtNewVal' "" pos:[31,563] width:225 height:66 enabled:true align:#left
	button 'btnResetPrefab' "Reset to Prefab" pos:[42,504] width:92 height:16 align:#left
	GroupBox 'grpPrefab' "Prefab" pos:[17,83] width:232 height:97 align:#left
	label 'lblPrefab' "Prefab:" pos:[36,101] width:87 height:17 align:#left
	label 'lblPrefabValue' "-- None --" pos:[81,101] width:62 height:17 align:#left
	button 'btnModifyPrefab' "Modify Prefab" pos:[148,503] width:91 height:16 align:#left
	label 'lblAssignPrefab' "Assign" pos:[36,122] width:61 height:17 align:#left
	edittext 'txtAssignPrefab' "" pos:[72,121] width:128 height:17 align:#left
	button 'btnAssignPrefab' "Assign" pos:[206,121] width:36 height:16 align:#left
	label 'lblNewPrefab' "New" pos:[35,155] width:26 height:17 align:#left
	edittext 'txtNewPrefab' "" pos:[59,153] width:103 height:17 align:#left
	button 'btnCreatePrefab' "Create" pos:[168,153] width:48 height:16 align:#left
	label 'lblComponents' "COMPONENTS" pos:[109,201] width:77 height:16 align:#left
	button 'btnWizard' "Wizard Value..." pos:[169,544] width:86 height:16 align:#left
	
	
	fn selectComponent sel = (
		local currentValue = tObj.getValor sel
		local enabled = (currentValue != undefined)
		chkComponent.state = enabled
		txtNewVal.readOnly = not enabled
		txtNewVal.text = ""
		if enabled do (
			txtCurVal.text = currentValue
		)
	)
	
	--Initialize
	on RL_Export open do
	(
		txtPrefabVal.readOnly 	= true
		txtCurVal.readOnly 		= true
		txtNewVal.readOnly 		= true
	)
	
	--Select Object
	on pckObj picked obj do
	(
		pckObj.text = obj.name
		selectedObj = obj
		
		tObj = TObject()
		tObj.setObj obj
		
		ddlComponents.items = #()
		listaComponentes = tObj.getComponentList()
		for comp in listaComponentes do (
			local item = comp.nombre
			format "%\n" item
			ddlComponents.items = append ddlComponents.items item
		)
		
		selectComponent 1
	)
	
	--Select Component
	on ddlComponents selected sel do
	(
		selectComponent sel
	)
	

	
	
	on btnExportFromObj pressed do
	(
	
	)
	on btnModify pressed do
	(
		tObj.setValor ddlComponents.selected txtNewVal.text
		txtCurVal.text = txtNewVal.text
		txtNewVal.text = ""
	)
	on btnModifyAll pressed do
	(
		for objS in $ do (
			local tObj2 = TObject()
			tObj2.setObj objS
			tObj2.setValor ddlComponents.selected txtNewVal.text
		)
		selectComponent ddlComponents.selection
	)
	on btnExportScene pressed do
	(
	
	)

	on chkComponent changed state do
		(
	
	)
	on btnResetPrefab pressed do
	(
	
	)
	on btnModifyPrefab pressed do
		(
	
	)
	on btnAssignPrefab pressed do
	(
	
	)
	on btnCreatePrefab pressed do
	(
	
	)
	on btnWizard pressed do
	(
	
	)
)
CreateDialog RL_Export

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
