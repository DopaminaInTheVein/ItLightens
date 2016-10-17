-- Parsing Auxiliares
--******************

fn parseable = (
	--Is hidden?
	if _parsingObject.ishidden then (
		local slept = getUserProp _parsingObject "slept"
		if (slept as string) != "yes" then ( return false)
	)	

	-- Es container?
	if (classof _parsingObject) == Container then (
		return false
	)	
	
	-- Es dummy?
	if (classof _parsingObject) == Dummy then (
		local ok = false
		local type = getUserProp _parsingObject "typeObject"
		if (type as string) == "railCamera" then (ok = true)
		if (type as string) == "BoxPlacer" then (ok = true)
		if (type as string) == "LookTarget" then (ok = true)
		if not ok then (return false)
	)
	
	-- FreeCameras & TargetCameras
	if (classof _parsingObject) == FreeCamera then (
		local type = getUserProp _parsingObject "typeObject"
		if type != "CamController" then (return false)	
		--if type == undefined or type == "railCamera" then (return false)
	)
	if (classof _parsingObject) == TargetCamera then (
		return false
	)
	--TargetObject
	if (classof _parsingObject) == TargetObject then (
		return false
	)
		
	-- Comprobamos que no sea un Helper
	if (getUserProp _parsingObject "typeHelper") != undefined then (
		return false
	)

	-- Reload <--> sufijo _init
	local reload = getUserProp _parsingObject "reload"
	local isReload = false
	if (reload as string) == "yes" then (
		isReload = true
	) else if (reload as string) == "no" then (
		isReload = false
	) else if (getUserProp _parsingObject "typeObject") != undefined then (
		isReload = true	
	)
	if _xml_sufix != "ui_scene" then (		
		if isReload and _xml_sufix != "init" then (
			return false
		)
		
		if (not isReload) and _xml_sufix == "init" then (
			return false
		)
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

-- Is Static object
fn isStatic obj = (
	local static = (getUserProp obj "static")
	if static != undefined then (
		return (static == "yes")
	)
	return ((getUserProp obj "typeObject") == undefined)
)

--Parse generic attribute
fn parsePropAttribute xml prop attr= (
	local prop = getUserProp _parsingObject prop
	if prop != undefined then (xml.addAttr attr prop)
)