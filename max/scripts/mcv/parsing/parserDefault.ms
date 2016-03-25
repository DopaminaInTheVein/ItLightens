-- Parse Object of type --
-- -- Default -- --
fn _parseDefault = (
	-- Entity
	local obj = _parsingObject
	local xml = undefined
	if ((getUserProp obj "typeHelper") == undefined) then(
		if classof obj == Editable_mesh or classof obj == Editable_Poly then (		
			xml = XMLNode()
			xml.nombre = "entity"
			
			-- Name
			_parseName xml
			
			-- Transform
			_parseTransform xml

			-- Parse Render del objeto
			_parseRenderStatic xml
			
			--Parse Rigid Body
			_parseRigidBody xml
		)
	)
	return xml
)