-- Parse Object of type --
-- -- CAMERA -- --
fn _parseCamController = (
	-- Entity
	local obj = _parsingObject
	local xml = XMLNode()
	xml.nombre = "entity"
	
	-- Name
	_parseName xml
	
	--Tag
	_parseTags xml  TAG_CAMERA
	
	-- Transform
	_parseTransform xml
	
	-- Camera
	xmlCamera = xml.addChild "camera"
	xmlCamera.addAttr "znear" _parsingObject.nearrange
	xmlCamera.addAttr "zfar" _parsingObject.farrange
	xmlCamera.addAttr "fov" _parsinObject.fov
	
	-- Controller 3rdPerson
	xmlController = xml.addChild "controller_3rd_person"
	
	return xml
)