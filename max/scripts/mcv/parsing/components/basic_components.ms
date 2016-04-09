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
	format "_parseTransform, obj = %\n" obj
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

fn centerIfNeeded = (
	if _descriptorObject.getType() == "Trigger" then (
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
-- RENDER  --
----------------------------------------------------------------------
fn _parse_render = (
	stMesh = getUserProp _parsingObject "staticMesh"
	if (stMesh != undefined) do (
		local xml = XML_PARSING.addChild "render_static_mesh"
		xml.addAttr "name" ("static_meshes/" + stMesh + staticMesh_ext)
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

