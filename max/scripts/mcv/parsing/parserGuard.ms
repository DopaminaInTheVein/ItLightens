-- Parse Object of type --
-- -- GUARD -- --
fn _parseGuard = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tag
	_parseTags xml #("AI","AI_guard")
	
	-- Transform
	_parseTransform xml
	
	--Life
	xmlLife = xml.addChild "life"
	xmlLife.addAttr "points" (getUserProp _parsingObject "life")
	
	--BT Guard
	btXml = xml.addChild "bt_guard"
	_parseKeyPoints btXml -- KptPoints
	_parseJurisdiction btXml --Jurisdiction

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	-- Character Controller (Physx)
	_parseCharController xml

	return xml
)