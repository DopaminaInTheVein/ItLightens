-- Parse Object of type --
-- -- BOX -- --
fn _parseBox = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tag
	_parseTags xml #("box","platform")
	
	-- Transform
	_parseTransform xml

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	-- Rigid Body (Physx)
	_parseRigidBody xml
	
	-- Box Component
	--xmlController = xml.addChild "box"
	return xml
)