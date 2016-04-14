-- Include XML Parser
fileIn (mcvParsingDir + "xmlParser.ms")
	
-- Include Auxiliars
getParsing = getFiles (mcvParsingDir + "auxiliars\\" + "*.ms")
for f in getParsing do
(
	fileIn f
)

-- Include Parsing Components
getParsing = getFiles (mcvParsingDir + "components\\" + "*.ms")
for f in getParsing do
(
	fileIn f
)

-- Current Object Parsing from userprops to XML
_parsingObject = undefined
_descriptorObject = undefined
XML_PARSING = undefined
_lua_code = ""


--*******************************************************
-------------FUNCTIONS-------------------------------------
--*******************************************************
---------------------------------------------------------------------
-- OBJECT --
---------------------------------------------------------------------
fn _parseObject parent = (
	
	print ("Object: " + (_parsingObject.name as string))
	-- Obtenemos descriptor del objeto
	_descriptorObject = OManager.getDescriptor _parsingObject
	
	if parseable() then (
		-- Añadimos Entity
		XML_PARSING = parent.addChild "entity"
		
		--Parseamos Tags
		tags = _descriptorObject.getTags()
		parseTags XML_PARSING tags
		
		--Parseamos components
		components = _descriptorObject.getComponents()
		for c in components do (
			parseComponent c
		)
	)
)

----------------------------------------------------------------------
-- LEVEL--
---------------------------------------------------------------------
fn _parseLevel = (
	local xml = XMLNode()
	_lua_code = ""
	xml.nombre = "entities"
	for obj in $* do (
		_parsingObject = obj
		_parseObject xml
		--print (xml.parse())
	)
	_parsingObject = undefined
	return xml
)

----------------------------------------------------------------------
--TAGS--
----------------------------------------------------------------------
fn parseTags parent tag = (
	if (ClassOf tag) != (ClassOf #()) then (
		parseTags parent #(t)
	) else (
		local xml = parent.addChild "tags"
		local tags_list = ""
		for t in tag do (
			tags_list += (" " + t)
		)
		xml.addAttr "tags" tags_list
	)
)

----------------------------------------------------------------------
-- COMPONENTS --
---------------------------------------------------------------------
fn parseComponent comp = (
	-- Realizamos la llamada de parse
	local parser = ("_parse_" + comp)
	--print ("Component execute before: " + (comp as string))
	local parserFn = execute parser
	--print ("Component execute after: " + (comp as string))
	if parserFn == undefined then (
		--print ("Component no existe: " + (comp as string))
		XML_PARSING.addChild comp
	) else (
		--print ("Component existe: " + (comp as string))
		execute (parser + "()")
	)
)

----------------------------------------------------------------------
-- TEST Object --
---------------------------------------------------------------------
--Test (ver el xml generado por un objeto)
fn xmlTest obj = (
	local xml = XMLNode()
	xml.nombre = "entities"
	_parsingObject = obj
	_parseObject xml
	_parsingObject = undefined
	xml.parse()
)