-- -- LIGHTS -- --
fn _parse_lightpoint = (
	local xml = XML_PARSING.addChild "light_point"
	
	--Color attribute
	local col = _parsingObject.rgb/255
	local colorValue = getAsXmlValues #(col.r, col.g, col.b, col.a)
	xml.addAttr "color" colorValue
	
	--In_radius
	xml.addAttr "in_radius" _parsingObject.farAttenStart
	
	--Out_radius
	xml.addAttr "out_radius" _parsingObject.farAttenEnd
)