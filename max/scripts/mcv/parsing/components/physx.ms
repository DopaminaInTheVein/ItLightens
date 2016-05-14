-- P A R S E    P H Y S X --
--***********************

--- Auxiliares ---
--------------------------------------------------------------------------------------------------------------------------------------------------------------
fn parseTypeShape xml collider = (
	local typeShape = undefined
	local sizeShape = undefined
	local radiusShape = undefined
	
	if collider != undefined then (
		 if ( classof collider == Pyramid )  then (
			 typeShape = "convex"
		 ) else if (classof collider == Box) then (
			typeShape = "box"
			local ss = stringstream ""
			format "% % %" collider.Width collider.Height collider.Length to:ss
			sizeShape = (ss as string)
		 ) else if (classof collider == Sphere) then (
			 typeShape = "sphere"
			 radiusShape = collider.radius
		 ) else (
			throw ("Falta implementar el collider de tipo" + ((classof collider) as string) + "! + (" + collider.name + ")\n")
		)
	)
	
	if typeShape != undefined then (
		xml.addAttr "type_shape" typeShape
		if sizeShape != undefined then (
			xml.addAttr "size" sizeShape
		)
		if radiusShape != undefined then (
			xml.addAttr "radius" radiusShape
		)
	)
)
fn parseRigidBodyDefault parent = (
	-- Si es mesh, rigidbody
	local clase = classof _parsingObject
	if clase == Editable_Mesh or clase == Editable_Poly then (
		local colision = getUserProp _parsingObject "collision"
		if colision as string != "no" then (
			local xml = parent.addChild "rigidbody"
			xml.addAttr "type_collision" "static"
		)
	)
)

fn parseRigidBodySpecial = (
	if ((getUserProp _parsingObject "typeObject") as string) == "Drone" then (
		local xml = XML_PARSING.addChild "rigidbody"
		xml.addAttr "type_collision" "dynamic"
		xml.addAttr "type_shape" "drone"
		return true
	)
	return false
)

fn parseRigidBody parent collider = (
	local xml = XML_PARSING.addChild "rigidbody"
	
	-- Type Collision (static, dynamic, trigger)
	local typeCollision = "static"
	if collider != undefined then (
		local dynamic = getUserProp collider "dynamic"
		if dynamic == "yes" then typeCollision = "dynamic"
	)
	xml.addAttr "type_collision" typeCollision
	
	--Type Shape (Convex, box, etc.)
	parseTypeShape xml collider
	
	--Mass
	if collider != undefined then (
		local mass = (getUserProp collider "mass")
		if mass != undefined then (
			xml.addAttr "mass" mass
		)
	)
)

fn parseRigidBodyTrigger parent = (
	local xml = parent.addChild "rigidbody"

	-- Type collision = trigger
	xml.addAttr "type_collision" "trigger"
	
	--Type Shape (Convex, box, etc.)
	parseTypeShape xml _parsingObject
)

fn parseCharController parent charController = (
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

--- Components ---
----------------------------------------------------------------------------------------------------------------------------------------------------------------

-- PHYSX --
-------------
fn _parse_physx = (
	-- Character Controller?
	local charController = findHelper "CharController"
	if charController != undefined then (
		parseCharController XML_PARSING charController
	) else (
		--Collider?
		local collider = findHelper "Collider"
		if collider != undefined then (
			parseRigidBody XML_PARSING collider
		) else if not (parseRigidBodySpecial() ) then (
			parseRigidBodyDefault XML_PARSING
		)
	)
)

-- Trigger --
--------------
fn _parse_trigger = (
	-- Rigid Body (Physx)
	parseRigidBodyTrigger XML_PARSING

	-- Component
	local component = getUserProp _parsingObject "component"

	--Enter
	local onEnter = getUserProp _parsingObject "onEnter"

	-- Exit
	local onExit = getUserProp _parsingObject "onLeave"
	
	-- Action
	local onAction = getUserProp _parsingObject "onAction"
	
	-- Script (component)
	if component != undefined then (
		if (onEnter != undefined) or (onExit != undefined) or (onAction != undefined) then (
			throw "No puede ser un trigger de componente y logico a la vez!"
		)
		XML_PARSING.addChild component
	) else (
		--Add trigger lua component
		local xml = XML_PARSING.addChild "trigger_lua"
			
		if (onEnter == undefined) and (onExit == undefined) and (onAction != undefined) then (
			throw "El trigger debe ser lógico o de componente!"
		)
		-- Logic trigger on enter
		if onEnter != undefined then (
			local nameFunc = "OnEnter_" + _parsingObject.name
			luaAppend nameFunc onEnter
		)
		
		-- Logic trigger on exit
		if onExit != undefined then (
			local nameFunc = "OnLeave_" + _parsingObject.name
			luaAppend nameFunc onExit
		)
		
		-- Logic trigger on action
		if onAction != undefined then (
			local nameFunc = "OnAction_" + _parsingObject.name
			luaAppend nameFunc onAction
			local action = getUserProp _parsingObject "action"
			if action == undefined then (action = "activate")
			xml.addAttr "action" action
		)
	)
)