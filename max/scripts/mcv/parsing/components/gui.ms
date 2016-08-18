-- P A R S E   G U I --
--***********************

fn _parse_gui_button = (
	local xml = XML_PARSING.addChild "gui_button"
	local size = _parsingObject.max - _parsingObject.min
	xml.addAttr "width" size.x
	xml.addAttr "height" size.z
)
