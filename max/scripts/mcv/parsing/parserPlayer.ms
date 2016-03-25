-- Parse Object of type --
-- -- PLAYER -- --
fn _parsePlayer = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tag
	_parseTags xml TAG_PLAYER
	
	-- Transform
	_parseTransform xml
	
	--Life
	xmlLife = xml.addChild "life"
	xmlLife.addAttr "points" (getUserProp _parsingObject "life")
	
	-- Character Controller (Physx)
	_parseCharController xml

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	-- Player Controller
	xmlController = xml.addChild "player"
	return xml
)