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
-- STUNT RECOVER --
----------------------------------------------------------------------
fn parseStuntRecover parent = (
	local stunt_recover = getUserProp _parsingObject "stunt_recover"
	if stunt_recover != undefined then (
		parent.addAttr "stunt_recover" stunt_recover
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
	local keyPoint
	if (classof _parsingObject) == Delegate then (
		keyPoint = _parsingObject
	) else(
		keyPoint = (findHelper "keyPoint")
	)
	if keyPoint != undefined then (
		xml = XML_PARSING.addChild "workstation"
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
	parseStuntRecover btXml --Formation
)

----------------------------------------------------------------------
-- BT_MOLE --
----------------------------------------------------------------------
fn _parse_bt_mole = (
	btXml = XML_PARSING.addChild "bt_mole"
	parseWayPoints btXml
	local max_patrol = getUserProp _parsingObject "max_patrol"
	if (max_patrol != undefined) then (
		btXml.addAttr "max_patrol" max_patrol
	)
)

----------------------------------------------------------------------
-- BT_SPEEDY --
----------------------------------------------------------------------
fn _parse_bt_speedy = (
	btXml = XML_PARSING.addChild "bt_speedy"
	parseWayPoints btXml
)

----------------------------------------------------------------------
-- DRONE --
----------------------------------------------------------------------
fn _parse_drone = (

	-- Drone component
	local xmlDrone = XML_PARSING.addChild "drone"
	
	
	--Speed
	parsePropAttribute xmlDrone "speed" "speed"
	
	--Espatllat
	parsePropAttribute xmlDrone "espatllat" "espatllat"
	
	--Life
	life = getUserProp _parsingObject "life"
	if (life != undefined) then (
		_parse_life()
	)
	
	--Patrol
	parseWayPoints xmlDrone
	
	--Textures self ilumination
	local textureFront = getUserProp _parsingObject "textureFront"
	xmlDrone.addAttr "textureFront" textureFront
	
	local textureBack = getUserProp _parsingObject "textureBack"
	xmlDrone.addAttr "textureBack" textureBack
)