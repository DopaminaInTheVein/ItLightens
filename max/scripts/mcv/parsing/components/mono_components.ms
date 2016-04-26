-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn parseMonoComponent = (
	--Name, Transform
	_parse_name()
	_parse_transform()
	if (classof _parsingObject) == OmniLight then (
		_parse_lightpoint()
		return true
	)
		
	return false
)