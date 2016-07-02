macroScript SceneMode
	category:"_ItLightens!"
	toolTip:"Scene"
(
	clearSelection()
	for obj in $* do (
		if ( (getUserProp obj "logic") as string != "yes") then (
			if obj.name != "_Logic" then (selectMore obj)
		)
	)
)