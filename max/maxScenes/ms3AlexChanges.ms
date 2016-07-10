function addAlexLogicCamaras = (
	if $__camara != undefined then (
		delete $__camara
	)
	-- CAMARAS DETECTORAS
	local auxobj =  $camara_centro
	setUserProp auxobj "typeObject" "detection_cam"
	setUserProp auxobj "staticMesh" "ms3/camara_derecha"
	setUserProp auxobj "mesh" "ms3/camara_derecha"
	setUserProp auxobj "range" 6
	setUserProp auxobj "width" 3
	setUserProp auxobj "rot_speed_sonar" 0.45
	setUserProp auxobj "rotating_left" false
	setUserProp auxobj "max_rotation" 162
	auxobj.rotation.x_rotation = 0
	auxobj.rotation.y_rotation = 0
	auxobj.rotation.z_rotation = 0
	local currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 171))
	auxobj.transform = currentMatrix
		
	auxobj =  $camara_derecha
	setUserProp auxobj "typeObject" "detection_cam"
	setUserProp auxobj "staticMesh" "ms3/camara_derecha"
	setUserProp auxobj "mesh" "ms3/camara_derecha"
	setUserProp auxobj "range" 6
	setUserProp auxobj "width" 3
	setUserProp auxobj "rot_speed_sonar" 0.25
	setUserProp auxobj "rotating_left" false
	setUserProp auxobj "max_rotation" 90
	auxobj.rotation.x_rotation = 0
	auxobj.rotation.y_rotation = 0
	auxobj.rotation.z_rotation = 0
	currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 45))
	auxobj.transform = currentMatrix

	auxobj =  $camara_izquierdaq
	setUserProp auxobj "typeObject" "detection_cam"
	setUserProp auxobj "staticMesh" "ms3/camara_derecha"
	setUserProp auxobj "mesh" "ms3/camara_derecha"
	setUserProp auxobj "range" 6
	setUserProp auxobj "width" 3
	setUserProp auxobj "rot_speed_sonar" 0.25
	setUserProp auxobj "rotating_left" true
	setUserProp auxobj "max_rotation" 90
	auxobj.rotation.x_rotation = 0
	auxobj.rotation.y_rotation = 0
	auxobj.rotation.z_rotation = 0
	currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 135))
	auxobj.transform = currentMatrix
)

fn assign_room_to_all_from_x = (
	for obj in $* do (
		if ( (obj.name != "suelo_003") and (obj.name != "__camController_1") and (obj.name != "suelo_2") and (obj.name != "rodapies") and (obj.name != "nivel") and (obj.name != "paredes_milestone_3") and (obj.name != "Techo_milestone_3") and (obj.name != "sofa2") )then (
			local trans = obj.transform
			if obj.name == "nivel" then (
					obj.room_name = "1"
			) else if trans.position.x < -45 then (
				obj.room_name = "2"
			) else (
				obj.room_name = "1"
			)			
		) else (
			obj.room_name = "-1"
		)
	)
)

fn createRoomSwitcherTrigger sala1 sala2 = (

	
	local nouObj = undefined
	local filePath = (objMax_dir+"TriggerBox.max")
	local objectes = getmaxfileobjectnames filePath
	clearSelection()
	
	if mergemaxfile filePath objectes #select then (
		objectes = $
		if (classof objectes == ObjectSet) then (objectes = ($ as array))
		else objectes = #(objectes)

		local o
		for o in objectes do (
			if (o.parent == undefined) then (
				nouObj = o
				exit
			)
		)
		
		nouObj.name = "Trigger_Cambio_Sala"
		nouObj.pos.x = -54.4765
		nouObj.pos.y = 5.47724
		nouObj.pos.z = -0.746866
		nouObj.length = 0.1
		nouObj.width = 3.0
		nouObj.height = 4.0
		
		setUserProp nouObj "typeObject" "Trigger"
		setUserProp nouObj "component" "room_switcher"
		setUserProp nouObj "front_room" "sala1"
		setUserProp nouObj "back_room" "sala2"
	) 
)

fn createRoomLimiterForPossession = (

	local nouObj = undefined
	local filePath = (objMax_dir+"TriggerBox.max")
	local objectes = getmaxfileobjectnames filePath
	clearSelection()
	
	if mergemaxfile filePath objectes #select then (
		objectes = $
		if (classof objectes == ObjectSet) then (objectes = ($ as array))
		else objectes = #(objectes)

		local o
		for o in objectes do (
			if (o.parent == undefined) then (
				nouObj = o
				exit
			)
		)
		
		nouObj.name = "Trigger_Escupe_Posessiones"
		nouObj.pos.x = -62.2198
		nouObj.pos.y = 5.47724
		nouObj.pos.z = -0.746866
		nouObj.length = 12
		nouObj.width = 0.1
		nouObj.height = 4.0
		
		setUserProp nouObj "typeObject" "Trigger"
		setUserProp nouObj "component" "limit"
	) 
)

fn loadAll = (
	if $Trigger_Escupe_Posessiones != undefined then (
		delete $Trigger_Escupe_Posessiones
	)
	if $Trigger_Cambio_Sala != undefined then (
		delete $Trigger_Cambio_Sala
	)
	addAlexLogicCamaras()
	assign_room_to_all_from_x()
	createRoomSwitcherTrigger "1" "2"
	createRoomLimiterForPossession()
	
)

loadAll()