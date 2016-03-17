-- Parse Object of type --
-- -- PRUEBA -- --
fn _parsePrueba = (
	local obj = _parsingObject
	format "_parsePrueba, obj = %\n" _parsingObject
	local atr1 = _val "hola"
	local atr2 = _val 5
	local res = ""
	res += "<entity>" + "\n"
	res += _parseTransform()
	res += "  <comp1" + "\n"
	res += "    atr1=" + atr1 + "\n"
	res += "    atr1=" + atr2 + "\n"
	res += "  />" + "\n"
	res += "</entity>" + "\n"
	return res
)