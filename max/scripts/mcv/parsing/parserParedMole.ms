-- Parse Object of type --
-- -- ParedMole -- --
fn _parseParedMole = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tags
	_parseTags xml #("breakable_wall", "platform")
	
	xmlTag = xml.addChild "tag"
	xmlTag.addAttr "name" "platform"
	
	-- Transform
	_parseTransform xml
	
	-- Rigid Body (Physx)
	xmlRigidBody = xml.addChild "rigidbody"
	xmlRigidBody. addAttr "type_collision" "static"

	-- Parse Render del objeto
	_parseRenderStatic xml

	return xml
)