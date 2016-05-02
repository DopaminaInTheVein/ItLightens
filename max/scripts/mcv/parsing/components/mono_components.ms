-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn parseMonoComponent = (
	local parsed = false
	if (classof _parsingObject) == OmniLight then (
		_parse_lightpoint()
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