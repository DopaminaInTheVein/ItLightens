-- P A R S E    Characters' s --
--***********************
--------------------------------------------------------------------------------------------------------------------------------------------------------------
--- Auxiliares ---
--------------------------------------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------
-- WAY POINTS --
----------------------------------------------------------------------
fn parseWayPoints parent = (
	local patrols = (findAllHelpers "Patrol_Go")
	if patrols.count > 0 then (
		parent.addAttr "wpts_size" patrols.count
		local curWpt = 0
		for p in patrols do (
			--Pos
			local mcv_tmx = matrixMax2MCV p.transform -- matrix3
			atrName =  "wpt" + (curWpt as string) + "_pos"
			atrValue = getTransformPosValue mcv_tmx
			parent.addAttr atrName atrValue
			
			-- Wait
			atrName = "wpt" + (curWpt as string) + "_wait"
			atrValue = (getUserProp p "waitTime")
			if (atrValue == undefined) then atrValue = 0
			parent.addAttr atrName atrValue
			
			--Next
			curWpt += 1
		)
	)
)

----------------------------------------------------------------------
-- KEY POINTS (Look & Go) --
----------------------------------------------------------------------
fn parseKeyPoints parent = (
	local patrols = (findAllHelpers #("Patrol_Go", "Patrol_Look"))
	if patrols.count > 0 then (
		parent.addAttr "kpt_size" patrols.count
		local curKpt = 0
		for p in patrols do (
			
			--Type
			local atrName = "kpt" + (curKpt as string) + "_type"
			local atrValue = undefined
			local curHelper = getUserProp p "typeHelper"
			if (curHelper == "Patrol_Go") then (
				atrValue = "seek"
			) else if (curHelper == "Patrol_Look") then (
				atrValue = "look"
			) else (
				throw "Tipo de Patrol Helper desconocido!"
			)
			parent.addAttr atrName atrValue
			
			-- Position
			local mcv_tmx = matrixMax2MCV p.transform -- matrix3
			atrName =  "kpt" + (curKpt as string) + "_pos"
			atrValue = getTransformPosValue mcv_tmx
			parent.addAttr atrName atrValue
			
			-- Wait
			atrName = "kpt" + (curKpt as string) + "_wait"
			atrValue = (getUserProp p "waitTime")
			if (atrValue == undefined) then atrValue = 0
			parent.addAttr atrName atrValue
			
			-- Next
			curKpt += 1
		)
	)
)

----------------------------------------------------------------------
-- JURISDICTION --
----------------------------------------------------------------------
fn parseJurisdiction parent = (
	local jurisdiction = (findHelper "Jurisdiction")
	if jurisdiction != undefined then (
		--Jurisdiction Pos
		local mcv_tmx = (matrixMax2MCV jurisdiction.transform)
		parent.addAttr "jurisdiction" (getTransformPosValue mcv_tmx)
		
		--Jurisdiction Radius
		parent.addAttr "jurRadius" jurisdiction.radius
	)
)

----------------------------------------------------------------------
-- FORMATION --
----------------------------------------------------------------------
fn parseFormation parent = (
	local formationGo = (findHelper "Formation_Go")
	if formationGo != undefined then (
		--Formation Go
		local mcv_tmx = (matrixMax2MCV formationGo.transform)
		local goPos = (getTransformPosValue mcv_tmx)
		parent.addAttr "formation_point" goPos
		
		local formationLook = (findHelper "Formation_Look")
		if formationLook != undefined then (
			--Formation Look
			local mcv_tmx_look = (matrixMax2MCV formationLook.transform)
			local lookPos = (getTransformPosValue mcv_tmx_look)
			
			parent.addAttr "formation_dir" (getTransformPosValue mcv_tmx_look)		
		) else (
			throw "Cannot be there was Formation Go but not Formation Look!"
		)
	)
)

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--- Components ---
----------------------------------------------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------
-- WORKSTATION --
----------------------------------------------------------------------
fn _parse_workstation = (
	xml = XML_PARSING.addChild "workstation"
	local keyPoint = (findHelper "keyPoint")
	if keyPoint != undefined then (
		--Key Point
		local mcv_tmx = (matrixMax2MCV keyPoint.transform)
		xml.addAttr "posAction" (getTransformPosValue mcv_tmx)
		xml.addAttr "yawAction" (degtorad (mcv_tmx.rotation as eulerangles).x)
		parsePropAttribute xml "animation" "animation"
	)
)
----------------------------------------------------------------------
-- LIFE --
----------------------------------------------------------------------
fn _parse_life = (
	xmlLife = XML_PARSING.addChild "life"
	xmlLife.addAttr "points" (getUserProp _parsingObject "life")
)

----------------------------------------------------------------------
-- BT_GUARD --
----------------------------------------------------------------------
fn _parse_bt_guard = (
	btXml = XML_PARSING.addChild "bt_guard"
	parseKeyPoints btXml -- KptPoints
	parseJurisdiction btXml --Jurisdiction
	parseFormation btXml --Formation
)

----------------------------------------------------------------------
-- BT_MOLE --
----------------------------------------------------------------------
fn _parse_bt_mole = (
	btXml = XML_PARSING.addChild "bt_mole"
	parseWayPoints btXml
)

----------------------------------------------------------------------
-- BT_SPEEDY --
----------------------------------------------------------------------
fn _parse_bt_speedy = (
	btXml = XML_PARSING.addChild "bt_speedy"
	parseWayPoints btXml
)