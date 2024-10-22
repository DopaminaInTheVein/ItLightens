-- Rollout Level Editor
try(DestroyDialog RL_LevelEditor)catch()
rollout RL_LevelEditor "IT Lightens! Editor" width:250 height:644
(
	local selectedObj = undefined
	local transformCopied
	local reservedProps = #(
		"typeObject", "typeHelper", "mesh", "staticMesh"
	)
	label lblTitle "LEVEL EDITOR" pos:[85,8] width:79 height:16
	button btnCreate "Create" pos:[23,92] width:212 height:24
	
	dropdownList ddlType "Object type" pos:[23,47] width:212 height:40
	GroupBox grpNewObject "New Object" pos:[14,29] width:228 height:97

	GroupBox gprHelper "Add Helper (to selected)" pos:[10,368] width:226 height:95
	label lblSelection "Selected [*]:" pos:[28,156] width:66 height:16
	pickbutton pckObj "-- None --" pos:[94,156] width:138 height:16
	dropdownList ddlHelper "Helper type" pos:[23,388] width:201 height:40
	button btnHelper "Add Helper (to [*])" pos:[20,434] width:204 height:20
	button btnExport "Export Level" pos:[25,528] width:195 height:19

	
	GroupBox grpProps "Edit Properties (from selected)" pos:[10,237] width:228 height:117
	dropdownList ddlProperties "Property" pos:[20,255] width:203 height:40
	label lblValue "Value" pos:[24,302] width:33 height:19
	edittext txtValue "" pos:[55,300] width:166 height:17
	button btnModify "Modify[*]" pos:[58,324] width:75 height:20
	GroupBox grpSelection "Selection" pos:[11,134] width:228 height:73
	
	fn updateProperties = (
		txtValue.text = ""
		local selProp = ddlProperties.selected
		if selProp != undefined do (
			local prop = getUserProp selectedObj ddlProperties.selected
			if prop != undefined do (
				txtValue.text = (prop as string)
			)
			local index = findItem reservedProps selProp 
			txtValue.enabled = true --(index == 0)
		)
	)	
	
	GroupBox grpExport "Export Level (XML)" pos:[11,476] width:228 height:97
	label lblExport "Name" pos:[26,505] width:33 height:19
	edittext txtExport "" pos:[57,504] width:166 height:17
	button btnModifyAll "Modify All ($)" pos:[137,324] width:79 height:20
	button btnSelectEquivalents "Select Equivalents" pos:[134,179] width:94 height:20
	button btnSelectSame "Select Same Type" pos:[23,179] width:94 height:20
	
	fn selectSame sameParentNeeded = (
		if selectedObj == undefined then (
			messageBox "Debes seleccionar un objeto!"
			return #abort
		)
		
		-- Nombre de propiedad (type) y valor propiedad (valueType)
		local type = undefined
		local valueType   = undefined
		local typesSearch = #("typeObject", "typeHelper")
		for ts in typesSearch do (
			valueType = getUserProp selectedObj ts
			if valueType != undefined then (
				type = ts
				exit
			)
		)
		
		-- No es de ninguno de los tipos conocidos (typeObject, typeHelper)
		if type == undefined then (return #ok)

		--Se requiere que la raiz del candidato comparta el del padre?
		local typeObjectRequired = undefined
		if (sameParentNeeded) then (
			local root = getHierarchyRoot selectedObj
			typeObjectRequired = getUserProp root "typeObject"
		)
 		
		-- Seleccionamos los objetos que cumplan la condici�n
		clearSelection()
		for obj in $* do (
			if (getUserProp obj type == valueType) then (
				--TypeObjectRequired?
				if (sameParentNeeded) then (
					local rootObj = getHierarchyRoot obj
					typeObj = getUserProp rootObj "typeObject"
					if (typeObj == typeObjectRequired) then (
						selectMore obj
					)
				) else (
					selectMore obj
				)
			)
		)	
	)
	
	button btnCopyPos "Copy Position $" pos:[14,585] width:90 height:24
	
	button btnPastePos "Paste Positionn $" pos:[116,585] width:95 height:24
	button btnLights "Export Lights" pos:[25,552] width:89 height:15
	label lbl5 "( add _lights on name)" pos:[118,552] width:110 height:16
	on RL_LevelEditor open do
	(
		ddlType.items = (getAllMaxIn objMax_dir)
		ddlHelper.items = #()--(getAllMaxIn helperMax_dir)
	)
	on btnCreate pressed do
	(
		local obj = drawObj objMax_dir ddlType.selected
		setUserProp obj "mesh_protected" "yes"
	)
	on pckObj picked obj do
	(
		selectedObj = obj
		pckObj.text = selectedObj.name
		local descriptor = OManager.getDescriptor obj
		local propList = getEditableProps obj
		for p in descriptor.getAttributes() do (appendifUnique propList p)
		ddlProperties.items = propList
		ddlHelper.items = descriptor.getHelpers()
		
		-- Actualizamos propiedades
		updateProperties()
		return #ok
		-- Seleccionamos objeto de verdad
		--if ($ != undefined) then (
		--	clearSelection()
		--	selectMore selectedObj
		--)
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
		if txtExport.text == "" then (
			txtExport.text = (filterstring maxFileName ".")[1]
		)
		-- Fill default values
		assign_default_room_to_all()
		fixUserProps()
		
		local xml, xml_init
		local fileName, fileNameInit
		local fsm, fsm_init
		
		local is_ui = _is_ui_scene()
		if is_ui then (
			xml = _parseLevel "ui_scene"
			fileName = (scenes_dir + txtExport.text + ".xml")
			fsm = createFile fileName
		) else (
			xml = _parseLevel ""
			xml_init = _parseLevel "init"
			
			fileName = (scenes_dir + txtExport.text + ".xml")
			fileNameInit = (scenes_dir + txtExport.text + "_init.xml")
			
			fsm = createFile fileName
			fsm_init = createFile fileNameInit
		)
		
		try (
			format (xml.parse()) to:fsm
			if (xml_init != undefined) then (format (xml_init.parse()) to:fsm_init)
		) catch (
			close fsm
			if (fsm_init != undefined) then (close fsm_init)
			DestroyDialog RL_LevelEditor
			throw()				
		)
		close fsm
		if (fsm_init != undefined) then (close fsm_init)
		if _lua_code != "" then (
			local fLua = createFile (lua_dir + "scenes\\" + txtExport.text + ".lua")
			format _lua_code to:fLua
			close fLua
			_lua_code = ""
		)
		messagebox ("Nivel exportado correctamente: \n(" + fileName + ")")
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
				messageBox "Ok!"
			)
		)
	)
	on btnModifyAll pressed do
	(
		if $ == undefined then (
			messageBox "Nada seleccionado! (Recuerda: selecci�n real ($), no virtual [*])"
		) else (
			local objs = $
			if classof objs != ObjectSet then ( objs = #(objs) )
			local objNoProp = #() -- getObjNoProp objs ddlProperties.selected
			for obj in objs do (
				if (getUserProp obj ddlProperties.selected == undefined) then (
					append objNoProp obj
				)
			)
			
			if objNoProp.count > 0 then (
				msg = "Hay objetos que no tienen la propiedad '" + ddlProperties.selected + "', estos no se ver�n afectados.\n"
				msg += "�Desea continuar?\n"
				for o in objNoProp do (
					msg += "\n- " + o.name 
				)
				
				local resp = (queryBox msg)
				if (not resp) do (
					return #abort
				)
			)
			
			for obj in objs do (
				--if (getUserProp obj ddlProperties.selected != undefined) then (
				setUserProp obj ddlProperties.selected txtValue.text
				--)
			)
		)
	)
	on btnSelectEquivalents pressed do
	(
		selectSame true
	)
	on btnSelectSame pressed do
	(
		selectSame false
	)
	on btnCopyPos pressed do
		transformCopied = $.transform
	on btnPastePos pressed do
		$.transform = transformCopied
	
	
	on btnLights pressed do
	(
		if txtExport.text == "" then (
			txtExport.text = (filterstring maxFileName ".")[1]
		)
		-- Fill default values
		assign_default_room_to_all()
		fixUserProps()
		
		xml = _parseLevel "lights"
		fileName = (scenes_dir + txtExport.text + "_lights.xml")
		fsm = createFile fileName
		
		try (
			format (xml.parse()) to:fsm
		) catch (
			close fsm
			DestroyDialog RL_LevelEditor
			throw()				
		)
		close fsm
		messagebox ("Luces exportadas OK \n(" + fileName + ")")
	)
)
CreateDialog RL_LevelEditor

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------