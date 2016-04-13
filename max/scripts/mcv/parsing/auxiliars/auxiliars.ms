-- Parsing Auxiliares
--******************

fn parseable = (
	-- Es spline?
	if (isSpline _parsingObject) then (return false)
	
	-- Comprobamos que no sea un Helper
	if (getUserProp _parsingObject "typeHelper") != undefined then (return false)
	
	return true
)

-- Valores -- (poner entre comillas)
fn _val val = (
	local valTrimmed = trimLeft (trimRight (val as string) )
	return "\"" + (valTrimmed as string) + "\""
)

fn isSpline obj = (
	try (
		local nspl =numSplines obj;
		return (nspl > 0 )
	) catch ( return false)
)