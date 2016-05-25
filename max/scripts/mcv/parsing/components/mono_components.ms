-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn parseMonoComponent = (
	local parsed = false
	if (classof _parsingObject) == OmniLight then (
		_parse_lightpoint()
		parsed = true
	)
	
	if (classof _parsingObject) == freeSpot then (
		_parse_lightDir()
		parsed = true
	)
	
	if ( (getUserProp _parsingObject "typeObject") as string) == "railCamera"  then
	(
		_parse_rail_camera()
		parsed = true
	)
	
	if ( (getUserProp _parsingObject "typeObject") as string) == "helperArrow"  then
	(
		_parse_helper_arrow()
		parsed = true
	)
	
	-- If parsed we add common components
	if parsed then (
		--Name, Transform
		_parse_name()
		_parse_transform()
	)
	return parsed
)