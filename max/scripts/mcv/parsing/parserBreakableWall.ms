-- Parse Object of type --
-- -- Breakable Wall -- --
fn _parseBreakableWall = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tags
	_parseTags xml #("breakable_wall", "platform")
	
	-- Transform
	_parseTransform xml

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	-- Rigid Body (Physx)
	_parseRigidBody xml

	return xml
)