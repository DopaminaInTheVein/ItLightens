-- OBJECTS --
--***********************

fn _parse_box = (
	local collider = findHelper "Collider"
	local xml = XML_PARSING.addChild "box"
	if collider != undefined then (
		local ss = stringstream ""
		format "% % %" collider.Width collider.Height collider.Length to:ss
		xml.addAttr "size" (ss as string)
	)
)

fn _parse_box_placer = (
	local xml = XML_PARSING.addChild "box_placer"
	local lua_event = getUserProp _parsingObject "event"
	xml.addAttr "event" lua_event
)