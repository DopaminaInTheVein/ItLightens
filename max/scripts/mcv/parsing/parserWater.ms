-- Parse Object of type --
-- -- WATER -- --
fn _parseWater = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"

	-- Name
	_parseName xml

	--Tag
	_parseTags xml "water"
		
	-- Transform
	_parseTransform xml

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	--Generator Component
	xmlWater = xml.addChild "water"
	
	return xml
)