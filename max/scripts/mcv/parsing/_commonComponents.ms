-- -- COMMON COMPONENTS -- --

fn _parsePolarized xml = (
	local polarized = findHelper "Polarized"
	if (polarized != undefined) then (
		xmlPol = xml.addChild "polarized"
		xmlPol.addAttr "pol" (getUserProp polarized "pol")
	)
)

fn _parseCommonComponents xml = (
	
	-- Polarized
	_parsePolarized xml
)