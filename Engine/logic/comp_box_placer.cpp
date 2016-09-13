#include "mcv_platform.h"
#include "comp_box_placer.h"
#include "comp_box.h"

#include "components/entity.h"
#include "components/comp_transform.h"

#include "app_modules/logic_manager/logic_manager.h"

bool TCompBoxPlacer::load(MKeyValue& atts) {
	lua_event_in = atts.getString("event_in", "");
	lua_event_out = atts.getString("event_out", "");
	radius = atts.getFloat("radius", 2.f);
	radius *= radius;
	return true;
}

bool TCompBoxPlacer::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("event_in", lua_event_in);
	atts.put("event_out", lua_event_out);
	atts.put("radius", sqrt(radius));
	return true;
}

void TCompBoxPlacer::init()
{
	box_placed = checkBox();
}

void TCompBoxPlacer::update(float elapsed) {
	bool box_placed_now = checkBox();
	if (box_placed != box_placed_now) {
		box_placed = box_placed_now;
		std::string lua = box_placed ? lua_event_in : lua_event_out;
		if (lua != "") logic_manager->throwUserEvent(lua + "();");
	}
}

bool TCompBoxPlacer::checkBox()
{
	bool res = false;
	GET_MY(tmx, TCompTransform);
	VEC3 myPos = tmx->getPosition();
	for (auto h : TCompBox::all_boxes) {
		GET_COMP(tbox, h, TCompTransform);
		res = inSquaredRangeXZ_Y(myPos, tbox->getPosition(), radius, 3.f);
		Debug->DrawLine(myPos, tbox->getPosition());
		if (res) break;
	}
	return res;
}