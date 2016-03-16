tool PointCreator2
(
	local p, createPoint
	local state = "st_create"
	local yWhenClick = 0
	fn createPoint = in coordsys grid p=point pod:gridPoint
		
	on start do (
			format "Se inicia tool\n"
	)
	
	on mousePoint clickno do
	(
		format "Entro en mouse Click: % \n" clickno
		if clickno == 2 then (
			createPoint()
			state = "st_movingXY"
			format "creo Punto\n"
		) else if clickno == 3 then (
			state = "st_movingZ"
			yWhenClick = viewPoint.y
			format "asigno XY\n"
		) else if clickno == 4 then (
			state = "st_create"
			format "Point created\n"
			#stop
		)
	)

	on mouseMove clickno do
	(
		if p != undefined do (
			if state == "st_movingXY" then (
				p.pos = worldPoint
			) else if state == "st_movingZ" then (
				p.pos.z = yWhenClick - viewPoint.y
			)
		)		
	)
)

end = false
while end == false do (
	end = (startTool PointCreator2 == #abort)
)
