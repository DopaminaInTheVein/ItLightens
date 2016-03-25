-- Parse Object of type --
-- -- MOLE -- --
fn _parseMole = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tag
	_parseTags xml #("AI","AI_mole","AI_poss")
	
	-- Transform
	_parseTransform xml
	
	--Life
	xmlLife = xml.addChild "life"
	xmlLife.addAttr "points" (getUserProp _parsingObject "life")
	
	--BT Mole
	btXml = xml.addChild "bt_mole"
	_parseWayPoints btXml

	-- Parse Render del objeto
	_parseRenderStatic xml

	-- Mole Controller
	xmlController = xml.addChild "player_mole"
	
	-- Character Controller (Physx)
	_parseCharController xml

	return xml
)