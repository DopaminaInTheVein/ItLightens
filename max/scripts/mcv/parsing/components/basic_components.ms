-- -- BASIC COMPONENTS -- --

----------------------------------------------------------------------
-- NAME--
---------------------------------------------------------------------
fn _parse_name = (
	local theName = _parsingObject.name
	local parentObj = _parsingObject.parent
	if parentObj != undefined then (
		if (getUserProp parentObj "typeObject") != undefined then (
			theName = (parentObj.name + "_" + theName)
		)
	)
	local xml = XML_PARSING.addChild "name"
	xml.addAttr "name" theName
)

----------------------------------------------------------------------
-- TRANSFORM --
----------------------------------------------------------------------
fn parseTransform parent = (
	local obj = _parsingObject
	local mcv_tmx = matrixMax2MCV obj.transform -- matrix3
	local t = mcv_tmx.pos
	local q = mcv_tmx.rotationPart as quat
	local s = mcv_tmx.scalePart

	if s.x < 0 or s.y < 0 or s.z < 0 then throw ("Object " + obj.name + " has scale transform with negative values. Please apply a ResetTransform, fix the culling and retry")
	--if s.x != s.y or s.y != s.z then throw ("Object " + obj.name + " has non-uniform scales. Please fix it and try again " + (s as string) )
	-- We change the sign of the quaternion because max conversion from matrix to quaternion
	-- rotates in the opposite direction from DX libs

	-- Creamos elemento Transform
	xmlTransform = parent.addChild "transform"
	
	-- Position
	posVal = getTransformPosValue mcv_tmx
	xmlTransform.addAttr "pos" posVal
	
	-- Quaternion (Orientation)
	quatVal = getTransformRotValue mcv_tmx
	xmlTransform.addAttr "quat" quatVal
	
	-- Scale
	scaleVal = s.x as string
	xmlTransform.addAttr "scale" scaleVal
)

fn needToCenter = (
	local res = (_descriptorObject.getType() == "Trigger")
	res = res or (_descriptorObject.getType() == "AreaPol")
)

fn centerIfNeeded = (
	if needToCenter() then (
		_parsingObject.pos = _parsingObject.center
	)
)

fn _parse_transform = (
	local posBefore = _parsingObject.pos
	centerIfNeeded()
	parseTransform XML_PARSING
	_parsingObject.pos = posBefore
)

----------------------------------------------------------------------
-- CAMERA  --
----------------------------------------------------------------------
fn _parse_camera = (
	-- Camera
	xmlCamera = XML_PARSING.addChild "camera"
	xmlCamera.addAttr "znear" _parsingObject.nearrange
	xmlCamera.addAttr "zfar" _parsingObject.farrange
	xmlCamera.addAttr "fov" _parsingObject.fov
)

