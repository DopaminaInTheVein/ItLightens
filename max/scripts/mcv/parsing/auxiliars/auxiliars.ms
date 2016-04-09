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
	return "\"" + (val as string) + "\""
)