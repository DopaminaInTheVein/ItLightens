----------------------------------------------------------------------
-- CAMERA & Main Camera --
----------------------------------------------------------------------
fn parseCameraCommon xml = (
	xml.addAttr "znear" _parsingObject.nearrange
	xml.addAttr "zfar" _parsingObject.farrange
	xml.addAttr "fov" _parsingObject.fov
)

fn _parse_camera = (
	-- Camera
	xmlCamera = XML_PARSING.addChild "camera"
	parseCameraCommon xmlCamera
)

fn _parse_camera_main = (
	-- Camera Main
	xmlCamera = XML_PARSING.addChild "camera_main"
	parseCameraCommon xmlCamera
	XML_PARSING.addChild "culling"
	XML_PARSING.addChild "render_glow"
)

----------------------------------------------------------------------
-- Rail/Guided Camera --
----------------------------------------------------------------------
fn _parse_rail_camera = (
	
	-- Get Cameras and Targets
	local all_cameras = findAllClassList TargetCamera
	local all_targets = findAllClassList TargetObject
	--Check
	if all_cameras.count != all_targets.count then (
		throw ("Error: No match number of cameras and targets! (" + _parsingObject.name + ")")
	)
	
	--Component
	local xml = XML_PARSING.addChild "guided_camera"
	
	--Velocity
	local vel = getUserProp _parsingObject "velocity"
	if (vel == undefined) do (
		vel = "2.0"
	)
	xml.addAttr "velocity" (vel as string)

	--Default dirs ?
	local default_dirs = getUserProp _parsingObject "default_dirs"
	if (default_dirs == undefined) do (
		default_dirs = "0"
	)
	xml.addAttr "default_dirs" (default_dirs as string)
	
	--Points size
	xml.addAttr "points_size" all_cameras.count
		
	--Skippable
	parsePropAttribute xml "skippable" "skippable"
	parsePropAttribute xml "stop_final" "stop_final"
	
	-- Parse points
	local mcv_tmx
	local wPoint
	local atrName
	local atrValue
	for i = 1 to all_cameras.count do (
		--Debug
		--messagebox(all_cameras[i].name)
		
		-- Atr Name
		atrName = ("wpt" + (i) as string)
		
		--Get Point
		wPoint = all_cameras[i].pos
		
		--Transform point in atrValue
		tMatrix = matrix3 1
		tMatrix.pos = wPoint
		mcv_tmx = matrixMax2MCV tMatrix
		atrValue = getTransformPosValue mcv_tmx
		
		--Parse Attribute wpt
		xml.addAttr  atrName atrValue
	)
	
	-- Parse targets
	for i = 1 to all_targets.count do (
		--Debug
		--messagebox(all_targets[i].name)
		
		-- Atr Name
		atrName = ("lpt" + (i) as string)
		
		--Get Point
		wPoint = all_targets[i].pos
		
		--Transform point in atrValue
		tMatrix = matrix3 1
		tMatrix.pos = wPoint
		mcv_tmx = matrixMax2MCV tMatrix
		atrValue = getTransformPosValue mcv_tmx
		
		--Parse Attribute wpt
		xml.addAttr  atrName atrValue
	)	
)