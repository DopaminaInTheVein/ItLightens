-- -- OPTIMIZE -- --

--Components optimization --
fn _parse_room = (
	local room = getUserProp _parsingObject "room"
	if (room != undefined) then (
		local xmlRoom = XML_PARSING.addChild "room"
		xmlRoom.addAttr "name" room
	)
)