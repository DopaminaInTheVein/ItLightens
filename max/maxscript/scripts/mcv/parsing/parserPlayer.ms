-- Parse Object of type --
-- -- PLAYER -- --
fn _parsePlayer = (
	local obj = _parsingObject
	format "_parsePrueba, obj = %\n" _parsingObject
	local name = _val obj.name
	local atr2 = _val 5
	local res = ""
	res += "<entity>" + "\n"
	res += _parseTransform()
	res += "  <TCompName" + "\n"
	res += "    name=" + name + "\n"
	res += "  />" + "\n"
	res += "</entity>" + "\n"
	return res
)