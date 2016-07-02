macroScript LogicMode
	category:"_ItLightens!"
	toolTip:"Logic"
(
	clearSelection()
	for obj in $* do (
		if ( (getUserProp obj "logic") as string == "yes") then (
			selectMore obj
		)
	)
	--actionMan.executeAction 0 "40140"
)