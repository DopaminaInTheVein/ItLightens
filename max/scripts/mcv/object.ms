clearListener()

listComponents = #("mesh", "staticMesh")
reservedProps = #("typeObject", "typeHelper", "defaultName")

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

fn getAllMaxIn dir = (
	local res = #()
	local maxFiles = getFiles (dir + "*.max")
	for f in maxFiles do
	(
		append res (getFilenameFile f)
	)
	return res
)

fn objExists nombre = (
	queryName = "$'"+nombre+"'"
	obj = execute (queryName)
	return (obj != undefined)
)

fn numerateObject obj = (
	obj.name += "_"
	local repeat = true
	local newName = undefined
	local index = 0
	while repeat do (
		index += 1
		newName = obj.name + (index as string)
		repeat = (objExists newName)
	)
	obj.name = newName
)

fn getEditableProps obj = (
	local props = getUserPropBuffer obj
	props = filterString props "=, \n"
	local res = #()
	local i = 1
	while i < props.count do (
		local index = findItem reservedProps props[i]
		if (index == 0) do (
			-- No es una propiedad reservada
			append res props[i]
		)
		i += 2
	)
	return res
)

