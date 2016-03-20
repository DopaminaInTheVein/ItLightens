-- Some general data
TAG_PLAYER = "target"
TAG_CAMERA = "player"

-- Current Object Parsing from userprops to XML
_parsingObject = undefined

--*******************************************************
-------------FUNCTIONS-------------------------------------
--*******************************************************
---------------------------------------------------------------------
-- OBJECT --
---------------------------------------------------------------------
fn _parseObject obj = (
	-- Guardamos objeto a parsear en una variable global
	_parsingObject = obj
	
	-- Según tipo de objeto llamamos a su correspondiente función de parseo
	local typeObject = getUserProp obj "typeObject"
	local parserFunction = "_parse" + typeObject
	xml = execute(parserFunction + "()")
	
	-- Devolvemos elemento XML del objeto parseado
	return xml
)
----------------------------------------------------------------------
-- NAME--
---------------------------------------------------------------------
fn _parseName parent = (
	local xml = parent.addChild "name"
	xml.addAttr "name" _parsingObject.name
)
----------------------------------------------------------------------
--TAGS--
----------------------------------------------------------------------
fn _parseTags parent tag= (
	format "Llamo parseTags con %\n" tag
	if (ClassOf tag) == (ClassOf #()) then (
		format "Es Array %\n" tag
		for t in tag do (
			format "Es ElemArray %\n" t
			_parseTags parent t
		)
	) else (
		format "Es string %\n" tag
		local xml = parent.addChild "tag"
		xml.addAttr "name" tag
	)
)
----------------------------------------------------------------------
-- TRANSFORM --
----------------------------------------------------------------------
fn _parseTransform parent = (
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
	local ss = stringstream ""
	format "% % %" t.x t.y t.z to:ss
	posVal = ss as string
	xmlTransform.addAttr "pos" posVal
	
	-- Quaternion (Orientation)
	ss = stringstream ""
	format "% % % %" -q.x -q.y -q.z q.w to:ss
	quatVal = ss as string
	xmlTransform.addAttr "quat" quatVal
	
	-- Scale
	scaleVal = s.x as string
	xmlTransform.addAttr "scale" scaleVal
)
----------------------------------------------------------------------
-- RENDER STATIC MESH --
----------------------------------------------------------------------
fn _parseRenderStatic parent = (
	stMesh = getUserProp _parsingObject "staticMesh"
	if (staticMesh != undefined) do (
		local xml = parent.addChild "render_static_mesh"
		xml.addAttr "name" ("static_meshes/" + stMesh)
	)
)

-- Aux Valores -- (poner entre comillas)
fn _val val = (
	return "\"" + (val as string) + "\""
)

--Test
fn xmlTest obj = (
	local xml = _parseObject obj
	xml.parse()
)