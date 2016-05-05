-- Convert matrix from MAX to MCV (Direct x)
fn matrixMax2MCV mtx = (
	local tm = rotateXMatrix -90
	local tm_inv = inverse tm
	return tm_inv * mtx * tm
)

-- Delete Hierarchy
fn deleteHierarchy obj = (
	for c in obj.children do (
		deleteHierarchy c
	)
	delete obj
)

-- Get Hierarchy
fn getHierarchyRec obj res = (
	for c in obj.children do (
		getHierarchyRec c res 
	)
	append res obj
)

fn getHierarchy obj = (
	res = #()
	getHierarchyRec obj res
	return res
)

fn getHierarchyRoot obj = (
	local res = obj
	while ( res.parent != undefined or (classof res.parent != Dummy) ) do (
		res = res.parent
	)
	return res
)

-- List Values for XML ("val_1 val_2 val_3..." without microvalues ( 1.5836e-7)
fn getAsXmlValues values = (
	local res = ""
	for v in values do (
		if ((abs v) < 0.01) then v = 0
		res += (v as string) + " "
	)
	return (trimRight res)
)

-- Format Transform Pos Value
fn getTransformPosValue transf = (
	local values = #(transf.pos.x, transf.pos.y, transf.pos.z)
	return getAsXmlValues values
)

-- Format Transform Rotation Value
fn getTransformRotValue transf = (
	local q = transf.rotationPart as quat
	local values = #(-q.x, -q.y, -q.z, q.w)
	return getAsXmlValues values
)

-- Obtener todos los ficheros max de un directorio
fn getAllMaxIn dir = (
	local res = #()
	local maxFiles = getFiles (dir + "*.max")
	for f in maxFiles do
	(
		append res (getFilenameFile f)
	)
	return res
)

-- Existe un objeto con el nombre "nombre"?
fn objExists nombre = (
	queryName = "$'"+nombre+"'"
	obj = execute (queryName)
	return (obj != undefined)
)

-- Numera un objeto insertado!
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

-- Editable prop (props editadas mejor?)
fn getEditableProps obj = (
	local props = getUserPropBuffer obj
	props = filterString props "=, \n"
	local res = #()
	local i = 1
	while i < props.count do (
		append res props[i]
		i += 2
	)
	return res
)

---------------------------------------------------------
-- Buscar Helpers
---------------------------------------------------------
fn findHelper tHelper = (
	local res = undefined
	for child in _parsingObject.children do (
		if ( (getUserProp child "typeHelper") == tHelper ) then (
			res = child
			exit
		)
	)
	return res
)

fn findAllHelpersList tHelper = (
	local res = #()
	for child in _parsingObject.children do (
		childHelper =  (getUserProp child "typeHelper")
		if ( (findItem tHelper childHelper) > 0) then (
			append res child
		)
	)
	return res
)

fn findAllHelpers tHelper = (
	if (classof tHelper) == Array then return (findAllHelpersList tHelper)
	else return (findAllHelpersList #(tHelper))
)

-- LUA --
-----------------------------------------------------------------------------------------------------------------
--Append Lua
fn luaAppend nameFunc code = (
	_lua_code += "function " + nameFunc + "( )" + "\n"
	_lua_code += code
	_lua_code += "\nend\n\n"
)

-- Strings
------------------------------------------------------------------------------------------------fn strReplace str strOld strNew = (
fn strReplace str strOld strNew = (
	while ((i=findstring str strOld) != undefined) do
	(
		str = replace str i strNew.count strNew
	)
	return str
)