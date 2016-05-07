-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn _parse_rail_camera = (
	--_parsingObject
	--XML_PARSING
	if ( (numSegments _parsingObject 1) == (_parsingObject.children.count)) then (

		local xml = XML_PARSING.addChild "guided_camera"
		
		local vel = getUserProp _parsingObject "velocity"
		if (vel == undefined) do (
			vel = "2.0"
		)
		xml.addAttr "velocity" (vel as string)
		xml.addAttr "points_size" ((numSegments _parsingObject 1) as string)
		for i = 1 to ((numSegments _parsingObject 1)+1) do (
			xml.addAttr ("wtp" + (i-1) as string) (((getKnotPoint _parsingObject 1 i).x as string)+" "+((getKnotPoint _parsingObject 1 i).z as string)+" "+((getKnotPoint _parsingObject 1 i).y as string))
		)
		local i2 = 0
		for c in _parsingObject.children do(
			if ((classof c) == FreeCamera or (classof c) == Targetcamera) then (
			
				xml.addAttr ("rot" + (i2 as string)) ((c.rotation.x as string) + " " + (c.rotation.z as string) + " " + (c.rotation.y as string) + " " + (c.rotation.w as string))
				
				i2 += 1
			)
		)
	) else throw ("Error: numSegments = " + 	(numSegments _parsingObject 1 as string) + " numCameras = " + (_parsingObject.children.count as string))
)