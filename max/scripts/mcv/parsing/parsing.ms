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
_entities_ids = #()
XML_PARSING = undefined
_lua_code = ""
_xml_sufix = "" -- init, ...
_

--*******************************************************
-------------FUNCTIONS-------------------------------------
--*******************************************************
---------------------------------------------------------------------
--Entity Id --
---------------------------------------------------------------------
fn parseId = (
	local id = getUserProp _parsingObject "id"
	if id != undefined then (
		id = id as integer
		if id == undefined then (
			throw ("id de la entidad debe ser un n�mero entero! [" + _parsingObject.name + "]")
		)
		if _entities_ids[id] == undefined then (
			_entities_ids[id] = true
			XML_PARSING.addAttr "id" id
		) else (
			throw ("id repetido! [" + _parsingObject.name + "]")
		)
	)	
)

fn parseSlept = (
	local slept = getUserProp _parsingObject "slept"
	if slept != undefined then (
		XML_PARSING.addAttr "slept" (slept as string)
	)
)

fn parsePermanent = (
	local permanent = getUserProp _parsingObject "permanent"
	if permanent != undefined then (
		XML_PARSING.addAttr "permanent" (permanent as string)
	)	
)

fn parseReload = (
	local reload = getUserProp _parsingObject "reload"
	if reload != undefined then (
		XML_PARSING.addAttr "reload" (reload as string)
	) else (
		if _xml_sufix != "ui_scene" then (	
			local type = getUserProp _parsingObject "typeObject"
			if type != undefined then (
				--if type == "Trigger" then (
					--XML_PARSING.addAttr "reload" "no"
				--) else (
				XML_PARSING.addAttr "reload" "yes"
				--)
			)
		)
	)
)

fn parsePad= (
	parsePropAttribute XML_PARSING "only_pad" "only_pad"
	parsePropAttribute XML_PARSING "only_keyboard" "only_keyboard"
)

fn parseTemp = (
	parsePropAttribute XML_PARSING "temp" "temp"
)

---------------------------------------------------------------------
-- OBJECT --
---------------------------------------------------------------------
fn _parseObject parent = (
	
	print ("Object: " + (_parsingObject.name as string))
	-- Obtenemos descriptor del objeto
	_descriptorObject = OManager.getDescriptor _parsingObject
	if parseable() then (
		-- A�adimos Entity
		XML_PARSING = parent.addChild "entity"
		parseId()
		parseSlept()
		parsePermanent()
		parseReload()
		parsePad()
		parseTemp()
		
		--Parseamos Tags
		tags = _descriptorObject.getTags()
		parseTags XML_PARSING tags
		
		
		if not parseMonoComponent() then (			
			--Parseamos components
			components = _descriptorObject.getComponents()
			for c in components do (
				parseComponent c
			)
		)
	)
)

----------------------------------------------------------------------
-- LEVEL--
---------------------------------------------------------------------
fn _parseLevel sufix = (
	local xml = XMLNode()
	--_lua_code = ""
	_xml_sufix = sufix
	_entities_ids = #()
	
	xml.nombre = "entities"
	for obj in $* do (
		obj.name = strReplace obj.name "�" "n"
		if not obj.isHidden then (
			_parsingObject = obj
			_parseObject xml
			--print (xml.parse())
		)
	)
	_parsingObject = undefined
	_entities_ids = #()
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
		local tags_list = getUserProp _parsingObject "tags"
		if tags_list == undefined then (tags_list = "")
		for t in tag do (
			tags_list += (" " + t)
		)
		if tags_list != "" then (xml.addAttr "tags" tags_list)
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
	-- Fill default values
	assign_default_room_to_all()
	
	local xml = XMLNode()
	xml.nombre = "entities"
	_parsingObject = obj
	_parseObject xml
	_parsingObject = undefined
	xml.parse()
)

fn xmlTestUi obj = (
	_xml_sufix ="ui_scene"
	xmlTest obj
)