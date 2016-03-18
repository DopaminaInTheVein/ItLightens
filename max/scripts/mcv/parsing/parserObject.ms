-- Parse an Object from userprops to XML
_parsingObject = undefined
fn _parseObject obj = (
	--_parsingObject = obj
	--format "_parseObject, obj = %\n" obj
	--local typeObject = getUserProp obj "typeObject"
	--local parserFunction = "_parse" + typeObject
	--return execute(parserFunction + "()")
)

fn _val val = (
	return "\"" + (val as string) + "\""
)

fn _parseTransform = (
	local obj = _parsingObject
	format "_parseTransform, obj = %\n" obj
	local mcv_tmx = matrixMax2MCV obj.transform -- matrix3
	local t = mcv_tmx.pos
	local q = mcv_tmx.rotationPart as quat
	local s = mcv_tmx.scalePart

	if s.x < 0 or s.y < 0 or s.z < 0 then throw ("Object " + obj.name + " has scale transform with negative values. Please apply a ResetTransform, fix the culling and retry")
	--if s.x != s.y or s.y != s.z then throw ("Object " + obj.name + " has non-uniform scales. Please fix it and try again " + (s as string) )
	-- We change the sign of the quaternion because max conversion from matrix to quaternion
	-- rotates in the opposite direction from DX libs
	
	xmlTransform = XMLNode()
	xmlTransform.nombre = "TCompTransform"
	
	--Position
	local ss = stringstream ""
	format "% % %" t.x t.y t.z to:ss
	posVal = ss as string
	xmlTransform.addAttr "pos" posVal
	
	--Quaternion (Orientation)
	ss = stringstream ""
	format "% % % %" -q.x -q.y -q.z q.w to:ss
	quatVal = ss as string
	xmlTransform.addAttr "quat" quatVal
	
	-- Scale
	scaleVal = s.x as string
	xmlTransform.addAttr "scale" scaleVal
	
	return xmlTransform.parse()--ss as string
)