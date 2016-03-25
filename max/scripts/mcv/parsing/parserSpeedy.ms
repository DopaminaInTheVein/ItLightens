-- Parse Object of type --
-- -- SPEEDY -- --
fn _parseSpeedy = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tag
	_parseTags xml #("AI","AI_speedy","AI_poss")
	
	-- Transform
	_parseTransform xml
	
	--Life
	xmlLife = xml.addChild "life"
	xmlLife.addAttr "points" (getUserProp _parsingObject "life")
	
	--BT Speedy
	btXml = xml.addChild "bt_speedy"
	_parseWayPoints btXml

	-- Parse Render del objeto
	_parseRenderStatic xml

	-- Speedy Controller
	xmlController = xml.addChild "player_speedy"
	
	-- Character Controller (Physx)
	_parseCharController xml

	return xml
)