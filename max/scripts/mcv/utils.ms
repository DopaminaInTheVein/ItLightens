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
	while ( res.parent != undefined ) do (
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