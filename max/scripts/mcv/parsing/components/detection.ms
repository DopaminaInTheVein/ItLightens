-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn _parse_detection_cam = (
	if ( ((classof _parsingObject) == Editable_mesh or (classof _parsingObject) == Editable_poly )) then (
		local lightdirshadow = XML_PARSING.addChild "light_dir_shadows"
		lightdirshadow.addAttr "color" "1 0 0 1"
		lightdirshadow.addAttr "znear" "1.1"
		lightdirshadow.addAttr "zfar" "10"
		lightdirshadow.addAttr "fov" "45"
		lightdirshadow.addAttr " resolution" "512"

		local camtags = XML_PARSING.addChild "tags"
		camtags.addAttr "tags" "ai_cam"
		
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

		local stMesh = getUserProp _parsingObject "staticMesh"
		if (stMesh != undefined) do (
			local xmlStMesh = XML_PARSING.addChild "render_static_mesh"
			xmlStMesh.addAttr "name" ("static_meshes/" + stMesh + staticMesh_ext)
			XML_PARSING.addChild "abs_aabb"
			local movable = getUserProp _parsingObject "movable"
			if (movable as string) == "yes" then (
				XML_PARSING.addChild "local_aabb"
			)
		)

		) else throw ("Error: not an editable mesh or poly for making beacons ")
)