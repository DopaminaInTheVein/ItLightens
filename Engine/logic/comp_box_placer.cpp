#include "mcv_platform.h"
#include "comp_box_placer.h"
#include "comp_box.h"

#include "components/entity.h"
#include "components/comp_transform.h"

#include "app_modules/logic_manager/logic_manager.h"

void TCompBoxPlacer::update(float elapsed) {
	if (box_placed) return; // No interesa comprobar si quitamos caja del sitio por ahora
	bool box_placed_now = false;
	GET_MY(tmx, TCompTransform);
	VEC3 myPos = tmx->getPosition();
	for (auto h : TCompBox::all_boxes) {
		GET_COMP(tbox, h, TCompTransform);
		box_placed_now = inSquaredRangeXZ_Y(myPos, tbox->getPosition(), radius, 16.f);
		Debug->DrawLine(myPos, tbox->getPosition());
	}
	if (box_placed_now) {
		logic_manager->throwUserEvent(lua_event + "();");
	}
}

bool TCompBoxPlacer::load(MKeyValue& atts) {
	lua_event = atts.getString("event", "");
	radius = atts.getFloat("radius", 2.f);
	radius *= radius;
	return true;
}

bool TCompBoxPlacer::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("event", lua_event);
	return true;
}