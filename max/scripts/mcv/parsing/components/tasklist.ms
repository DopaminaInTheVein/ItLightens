-- -- MONO COMPONENTS -- --
--(Objetos que se interpretan como un solo componente, por su tipo de max) --

fn _parse_tasklist = (
	if ( (classof _parsingObject) == Editable_mesh or (classof _parsingObject) == Editable_Poly ) then (
		
		local xml1 = XML_PARSING.addChild "render_static_mesh"
		xml1.addAttr "name" "static_meshes/test/baliza.static_mesh"		
		local xml2 = XML_PARSING.addChild "room"
		xml2.addAttr "name" "-1"		

		
		local xml = XML_PARSING.addChild "tasklist"

		xml.addAttr "points_size" ((_parsingObject.children.count+1) as string)
		
		local names_array = for nameaux in _parsingObject.children collect nameaux.name
		
		sort names_array
		
		local mcv_tmx
		local wPoint
		local atrName		= "pos0"
		local atrName2	= "room0"
		local atrValue
		
		--Get Point
		wPoint = _parsingObject.pos
		
		--Transform point in atrValue
		tMatrix = matrix3 1
		tMatrix.pos = wPoint
		mcv_tmx = matrixMax2MCV tMatrix
		atrValue = getTransformPosValue mcv_tmx
		
		xml.addAttr  atrName	atrValue
		xml.addAttr  atrName2	_parsingObject.room_name

		
		for i = 1 to _parsingObject.children.count do (		
			local task = getNodeByName names_array[i]

			-- Atr Name
			atrName = ("pos" + (i) as string)
			atrName2 = ("room" + (i) as string)
			
			--Get Point
			wPoint = task.pos
			
			--Transform point in atrValue
			tMatrix = matrix3 1
			tMatrix.pos = wPoint
			mcv_tmx = matrixMax2MCV tMatrix
			atrValue = getTransformPosValue mcv_tmx
			
			--Parse Attribute wpt
			xml.addAttr  atrName	atrValue
			xml.addAttr  atrName2	task.room_name
		)
		
		) else throw ("Error: not an editable mesh or poly for making tasklist")
)