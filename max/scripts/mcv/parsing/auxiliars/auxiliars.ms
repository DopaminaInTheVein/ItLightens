-- Parsing Auxiliares
--******************

fn parseable = (
  if (getUserProp _parsingObject "typeHelper") != undefined then (
	return false
  )
  
  return true
)

-- Valores -- (poner entre comillas)
fn _val val = (
	local valTrimmed = trimLeft (trimRight (val as string) )
	return "\"" + (valTrimmed as string) + "\""
)