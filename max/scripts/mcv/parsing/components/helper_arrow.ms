-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn _parse_helper_arrow = (
	if ( (classof _parsingObject) == Editable_mesh ) then (
		XML_PARSING.addChild "helper_arrow"
		local xml = XML_PARSING.addChild "render_static_mesh"
		xml.addAttr "name" "static_meshes/helpers/helper_arrows.static_mesh"		

		) else throw ("Error: not an editable mesh for making helper arrow ")
)