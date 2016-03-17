tool PointCreator
(
	local p, createPoint
	
	fn createPoint = in coordsys grid p=point pod:gridPoint
		
	on mousePoint clickno do
	(
		if clickno == 1
		then createPoint()
		else if p != undefined do (p.pos=worldPoint; p=undefined)
	)

	on mouseMove clickno do
	(
		if p != undefined
		then p.pos=worldPoint
		else if lbutton do createPoint()
		
	)
)

startTool PointCreator
