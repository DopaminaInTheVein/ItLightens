-- Parse Object of type --
-- -- Crystal Wall -- --
fn _parseCrystalWall = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tags
	_parseTags xml #("crystal")
	
	-- Transform
	_parseTransform xml

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	-- Rigid Body (Physx)
	_parseRigidBody xml

	return xml
)