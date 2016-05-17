-- P A R S E    D O O R S and E L V A T O R S --
--******************************************--

--- Auxiliares ---
--------------------------------------------------------------------------------------------------------------------------------------------------------------
fn parseSpeeds xml = (
	parsePropAttribute xml "speedOpening" "speedOpening"
	parsePropAttribute xml "speedClosing" "speedClosing"
)

fn parseTarget xml = (
	local target = findClass Point
	if target != undefined then (
		local mcv_transform = (matrixMax2MCV target.transform)
		local pos = (getTransformPosValue mcv_transform)
		xml.addAttr "target" pos
	)
)

fn parseLocked xml = (
	parsePropAttribute xml "locked" "locked"
)

--- COMPONENTS---
----------------------------------------------------------------------------------------------------------------------------------------------------------------

-- Magnet Door --
--------------------
fn _parse_magnet_door = (
	--Magnet door component
	local xml = XML_PARSING.addChild "magnet_door"
	--Locked
	parseLocked xml
	--Speeds
	parseSpeeds xml
	--Target
	parseTarget xml
)