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
fn _parseObject parent = (
	
	-- Según tipo de objeto llamamos a su correspondiente función de parseo
	local typeObject = (getUserProp _parsingObject "typeObject")
	local xml = undefined
	if typeObject == undefined then (
		xml = _parseDefault()
	) else (
		local parserFunction = "_parse" + typeObject
		xml = execute(parserFunction + "()")
	)
	
	-- Añadimos elemento XML del objeto parseado
	if xml != undefined then (
		_parseCommonComponents xml
		parent.addElement xml
	)
)

----------------------------------------------------------------------
-- LEVEL--
---------------------------------------------------------------------
fn _parseLevel = (
	local xml = XMLNode()
	xml.nombre = "entities"
	for obj in $* do (
		_parsingObject = obj
		_parseObject xml
	)
	_parsingObject = undefined
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
	posVal = getTransformPosValue mcv_tmx
	xmlTransform.addAttr "pos" posVal
	
	-- Quaternion (Orientation)
	quatVal = getTransformRotValue mcv_tmx
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
	if (stMesh != undefined) do (
		local xml = parent.addChild "render_static_mesh"
		xml.addAttr "name" ("static_meshes/" + stMesh)
	)
)

----------------------------------------------------------------------
-- RIGID BODY --
----------------------------------------------------------------------
fn _parseRigidBody parent = (
	local xml = parent.addChild "rigidbody"
	local collider = findHelper "Collider"
	
	-- Type Collision (static, dynamic)
	local typeCollision = "static"
	if collider != undefined then (
		local dynamic = getUserProp collider "dynamic"
		
		if dynamic == "yes" then typeCollision = "dynamic"
	)
	xml.addAttr "type_collision" typeCollision
	
	--Type Shape (Convex, box, etc.)
	local typeShape = undefined
	if collider != undefined then (
		 if ( classof collider == Pyramid )  then (
			 typeShape = "convex"
		 ) else if (classof collider == Box) then (
			typeShape = "box"
			 local ss = stringstream ""
			 format "% % %" collider.Length collider.Height collider.Width to:ss
			 xml.addAttr "size" (ss as string)
		 ) else (
			throw ("Falta implementar el collider de tipo" + ((classof collider) as string) + "!\n")
		)
	)
	if typeShape != undefined then (
		xml.addAttr "type_shape" typeShape
	)
	
	--Mass
	if collider != undefined then (
		local mass = (getUserProp collider "mass")
		if mass != undefined then (
			xml.addAttr "mass" mass
		)
	)
)

----------------------------------------------------------------------
-- CHAR CONTROLLER --
----------------------------------------------------------------------
fn _parseCharController parent = (
	local charController = (findHelper "CharController")
	local radius = "0.3" --> Prefab!?
	local height = "0.5" --> Prefab!?
	if charController != undefined then (
		radius = charController.radius as string
		height = charController.height as string
	)
	local xml = parent.addChild "character_controller"
	xml.addAttr "radius" radius
	xml.addAttr "height" height
)

----------------------------------------------------------------------
-- WAY POINTS --
----------------------------------------------------------------------
fn _parseWayPoints parent = (
	local patrols = (findAllHelpers "Patrol_Go")
	if patrols.count > 0 then (
		parent.addAttr "wpts_size" patrols.count
		local curWpt = 0
		for p in patrols do (
			local mcv_tmx = matrixMax2MCV p.transform -- matrix3
			atrName =  "wpt" + (curWpt as string) + "_pos"
			atrValue = getTransformPosValue mcv_tmx
			parent.addAttr atrName atrValue
			curWpt += 1
		)
	)
)

----------------------------------------------------------------------
-- KEY POINTS (Look & Go) --
----------------------------------------------------------------------
fn _parseKeyPoints parent = (
	local patrols = (findAllHelpers #("Patrol_Go", "Patrol_Look"))
	if patrols.count > 0 then (
		parent.addAttr "kpts_size" patrols.count
		local curKpt = 0
		for p in patrols do (
			
			--Type
			local atrName = "kpt" + (curKpt as string) + "_type"
			local atrValue = undefined
			local curHelper = getUserProp p "typeHelper"
			if (curHelper == "Patrol_Go") then (
				atrValue = "seek"
			) else if (curHelper == "Patrol_Look") then (
				atrValue = "look"
			) else (
				throw "Tipo de Patrol Helper desconocido!"
			)
			parent.addAttr atrName atrValue
			
			-- Position
			local mcv_tmx = matrixMax2MCV p.transform -- matrix3
			atrName =  "kpt" + (curKpt as string) + "_pos"
			atrValue = getTransformPosValue mcv_tmx
			parent.addAttr atrName atrValue
			
			-- Wait
			atrName = "kpt" + (curKpt as string) + "_wait"
			atrValue = (getUserProp p "waitTime")
			if (atrValue == undefined) then atrValue = 0
			parent.addAttr atrName atrValue
			
			-- Next
			curKpt += 1
		)
	)
)

----------------------------------------------------------------------
-- JURISDICTION --
----------------------------------------------------------------------
fn _parseJurisdiction parent = (
	local jurisdiction = (findHelper "Jurisdiction")
	if jurisdiction != undefined then (
		--Jurisdiction Pos
		local mcv_tmx = (matrixMax2MCV jurisdiction.transform)
		parent.addAttr "jursidiction" (getTransformPosValue mcv_tmx)
		
		--Jurisdiction Radius
		parent.addAttr "jurRadius" jurisdiction.radius
	)
)


-------------------------------------------------------------------------------------
-- Auxiliares -----------------------------------------------------------------------
-------------------------------------------------------------------------------------

---------------------------------------------------------
-- Aux Valores -- (poner entre comillas)
--------------------------------------------------------
fn _val val = (
	return "\"" + (val as string) + "\""
)

---------------------------------------------------------
--Test
---------------------------------------------------------
fn xmlTest obj = (
	local xml = XMLNode()
	xml.nombre = "entities"
	_parsingObject = obj
	_parseObject xml
	_parsingObject = undefined
	xml.parse()
)