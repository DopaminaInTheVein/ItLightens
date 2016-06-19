-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn _parse_beacon = (
	if ( ((classof _parsingObject) == Editable_mesh or (classof _parsingObject) == Editable_poly )) then (
		local lightdirshadow = XML_PARSING.addChild "light_dir_shadows"
		lightdirshadow.addAttr "color" "1 0 0 1"
		lightdirshadow.addAttr "znear" "0.1"
		lightdirshadow.addAttr "zfar" "10"
		lightdirshadow.addAttr "fov" "45"
		lightdirshadow.addAttr " resolution" "512"

		local beicon = XML_PARSING.addChild "beacon"
		
		local ran = getUserProp _parsingObject "range"
		if (ran != undefined) do (
			beicon.addAttr "range" (ran as string)
		)
		local wi = getUserProp _parsingObject "width"
		if (wi != undefined) do (
			beicon.addAttr "width" (wi as string)
		)
		local spe = getUserProp _parsingObject "rot_speed_sonar"
		if (spe != undefined) do (
			beicon.addAttr "rot_speed_sonar" (spe as string)
		)
		local rot = getUserProp _parsingObject "rotating_left"
		if (rot != undefined) do (
			beicon.addAttr "rotating_left" (rot as string)
		)

		local xml = XML_PARSING.addChild "render_static_mesh"
		xml.addAttr "name" "static_meshes/test/baliza.static_mesh"		

		) else throw ("Error: not an editable mesh or poly for making beacons ")
)

fn _parse_detection_cam = (
	if ( ((classof _parsingObject) == Editable_mesh or (classof _parsingObject) == Editable_poly )) then (
		local lightdirshadow = XML_PARSING.addChild "light_dir_shadows"
		lightdirshadow.addAttr "color" "1 0 0 1"
		lightdirshadow.addAttr "znear" "0.1"
		lightdirshadow.addAttr "zfar" "10"
		lightdirshadow.addAttr "fov" "45"
		lightdirshadow.addAttr " resolution" "512"

		local camdetect = XML_PARSING.addChild "ai_cam"
		
		local ran = getUserProp _parsingObject "range"
		if (ran != undefined) do (
			camdetect.addAttr "range" (ran as string)
		)
		local wi = getUserProp _parsingObject "width"
		if (wi != undefined) do (
			camdetect.addAttr "width" (wi as string)
		)
		local spe = getUserProp _parsingObject "rot_speed_sonar"
		if (spe != undefined) do (
			camdetect.addAttr "rot_speed_sonar" (spe as string)
		)
		local rot = getUserProp _parsingObject "rotating_left"
		if (rot != undefined) do (
			camdetect.addAttr "rotating_left" (rot as string)
		)
		local mxr = getUserProp _parsingObject "max_rotation"
		if (mxr != undefined) do (
			camdetect.addAttr "max_rotation" (mxr as string)
		)

		local xml = XML_PARSING.addChild "render_static_mesh"
		xml.addAttr "name" "static_meshes/test/baliza.static_mesh"		

		) else throw ("Error: not an editable mesh or poly for making beacons ")
)