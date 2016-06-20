-- -- OPTIMIZE -- --

--Components optimization --
fn _parse_room = (
	local room = _parsingObject.room_name
	if (room != undefined and room != "none") then (
		local xmlRoom = XML_PARSING.addChild "room"
		xmlRoom.addAttr "name" room
	)
)