-- -- BASIC COMPONENTS -- --

fn IsCollider obj = (
	if (getUserProp obj "collision") == "yes" then (
		if  (classof obj) == Box then (return true)
	)
	return false
)

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
	res = res or (IsCollider _parsingObject)
	--messagebox ("Need to center: " +  (res as string))
	return res
)

fn centerIfNeeded = (
	if needToCenter() then (
		_parsingObject.pos = _parsingObject.center
	)
)

fn parseTmxAnimator = (
	local yaw_speed = getUserProp _parsingObject "yaw_speed"
	local roll_speed = getUserProp _parsingObject "roll_speed"
	local pitch_speed = getUserProp _parsingObject "pitch_speed"
	local animated = false
	if yaw_speed !=undefined then (animated = true)
	if roll_speed !=undefined then (animated = true)
	if pitch_speed !=undefined then (animated = true)
	if animated then (
		local xml = XML_PARSING.addChild "tmx_animator"
		if yaw_speed !=undefined then (animated = true)
		if roll_speed !=undefined then (animated = true)
		if pitch_speed !=undefined then (animated = true)
		parsePropAttribute xml "yaw_speed" "yaw_speed"
		parsePropAttribute xml "roll_speed" "roll_speed"
		parsePropAttribute xml "pitch_speed" "pitch_speed"
	)
)

fn _parse_transform = (
	local posBefore = _parsingObject.pos
	centerIfNeeded()
	parseTransform XML_PARSING
	_parsingObject.pos = posBefore
	parseTmXAnimator()
)

----------------------------------------------------------------------
-- CAMERA  --
----------------------------------------------------------------------
fn parseCameraCommon xml = (
	xml.addAttr "znear" _parsingObject.nearrange
	xml.addAttr "zfar" _parsingObject.farrange
	xml.addAttr "fov" _parsingObject.fov
)

fn _parse_camera = (
	-- Camera
	xmlCamera = XML_PARSING.addChild "camera"
	parseCameraCommon xmlCamera
)

fn _parse_camera_main = (
	-- Camera Main
	xmlCamera = XML_PARSING.addChild "camera_main"
	parseCameraCommon xmlCamera
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
fn parse_render_common xml = (
	xml.addAttr "static" (isStatic _parsingObject)
	parsePropAttribute xml "cast_shadows" "cast_shadows"
)

fn _parse_render = (
	-- Tiene skeleton?
	skel = getUserProp _parsingObject "skeleton"
	local xmlStMesh = undefined
	if skel != undefined then (
		local xmlSkel = XML_PARSING.addChild "skeleton"
		xmlSkel.addAttr "model" ("skeletons/" + skel + "/" + skel + skel_ext)
		local anim = getUserProp _parsingObject "anim"
		if anim !=undefined then (
			xmlSkel.addAttr "anim" anim
		)
		
		xmlStMesh = XML_PARSING.addChild "render_static_mesh"
		xmlStMesh.addAttr "name" ("skeletons/" + skel + "/" + skel + staticMesh_ext)
		XML_PARSING.addChild "skeleton_ik"
		XML_PARSING.addChild "abs_aabb"
		XML_PARSING.addChild "local_aabb"		
	) else (
		--Tiene staticMesh?
		stMesh = getUserProp _parsingObject "staticMesh"
		if (stMesh != undefined) do (
			xmlStMesh = XML_PARSING.addChild "render_static_mesh"
			xmlStMesh.addAttr "name" ("static_meshes/" + stMesh + staticMesh_ext)
			XML_PARSING.addChild "abs_aabb"
			local movable = getUserProp _parsingObject "movable"
			if (movable as string) == "yes" then (
				XML_PARSING.addChild "local_aabb"
			)
			
		)
	)
	if xmlStMesh != undefined then (
		parse_render_common xmlStMesh
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
		local offset = polarized.pos - _parsingObject.pos
		local offsetValues = #(offset.x, offset.z,-offset.y)
		xmlPol.addAttr "offset" (getAsXmlValues offsetValues)		
	) else (
		local polarized = getUserProp _parsingObject "pol"
		if (polarized != undefined) then (
			local xmlPol = XML_PARSING.addChild "polarized"
			xmlPol.addAttr "pol" polarized
		)		
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

----------------------------------------------------------------------
-- HIERARCHY --
----------------------------------------------------------------------
fn _parse_hierarchy = (
	if ((getUserProp  _parsingObject "hierarchy") as string) == "yes" then (
		local parent = _parsingObject.parent
		if _parsingObject.parent == undefined then ( 
			throw ("Object [" + _parsingObject.name +"] has not parent!")
		)
		local parent_id = getUserProp parent "id"
		if parent_id == undefined then ( 
			throw ("Object [" + _parsingObject.name +"] has not ID!")
		)
		local xml = XML_PARSING.addChild "hierarchy"
		xml.addAttr "parent" parent_id
	)
)