__newObj = undefined
tool PointCreator2
(
	local state = "st_create"
	local yWhenClick = 0
		
	on start do (
			format "Se inicia tool\n"
		__newObj.pos = [-1000, -1000, -1000]
	)
	
	on mousePoint clickno do
	(
		format "Entro en mouse Click: % \n" clickno
		if clickno == 2 then (
			state = "st_movingXY"
			format "creo Punto\n"
			__newObj.pos = worldPoint
		) else if clickno == 3 then (
			state = "st_movingZ"
			yWhenClick = viewPoint.y
			format "asigno XY\n"
		) else if clickno == 4 then (
			--state = "st_create"
			format "Point created\n"
			#stop
		)
	)

	on mouseMove clickno do
	(
		if __newObj != undefined do (
			if state == "st_movingXY" then (
				__newObj.pos = worldPoint
			) else if state == "st_movingZ" then (
				__newObj.pos.z = yWhenClick - viewPoint.y
			)
		)		
	)
)

fn drawObj nombre = (
	local filePath = objMax_dir + nombre + ".max"
	if mergemaxfile filePath #(nombre) then (
		local labelObj = "$'" + nombre + "'"
		__newObj = execute(labelObj)
		__newObj.name += "_"
		if startTool PointCreator2 == #abort then delete __newObj
	) else (
		messageBox "Error cargando objeto " + nombre + " del fichero " + nombre + ".max"
	)
)

drawObj "player"


