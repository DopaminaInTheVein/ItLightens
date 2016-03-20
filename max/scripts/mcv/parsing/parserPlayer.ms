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
	xmlLife.addAttr "points" "10"
	
	-- Player Controller
	xmlController = xml.addChild "player"
	
	-- Character Controller (Physx)
	xmlCC = xml.addChild "character_controller"
	xmlCC.addAttr "radius" "0.3"
	xmlCC.addAttr "height" "0.4"

	-- Parse Render del objeto
	_parseRenderStatic xml

	return xml
)