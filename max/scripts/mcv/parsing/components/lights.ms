-- -- LIGHTS -- --

--Common
fn _parse_light_common xml = (
		--Color attribute
	local col = _parsingObject.rgb/255
	local colorValue = getAsXmlValues #(col.r, col.g, col.b, _parsingObject.multiplier)
	xml.addAttr "color" colorValue
	
	--In_radius
	xml.addAttr "in_radius" _parsingObject.farAttenStart
	
	--Out_radius
	xml.addAttr "out_radius" _parsingObject.farAttenEnd
)

-- Resolution
fn _parse_light_resolution xml = (
	local res = getUserProp _parsingObject "res"
	if res != undefined then (xml.addAttr "resolution" res)
)

-- Resolution
fn _parse_light_mask xml = (
	local mascara = _parsingObject.projectorMap
	if mascara != undefined then (
		local mascara_file = fileNameFromPath (mascara.filename)
		xml.addAttr "light_mask" mascara_file
	)
)

-- Light Point
fn _parse_lightpoint = (
	local xml = XML_PARSING.addChild "light_point"
	--Light Common
	_parse_light_common xml
)

--Light Dir & Light Dir Shadows
fn _parse_lightDir = (
	local xml 
	if _parsingObject.castShadows then (
		xml = XML_PARSING.addChild "light_dir_shadows"
	) else (
		xml = XML_PARSING.addChild "light_dir"
	)
	
	--Light Common
	_parse_light_common xml
	
	--Parse light resolution
	_parse_light_resolution xml
	
	--Parse light mask
	_parse_light_mask xml
)