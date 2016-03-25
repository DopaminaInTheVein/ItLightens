-- Parse Object of type --
-- -- Beacon -- --
fn _parseBeacon = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tags
	_parseTags xml #("beacon")
	
	-- Transform
	_parseTransform xml

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	-- Rigid Body (Physx)
	_parseRigidBody xml

	-- Beacon Component
	xmlController = xml.addChild "beacon"
	
	return xml
)