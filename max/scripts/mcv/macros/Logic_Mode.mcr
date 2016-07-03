macroScript LogicMode
	category:"_ItLightens!"
	toolTip:"Logic"
(
	clearSelection()
	for obj in $* do (
		if ( (getUserProp obj "logic") as string == "yes") then (
			selectMore obj
			continue
		)
		if (getUserProp obj "typeObject") != undefined then (
			selectMore obj
			continue
		)
		if (getUserProp obj "typeHelper") != undefined then (
			selectMore obj
			continue
		)
	)
	--actionMan.executeAction 0 "40140"
)