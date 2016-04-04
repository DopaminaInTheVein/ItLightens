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
	print ("Trigger: " + _parsingObject.name)
	local xml = parent.addChild "rigidbody"

	-- Type collision = trigger
	xml.addAttr "type_collision" "trigger"
	
	--Type Shape (Convex, box, etc.)
	parseTypeShape xml _parsingObject
	print ("Trigger End: " + _parsingObject.name)
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
	print "Physx: " + _parsingObject.name
	-- Character Controller?
	local charController = findHelper "CharController"
	if charController != undefined then (
		parseCharController XML_PARSING charController
	) else (
		--Collider?
		local collider = findHelper "Collider"
		if collider != undefined then (
			parseRigidBody XML_PARSING collider
		) else (
			parseRigidBodyDefault XML_PARSING
		)
	)
)

-- Trigger --
--------------
fn _parse_trigger = (
	-- Leemos script
	local script = getUserProp _parsingObject "script"
	if (script as string) == "undefined" then (
		msg = "¡Cuidaaaao! ¡Això és rojaaaaa!\n\n"
		msg += ("El trigger [" + _parsingObject.name + "] no tiene script!\n")
		messageBox msg
		throw msg
	)
	
	-- Rigid Body (Physx)
	parseRigidBodyTrigger XML_PARSING
	
	-- Component
	XML_PARSING.addChild script
)