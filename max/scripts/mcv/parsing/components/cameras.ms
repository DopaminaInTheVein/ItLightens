-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn _parse_rail_camera = (
	
	-- Get Cameras and Targets
	local all_cameras = findAllClassList TargetCamera
	local all_targets = findAllClassList TargetObject
	--Check
	if all_cameras.count != all_targets.count then (
		throw ("Error: No match number of cameras and targets! (" + _parsingObject.name + ")")
	)
	
	--Component
	local xml = XML_PARSING.addChild "guided_camera"
	
	--Velocity
	local vel = getUserProp _parsingObject "velocity"
	if (vel == undefined) do (
		vel = "2.0"
	)
	xml.addAttr "velocity" (vel as string)

	--Default dirs ?
	local default_dirs = getUserProp _parsingObject "default_dirs"
	if (default_dirs == undefined) do (
		default_dirs = "0"
	)
	xml.addAttr "default_dirs" (default_dirs as string)
	
	--Points size
	xml.addAttr "points_size" all_cameras.count
		
	-- Parse points
	local mcv_tmx
	local wPoint
	local atrName
	local atrValue
	for i = 1 to all_cameras.count do (
		--Debug
		--messagebox(all_cameras[i].name)
		
		-- Atr Name
		atrName = ("wpt" + (i) as string)
		
		--Get Point
		wPoint = all_cameras[i].pos
		
		--Transform point in atrValue
		tMatrix = matrix3 1
		tMatrix.pos = wPoint
		mcv_tmx = matrixMax2MCV tMatrix
		atrValue = getTransformPosValue mcv_tmx
		
		--Parse Attribute wpt
		xml.addAttr  atrName atrValue
	)
	
	-- Parse targets
	for i = 1 to all_targets.count do (
		--Debug
		--messagebox(all_targets[i].name)
		
		-- Atr Name
		atrName = ("lpt" + (i) as string)
		
		--Get Point
		wPoint = all_targets[i].pos
		
		--Transform point in atrValue
		tMatrix = matrix3 1
		tMatrix.pos = wPoint
		mcv_tmx = matrixMax2MCV tMatrix
		atrValue = getTransformPosValue mcv_tmx
		
		--Parse Attribute wpt
		xml.addAttr  atrName atrValue
	)	
)
/* 	--_parsingObject
	--XML_PARSING
	if ( (numSegments _parsingObject 1) == ((_parsingObject.children.count)/2)) then (

		local xml = XML_PARSING.addChild "guided_camera"
		

		
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
 */
