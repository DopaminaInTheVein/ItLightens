-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn _parse_rail_camera = (
	--_parsingObject
	--XML_PARSING
	if ( (numSegments _parsingObject 1) == ((_parsingObject.children.count)/2)) then (

		local xml = XML_PARSING.addChild "guided_camera"
		
		local vel = getUserProp _parsingObject "velocity"
		if (vel == undefined) do (
			vel = "2.0"
		)
		xml.addAttr "velocity" (vel as string)
		
		local default_dirs = getUserProp _parsingObject "default_dirs"
		if (default_dirs == undefined) do (
			default_dirs = "0"
		)
		xml.addAttr "default_dirs" (default_dirs as string)
		
		xml.addAttr "points_size" (((numSegments _parsingObject 1)+1) as string)
		
		local mcv_tmx
		local wPoint
		local atrName
		local atrValue
		for i = 1 to ((numSegments _parsingObject 1)+1) do (
			-- Atr Name
			atrName = ("wpt" + (i-1) as string)
		
			--Get Point
			wPoint = getKnotPoint _parsingObject 1 i
		
			--Transform point in atrValue
			tMatrix = matrix3 1
			tMatrix.pos = wPoint
			mcv_tmx = matrixMax2MCV tMatrix
			atrValue = getTransformPosValue mcv_tmx	
			--Parse Attribute wpt
			xml.addAttr  atrName atrValue
		)
		local i2 = 0
		for c in _parsingObject.children do(
			if ((classof c) == Targetobject ) then (
				-- Atr Name
				atrName = "rot" + (i2 as string)
				--format c.name
				-- Get target Position
				mcv_tmx = matrixMax2MCV c.transform
				local posi = mcv_tmx.position
				atrValue = getAsXmlValues #(posi.x, posi.z, posi.y)
				xml.addAttr atrName atrValue
				i2 += 1
			)
		)
	) else throw ("Error: numSegments = " + 	(numSegments _parsingObject 1 as string) + " numCameras = " + (_parsingObject.children.count as string))
)