----------------------------------------------------------------------
-- TRACKER  --
----------------------------------------------------------------------
fn _parse_tracker = (
	local tracker = undefined
	
	-- Busqueda tracker
	for child in _parsingObject.children do (
		if (isSpline child) then (
			tracker = child
			exit -- Solo aceptamos un componente tracker
		)
	)
	
	-- Parseo tracker
	if tracker != undefined then (
		--Creamos copia del tracker
		tracker = copy tracker
		
		try (
			-- Convert to spline
			clearSelection()
			selectMore tracker
			macros.run "Modifier Stack" "Convert_to_Spline"
			clearSelection()
			
			--Obtenemos longitud
			local lengths = getSegLengths tracker 1 cum:true
			local longitud = lengths[lengths.count]
			
			-- Ojo resolucion super hard-code!
			local numDivs =ceil (longitud * 2) -- Cada 0.5m
			local deltaInterval = ( 1 / numDivs)
			
			local xmlTracker = XML_PARSING.addChild "tracker"
			xmlTracker.addAttr "long" longitud
			nspl = numSplines tracker
		
			-- 
			local curPoint = 0
			local atrName = undefined
			local atrValue	= undefined
			local tPoint = undefined
			local tMatrix = undefined
			local mcv_tmx = undefined
			-- 
			local x = 0 -- valor entre 0 y 1 donde evaluar curva
			while x < 1 do (
				
				-- Attribute pos
				tPoint = interpCurve3D tracker 1 x
				tMatrix = matrix3 1
				tMatrix.pos = tPoint
				mcv_tmx = matrixMax2MCV tMatrix
				atrName = "point" + (curPoint as string)
				atrValue = getTransformPosValue mcv_tmx
				xmlTracker.addAttr atrName atrValue
				
				-- Attribute tangent
				tPoint = tangentCurve3D tracker 1 x
				tMatrix = matrix3 1
				tMatrix.pos = tPoint
				mcv_tmx = matrixMax2MCV tMatrix
				atrName = "tangent" + (curPoint as string)
				atrValue = getTransformPosValue mcv_tmx
				--xmlTracker.addAttr atrName atrValue
				
				-- Next
				x += deltaInterval
				curPoint += 1
			)
			xmlTracker.addAttr "size" curPoint
		) catch (
			delete tracker
			throw()
		)
		delete tracker
	)
)
----------------------------------------------------------------------
-- RENDER  --
----------------------------------------------------------------------
fn _parse_render = (
	-- Tiene skeleton?
	skel = getUserProp _parsingObject "skeleton"
	if skel != undefined then (
		local xmlSkel = XML_PARSING.addChild "skeleton"
		xmlSkel.addAttr "model" ("skeletons/" + skel + "/" + skel + skel_ext)
		
		local xmlStMesh = XML_PARSING.addChild "render_static_mesh"
		xmlStMesh.addAttr "name" ("skeletons/" + skel + "/" + skel + staticMesh_ext)
	) else (
		--Tiene staticMesh?
		stMesh = getUserProp _parsingObject "staticMesh"
		if (stMesh != undefined) do (
			local xmlStMesh = XML_PARSING.addChild "render_static_mesh"
			xmlStMesh.addAttr "name" ("static_meshes/" + stMesh + staticMesh_ext)
		)
	)
)

----------------------------------------------------------------------
-- POLARIZED --
----------------------------------------------------------------------
fn _parse_polarized = (
	local polarized = findHelper "Polarized"
	if (polarized != undefined) then (
		local xmlPol = XML_PARSING.addChild "polarized"
		xmlPol.addAttr "pol" (getUserProp polarized "pol")
	)
)

fn parsePolarizedArea obj = (
	--Polarized component
	local xmlPol = XML_PARSING.addChild "polarized"
	
	--Polarity attribute (by color)
	local pol
	local r = obj.mat.diffuse.r
	local g = obj.mat.diffuse.g
	local b = obj.mat.diffuse.b
	if r > ((g + b) * 2) then ( pol = "plus")
	else if b > ((r + g) * 2) then ( pol = "minus")
	else if b*4 < (r + g) then ( pol = "neutral")
	else throw ("Cant calc polarity of " + obj.name + " from color material!")
	xmlPol.addAttr "pol" pol

	-- Box
	local ss = stringstream ""
	format "% % %" obj.Width obj.Height obj.Length to:ss
	local size = (ss as string)
	xmlPol.addAttr "size" size
)

fn _parse_polarizedArea = (
	--Polarized Component
	local polArea = findHelper "AreaPol"
	if polArea != undefined then (
		parsePolarizedArea polArea
	)	
)

----------------------------------------------------------------------
-- DRONE --
----------------------------------------------------------------------
fn _parse_drone = (

	-- Drone component
	local xmlDrone = XML_PARSING.addChild "drone"
	
	--Patrol
	parseWayPoints xmlDrone
	
	--Textures self ilumination
	local textureFront = getUserProp _parsingObject "textureFront"
	xmlDrone.addAttr "textureFront" textureFront
	
	local textureBack = getUserProp _parsingObject "textureBack"
	xmlDrone.addAttr "textureBack" textureBack
)

----------------------------------------------------------------------
-- BOX --
----------------------------------------------------------------------
fn _parse_box = (
	local xmlBox = XML_PARSING.addChild "box"
	local fixedValue = (getUserProp _parsingObject "fixed") as string
	if fixedValue != "undefined" then (
		if fixedValue == "yes" then (fixedValue = "true")
		else if fixedValue == "no" then (fixedValue = "false")
		else throw ("Error [" + _parsingObject + "]: wrong 'fixed' value")
		xmlBox.addAttr "fixed" fixedValue
	)
)