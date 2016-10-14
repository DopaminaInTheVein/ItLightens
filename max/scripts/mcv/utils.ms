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
	return findHelperIn _parsingObject tHelper
)

fn findHelperIn obj tHelper = (
	local res = undefined
	for child in obj.children do (
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
	if (classof tHelper) == Array then (
		local res = (findAllHelpersList tHelper)
		return (sortByName res)
	) else (
		return (findAllHelpersList #(tHelper))
	)
)

---------------------------------------------------------
-- Buscar Class Object
---------------------------------------------------------
fn findClass tClass = (
	local res = undefined
	for child in _parsingObject.children do (
		if ( classof child == tClass ) then (
			res = child
			exit
		)
	)
	
	return res
)

fn findAllClassList tClass = (
	local res = #()
	for child in _parsingObject.children do (
		if ( classof child == tClass ) then (
			append res child
		)
	)
	return sortByName res
)

-- LUA --
-----------------------------------------------------------------------------------------------------------------
--Append Lua
fn luaAppend nameFunc code = (
	_lua_code += "function " + nameFunc + "( )" + "\n"
	_lua_code += (code + "()")
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

fn getSuffixAfter str keyword = (
	local index = findString str keyword
	local res
	if index == undefined then (
		res = undefined
	) else (
		res = substring str (index + keyword.count) -1
	)
	return res
)	

fn sortByName arr = (
  res = #()
  nameOrdered = sort(for i in arr collect i.name)
  for nom in nameOrdered do (
	for elem in arr do (
		if elem.name == nom then (
			append res elem
			exit
		)
	)
  )
  return res
)


-- Attributes for room configuration
v_room_data = attributes v_room_data version:1 ( 	parameters room_data 	( 		room_name  type:#string  default:"-1"  	) ) 

-- Apply attribute class to an object
fn assign_default_room_to_all = (
	for obj in $* do (
		if not (hasProperty obj "room_name") then (
			add_attribute = custAttributes.add obj v_room_data baseobject:True
		)
	)
)

fn assign_room_name_to selected rname = (
	if selected.room_name == "-1" then (	
		selected.room_name = rname
	) else (
		selected.room_name += "/" + rname
	)
)

fn assign_room_name_to_selected rname = (
		for obj in $ do (
			assign_room_name_to obj rname
		)
)

fn _is_ui_scene = (
	for obj in $* do (
		local h = getUserProp obj "typeHelper"
		if (h as string) == "znear" then (return true)
	)
	return false
)

 --  Add TAGS --
fn addTagTo obj tag = (
	local current_tag = getUserProp obj "tags"
	if current_tag == undefined then (current_tag = "")
	setUserProp obj "tags" (current_tag + tag)
)

fn addTag tag = (
	if (classof $) == ObjectSet then (
		for obj in $ do (
			addTagTo obj tag
		)
	) else (
		addTagTo $ tag
	)
)

 --  SetProp --
fn setPropTo obj prop val = (
	setUserProp obj prop val
)

fn setProp prop val = (
	if (classof $) == ObjectSet then (
		for obj in $ do (
			setPropTo obj prop val
		)
	) else (
		setPropTo $ prop val
	)
)
 
 
-- FIXES SCENE
fn fixUserProps = (
	names_modified = #()
	for obj in $* do (
		prop = getUserPropBuffer obj
		if prop.count > 0 then(
			if prop[prop.count] != "\n" then (
				setUserPropBuffer obj (prop + "\n")
				append names_modified obj.name
			)
		)
	)
	for nombre in names_modified do (
		print nombre
	)
)
	
-- FILES
fn getFolderFromPath folder = (
	local sub_folders = filterString folder "\\"
	local res = ""
	if sub_folders.count > 0 then (
		res = sub_folders[1]
		if sub_folders.count > 1 then (
			for i = 2 to sub_folders.count - 1 do (
				res += ("\\" + sub_folders[i])
			)
		)
	)
	return res
)

fn copyFileAndDirs orig dest = (
	local res = false
	local destFolder = (getFolderFromPath dest)
	local dirs = getDirectories (destFolder)
	if dirs.count == 0 then (
		if (makeDir destFolder) then (
			res =copyFile orig dest
		)
	) else (
		res = copyFile orig dest
	)
	return res
)

-- UI --
-- Transfer ui object
fn transferUi orig dest = (
	print orig.name
	print dest.name
	dest.mat = orig.mat
	local props = getUserPropBuffer orig 
	setUserPropBuffer dest props
	dest.name = orig.name
)

-- Selections
fn selectSpecial = (
	clearSelection()
	for obj in $* do (
		if (getUserProp obj "typeObject" != undefined) then (
				selectMore obj
		)
	)	
)

fn selectDynamic = (
	clearSelection()
	for obj in $* do (
		if (getUserProp obj "staticMesh") == undefined then (
			if (getUserProp obj "skeleton") == undefined then (
				continue
			)
		)
		if not (isStatic obj) then (
			selectMore obj
		)
	)	
)