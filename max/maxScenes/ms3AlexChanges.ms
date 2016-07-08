function addAlexLogicCamaras = (
	-- CAMARAS DETECTORAS
	local auxobj =  $camera_1
	setUserProp auxobj "typeObject" "detection_cam"
	-- no! caca! setUserProp auxobj "staticMesh" "ms3/logic/camera"
	setUserProp auxobj "range" 4
	setUserProp auxobj "width" 2
	setUserProp auxobj "rot_speed_sonar" 0.9
	setUserProp auxobj "rotating_left" false
	setUserProp auxobj "max_rotation" 162
	auxobj.rotation.x_rotation = 0
	auxobj.rotation.y_rotation = 0
	auxobj.rotation.z_rotation = 0
	local currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 -9))
	auxobj.transform = currentMatrix
		
	auxobj =  $camera_2
	setUserProp auxobj "typeObject" "detection_cam"
	-- no! caca! setUserProp auxobj "staticMesh" "ms3/logic/camera"
	setUserProp auxobj "range" 4
	setUserProp auxobj "width" 2
	setUserProp auxobj "rot_speed_sonar" 0.5
	setUserProp auxobj "rotating_left" false
	setUserProp auxobj "max_rotation" 90
	auxobj.rotation.x_rotation = 0
	auxobj.rotation.y_rotation = 0
	auxobj.rotation.z_rotation = 0
	currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 225))
	auxobj.transform = currentMatrix

	auxobj =  $camera_3
	setUserProp auxobj "typeObject" "detection_cam"
	-- no! caca! setUserProp auxobj "staticMesh" "ms3/logic/camera"
	setUserProp auxobj "range" 4
	setUserProp auxobj "width" 2
	setUserProp auxobj "rot_speed_sonar" 0.5
	setUserProp auxobj "rotating_left" true
	setUserProp auxobj "max_rotation" 90
	auxobj.rotation.x_rotation = 0
	auxobj.rotation.y_rotation = 0
	auxobj.rotation.z_rotation = 0
	currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 315))
	auxobj.transform = currentMatrix
)

fn assign_room_to_all_from_x sala1 sala2= (
	for obj in $* do (
		if (not (hasProperty obj "room_name") and (obj.name != "suelo_003") and (obj.name != "suelo_2") and (obj.name != "rodapies") and (obj.name != "paredes_milestone_3") and (obj.name != "Techo_milestone_3") )then (
			add_attribute = custAttributes.add obj v_room_data baseobject:True
			local trans = obj.transform
			if trans.position.x < -45 then (
				obj.room_name = sala2
			) else (
				obj.room_name = sala1
			)
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
	if $Trigger_Escupe_Posessiones != undefined then (
		delete $Trigger_Escupe_Posessiones
	)
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

--addAlexLogicCamaras()
--assign_room_to_all_from_x "sala1" "sala2"
--createRoomSwitcherTrigger "sala1" "sala2"
--createRoomLimiterForPossession()
