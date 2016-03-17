clearListener()

listComponents = #("mesh", "staticMesh")

struct TComponentListItem (
	nombre,
	valor
)

struct TObject (
	obj,
	
	fn setObj _obj = (
		obj = _obj
	),
	
	fn getComponentList = (
		result = #()
		for c in listComponents do (
			local item = TComponentListItem()
			item.nombre = c
			item.valor = getUserProp obj c
			append result item
		)
		return result
	),
	
	fn getValor index = (
		return  getUserProp obj listComponents[index]
	),
	
	fn setValor nombre valor = (
		setUserProp obj nombre valor
	)
)

fn getAllObjectTypes = (
	local res = #()
	local maxFiles = getFiles (objMax_dir + "*.max")
	for f in maxFiles do
	(
		f
		append res (getFilenameFile f)
	)
	return res
)


