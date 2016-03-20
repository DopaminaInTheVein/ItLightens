-- Parse Object of type --
-- -- CAMERA -- --
fn _parseCamera = (
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
	xmlCamera.addAttr "znear" "1"
	xmlCamera.addAttr "zfar" "250"
	xmlCamera.addAttr "fov" "60"
	
	-- Controller 3rdPerson
	xmlController = xml.addChild "controller_3rd_person"
	
	return xml
)