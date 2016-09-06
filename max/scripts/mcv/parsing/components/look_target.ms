-- -- LOOK TARGET -- --

----------------------------------------------------------------------
-- LOOK TARGET --
---------------------------------------------------------------------
fn _parse_look_target = (
	local xml = XML_PARSING.addChild "look_target"
	parsePropAttribute xml "event_lua_in" "event_lua_in"
	parsePropAttribute xml "event_lua_out" "event_lua_out"
	parsePropAttribute xml "sense_vision" "sense_vision"
)