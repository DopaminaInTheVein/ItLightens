function addAlexLogicCamaras = (
	-- CAMARAS DETECTORAS
	local auxobj =  $camara_centro
	setUserProp auxobj "typeObject" "detection_cam"
	setUserProp auxobj "staticMesh" "ms3/camara_derecha"
	setUserProp auxobj "range" 4
	setUserProp auxobj "width" 2
	setUserProp auxobj "rot_speed_sonar" 0.5
	setUserProp auxobj "rotating_left" false
	setUserProp auxobj "max_rotation" 130
	local currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 155))
	auxobj.transform = currentMatrix
		
	auxobj =  $camara_derecha
	setUserProp auxobj "typeObject" "detection_cam"
	setUserProp auxobj "staticMesh" "ms3/camara_derecha"
	setUserProp auxobj "range" 4
	setUserProp auxobj "width" 2
	setUserProp auxobj "rot_speed_sonar" 0.5
	setUserProp auxobj "rotating_left" false
	setUserProp auxobj "max_rotation" 90
	currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 90))
	auxobj.transform = currentMatrix

	auxobj =  $camara_izquierdaq
	setUserProp auxobj "typeObject" "detection_cam"
	setUserProp auxobj "staticMesh" "ms3/camara_derecha"
	setUserProp auxobj "range" 4
	setUserProp auxobj "width" 2
	setUserProp auxobj "rot_speed_sonar" 0.5
	setUserProp auxobj "rotating_left" true
	setUserProp auxobj "max_rotation" 90
	currentMatrix = auxobj.transform
	preRotate currentMatrix (eulertoquat (eulerAngles 0 0 90))
	auxobj.transform = currentMatrix
)
addAlexLogicCamaras()