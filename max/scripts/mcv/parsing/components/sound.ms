-- -- SOUND -- --

--Common
fn _parse_sound = (
	local xml = XML_PARSING.addChild "sound"
	parsePropAttribute xml "event" "event"
	parsePropAttribute xml "volume" "volume"
)