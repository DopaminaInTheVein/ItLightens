clearListener()

listComponents = #("staticMesh", "mesh")

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
	)
)

gc()
--se = TSceneExporter();
--se.exportScene "scene01"

