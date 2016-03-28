-- Parse Object of type --
-- -- TRIGGER -- --
fn _parseTrigger = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	local script = getUserProp _parsingObject "script"
	if (script as string) == "undefined" then (
		msg = "¡Cuidaaaao! ¡Això és rojaaaaa!\n\n"
		msg += ("El trigger [" + _parsingObject.name + "] no tiene script!\n")
		messageBox msg
		throw msg
	)
	
	-- Name
	_parseName xml
	
	--Tag
	_parseTags xml script
	
	-- Transform
	local posBefore = _parsingObject.pos
	_parsingObject.pos = _parsingObject.center
	_parseTransform xml
	_parsingObject.pos = posBefore
	
	-- Rigid Body (Physx)
	_parseRigidBodyTrigger xml
	
	-- Component
	xmlComponent = xml.addChild script
	return xml
)