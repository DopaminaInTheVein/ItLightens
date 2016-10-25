-- P A R S E   G U I --
--***********************

--- Aux ---
fn parseMenuName xml = (
	local menu_name = getUserProp _parsingObject "menu_name"
	if menu_name == undefined then(
		menu_name = (filterstring maxFileName ".")[1]
	)
	xml.addAttr "menu_name" menu_name
)
--------------------------------------------------------------

fn _parse_gui = (
	local xml = XML_PARSING.addChild "gui"
	parseMenuName xml
	parsePropAttribute xml "row" "row"
	parsePropAttribute xml "col" "col"
	
	local size = _parsingObject.max - _parsingObject.min
	local gui_helper = findHelper "gui_helper"
	if gui_helper != undefined then (
		size = gui_helper.max - gui_helper.min
	)
	xml.addAttr "width" size.x
	xml.addAttr "height" size.z
	parsePropAttribute xml "lang" "lang"
)

-- fn _parse_gui_button = (
-- 	local xml = XML_PARSING.addChild "gui_button"
-- 	
-- 	--Si se lee del TCompGui se puede quitar de aqui!
-- 	local size = _parsingObject.max - _parsingObject.min
-- 	xml.addAttr "width" size.x
-- 	xml.addAttr "height" size.z
-- )

fn _parse_gui_cursor = (
	local xml = XML_PARSING.addChild "gui_cursor"
	parseMenuName xml
)
