-- -- LIGHTS -- --

--Common
fn _parse_light_common xml is_ldir = (
	--Color attribute
	local col = _parsingObject.rgb/255
	local colorValue = getAsXmlValues #(col.r, col.g, col.b, _parsingObject.multiplier)
	xml.addAttr "color" colorValue
	
	--In_radius
	local near = "in_radius"
	if is_ldir then ( near = "znear")
	xml.addAttr  near _parsingObject.farAttenStart
	
	--Out_radius
	local far = "out_radius"
	if is_ldir then ( far = "zfar" )
	xml.addAttr far _parsingObject.farAttenEnd
	
	--Use ramp color?
	parsePropAttribute xml "ramp" "ramp"
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
	_parse_light_common xml false
)

--Light Dir & Light Dir Shadows
fn _parse_lightDir = (
	local xml 
	--if _parsingObject.castShadows then (
	local shadow_dyn = (getUserProp _parsingObject "ldynamic") as string
	if shadow_dyn == "yes" then(
		xml = XML_PARSING.addChild "light_dir_shadows_dynamic"
	) else (
		xml = XML_PARSING.addChild "light_dir_shadows"
		local gen_shadow = "yes"
		if not _parsingObject.castShadows then (
			gen_shadows = "no"
		)
		xml.addAttr "gen_shadow" gen_shadow
		XML_PARSING.addChild "culling"
	)
		
	--) else (
	--	xml = XML_PARSING.addChild "light_dir"
	--
	
	--Light Common
	_parse_light_common xml true
	
	--Parse light resolution
	_parse_light_resolution xml
	
	--Parse light mask
	_parse_light_mask xml
	
	--Parse light fov
	xml.addAttr "fov" _parsingObject.falloff
)