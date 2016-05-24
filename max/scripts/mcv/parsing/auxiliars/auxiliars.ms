-- Parsing Auxiliares
--******************

fn parseable = (
	--Is hidden?
	if _parsingObject.ishidden then (
		local slept = getUserProp _parsingObject "slept"
		if (slept as string) != "yes" then ( return false)
	)	
	
	-- Es dummy?
	if (classof _parsingObject) == Dummy then (return false)
	
	-- FreeCameras
	if (classof _parsingObject) == FreeCamera then (
		local type = getUserProp _parsingObject "typeObject"
		if type == undefined or type == "railCamera" then (return false)
	)
	
	-- Es spline?
	if (isSpline _parsingObject) then (
		if ((getUserProp _parsingObject "typeObject" ) as string) != "railCamera" then (
			return false
		)
	)
	
	-- Comprobamos que no sea un Helper
	if (getUserProp _parsingObject "typeHelper") != undefined then (
		return false
	)
	
	return true
)

-- Valores -- (poner entre comillas)
fn _val val = (
	local valTrimmed = trimLeft (trimRight (val as string) )
	return "\"" + (valTrimmed as string) + "\""
)

fn isSpline obj = (
	try (
		local nknots =numKnots obj;
		return (nknots > 0 )
	) catch ( return false)
)

--Parse generic attribute
fn parsePropAttribute xml prop attr= (
	local prop = getUserProp _parsingObject prop
	if prop != undefined then (xml.addAttr attr prop)
)