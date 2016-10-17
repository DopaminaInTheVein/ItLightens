-- P A R S E    P H Y S X --
--***********************

--- Auxiliares ---
--------------------------------------------------------------------------------------------------------------------------------------------------------------
fn parseTypeShape xml collider isTrigger = (
	local typeShape = undefined
	local sizeShape = undefined
	local radiusShape = undefined
	
	if collider != undefined then (
		local theClass = classof collider
		if ( theClass == Pyramid)  then (
			 typeShape = "convex"
		 ) else if (classof collider == Box) then (
			local box_forced = ((getUserProp collider "box_forced") as string) == "yes"
			box_forced = box_forced or ((getUserProp collider "typeHelper") == undefined)
			if isTrigger or box_forced then(
				typeShape = "box"
				local ss = stringstream ""
				format "% % %" collider.Width collider.Height collider.Length to:ss
				sizeShape = (ss as string)
			) else ( -- Si es caja lo hacemos convexo por bug
				typeShape = "convex"
			)
		 ) else if (classof collider == Sphere) then (
			 typeShape = "sphere"
			 radiusShape = collider.radius
		 ) else (
			throw ("Falta implementar el collider de tipo" + ((classof collider) as string) + "! + (" + collider.name + ")\n")
		)
	) else (
		local typeCollider = (getUserProp _parsingObject "collider_type") as string
		if typeCollider == "convex" then (typeShape = "convex")
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
	-- Si es mesh, rigidbody (a menos que collision = no)
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
		XML_PARSING.addChild "local_aabb"
		return true
	)
	return false
)

fn parseRigidBody parent collider = (
	local xml = parent.addChild "rigidbody"
	
	-- Type Collision (static, dynamic, trigger)
	local typeCollision = "static"
	if collider != undefined then (
		local dynamic = getUserProp collider "dynamic"
		if dynamic == "yes" then typeCollision = "dynamic"
		parent.addChild "local_aabb"
	)
	xml.addAttr "type_collision" typeCollision
	
	--Type Shape (Convex, box, etc.)
	parseTypeShape xml collider false
	
	--Mass
	if collider != undefined then (
		local mass = (getUserProp collider "mass")
		if mass != undefined then (
			xml.addAttr "mass" mass
		)
	)
	
	--Set Kinematic
	if collider != undefined then (
		local kinematic = (getUserProp collider "kinematic")
		if kinematic != undefined then (
			xml.addAttr "kinematic" kinematic
		)
	)
)

fn parseRigidBodyTrigger parent = (
	local xml = parent.addChild "rigidbody"

	-- Type collision = trigger
	xml.addAttr "type_collision" "trigger"
	
	--Type Shape (Convex, box, etc.)
	parseTypeShape xml _parsingObject true
)

fn parseCharController parent charController = (
	local radius = "0.3"
	local height = "0.5"
	if charController != undefined then (
		local type = (getUserProp _parsingObject "typeObject") as string
		if type != "Scientist" then (
			radius = charController.radius as string
		)
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
	if _xml_sufix == "ui_scene" then (return false)
	
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
	local tags_list = #("trigger")
	local component = getUserProp _parsingObject "component"
	if (component != undefined) then ( append tags_list (component as string) )
	parseTags XML_PARSING tags_list
	--Enter
	local onEnter = getUserProp _parsingObject "onEnter"

	-- Exit
	local onExit = getUserProp _parsingObject "onLeave"
	
	-- Action Player
	local onAction = getUserProp _parsingObject "onAction"
	
	-- Action Scientist
	local onActionSci = getUserProp _parsingObject "onActionSci"
	
	-- Action Mole
	local onActionMole = getUserProp _parsingObject "onActionMole"

	-- Action Mole
	local onActionPila = getUserProp _parsingObject "onActionPila"

	local actived = getUserProp _parsingObject "actived"
	
	-- Script (component)
	if component != undefined then (
		if (onEnter != undefined) or (onExit != undefined) or (onAction != undefined) or (onActionSci != undefined) or (onActionMole != undefined)  or (onActionPila != undefined) then (
			throw "No puede ser un trigger de componente y logico a la vez!"
		)
		local compi = XML_PARSING.addChild component
		local fr = getUserProp _parsingObject "front_room"
		local br = getUserProp _parsingObject "back_room"
		if (component == "room_switcher" and fr != undefined and br != undefined) then(
			compi.addAttr "front_room" fr
			compi.addAttr "back_room" br
		)
		local taskid = getUserProp _parsingObject "task"
		if (component == "task_switcher" and taskid != undefined) then (
			compi.addAttr "task" taskid
		)
	) else (
		--Add trigger lua component
		local xml = XML_PARSING.addChild "trigger_lua"
		
		if actived != undefined then (
			xml.addAttr "actived" actived
		)
		
		if (onEnter == undefined) and (onExit == undefined) and (onAction == undefined) then (
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
		
		-- Logic trigger on action Scientist
		if onActionSci != undefined then (
			local nameFunc = "OnActionSci_" + _parsingObject.name
			luaAppend nameFunc onActionSci
			local action = getUserProp _parsingObject "actionSci"
			if action == undefined then (action = "activate")
			xml.addAttr "actionSci" action
		)
		
		-- Logic trigger on action Mole
		if onActionMole != undefined then (
			local nameFunc = "OnActionMole_" + _parsingObject.name
			luaAppend nameFunc onActionMole
			local action = getUserProp _parsingObject "actionMole"
			if action == undefined then (action = "activate")
			xml.addAttr "actionMole" action
		)

		-- Logic trigger on action Pila
		if onActionPil != undefined then (
			local nameFunc = "OnActionPila_" + _parsingObject.name
			luaAppend nameFunc onActionMole
			local action = getUserProp _parsingObject "actionPila"
			if action == undefined then (action = "activate")
			xml.addAttr "actionPila" action
		)
		
	)
)