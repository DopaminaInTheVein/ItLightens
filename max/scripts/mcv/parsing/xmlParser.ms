--XMLNode & Functions
struct XMLAttr (
	nombre = "",
	valor = ""
)

struct XMLNode (
	nombre = "",
	attrs = #(),
	elems = #(),
	
	fn addAttr nombre valor= (
		local atr = XMLAttr()
		atr.nombre = nombre
		atr.valor = valor
		append attrs atr
	),
	
	fn addChild nombreNodo = (
		local nodo = XMLNode()
		nodo.nombre = nombreNodo
		append elems nodo
		return nodo
	),
	
	fn addElement nodo = (
		append elems nodo
	),
	
	fn parse = (
		print nombre
		res = "<"
		res += nombre
		
		--Atributos ?
		if attrs.count > 0 then (
			-- Tiene más atributos
			for atr in attrs do (
				res += "\n" + "   " + atr.nombre + "=" + (_val atr.valor)
			)
		)
		
		-- Tiene más elementos
		if elems.count > 0 then (
			--Tag de abrir
			res += ">\n"
			
			-- Parseamos elementos
			for elem in elems do (
				res += elem.parse()
			)
			--Tag cierre
			res += "\n</"
			res += nombre
			res += ">\n"
		) else (
			res += "/>\n"
		)
		
		return res
	)
)

/* xml = XMLNode()
xml.nombre = "entities"
ent1 = xml.addChild "entity"
ent2 = xml.addChild "entity"
ent1Name = ent1.addChild "TCompName"
ent1Name.addAttr "name" "pepito"
ent1Name.addAttr "lastName" "grillo"
ent2Name = ent2.addChild "TCompTransform"
ent2Name.addAttr "pos" "0 0 0"
ent2Name.addAttr "quad" "1 0 0 0"
xml.parse() */





