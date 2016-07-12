fn checkMatName mat_name = (
	tieneEspacios = (findString mat_name " ")
	tieneAlmohadilla = (findString mat_name "#")
	if (tieneEspacios != undefined) do (
		return false
	)
	if (tieneAlmohadilla != undefined) do (
		return false
	)
	return true
)

fn checkAllMaterials = (
	errors = #()
	for obj in $* do (
		mat = obj.mat
		if mat != undefined then (
			if not (checkMatName mat.name) then (
				append errors obj.name
			)
		)
	)
	return errors
)

res = checkAllMaterials()
for v in res do (
	print v
)