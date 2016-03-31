-- Parse Object of type --
-- -- GENERATOR -- --
fn _parseGenerator = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"

	-- Name
	_parseName xml

	--Tag
	_parseTags xml "recover_point"
		
	-- Transform
	_parseTransform xml

	-- Parse Render del objeto
	_parseRenderStatic xml
	
	--Generator Component
	xmlGenerator = xml.addChild "generator"
	local rad = 2
	local jurisdiction = findHelper "Jurisdiction"
	if (jurisdiction != undefined) then rad = jurisdiction.radius
	xmlGenerator.addAttr "rad" rad
		
	return xml
)