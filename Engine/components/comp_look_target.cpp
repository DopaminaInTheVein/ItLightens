#include "mcv_platform.h"
#include "comp_look_target.h"

#include "app_modules/logic_manager/logic_manager.h"

#include "entity.h"

#include "comp_culling.h"
#include "comp_aabb.h"

bool TCompLookTarget::load(MKeyValue& atts)
{
	visible = false;
	lua_in = atts.getString("event_lua_in", "");
	sense_vision_needed = atts.getBool("sense_vision", false);
	time_elapsed = 0.f;
	time_wait = atts.getFloat("time_visible", 1.f);
	return true;
}

bool TCompLookTarget::getUpdateInfo()
{
	if (!culling.isValid()) {
		CHandle camera_main = tags_manager.getFirstHavingTag("camera_main");
		if (!camera_main.isValid()) return false;
		culling = GETH_COMP(camera_main, TCompCulling);
		if (!culling.isValid()) return false;
	}

	aabb = GETH_MY(TCompAbsAABB);
	if (!aabb) return false;

	return true;
}

void TCompLookTarget::update(float dt)
{
	//Remeber if was visible
	bool visible_prev = visible;

	// Check if is visible now
	if (sense_vision_needed && !GameController->isSenseVisionEnabled()) {
		visible = false;
	}
	else {
		visible = TCompCulling::checkAABB(culling, aabb);
	}

	if (visible ^ visible_prev) {
		// Visible changed
		time_elapsed = 0.f;
	}
	else {
		//Visible remains the same
		if (time_elapsed >= 0.f) time_elapsed += dt;
		if (time_elapsed > time_wait) {
			time_elapsed = -1.f; // time = -1 --> stop timer
			if (visible) {
				if (lua_in != "") logic_manager->throwUserEvent(lua_in + "()", "", MY_OWNER);
			}
			else {
				if (lua_out != "") logic_manager->throwUserEvent(lua_out + "()", "", MY_OWNER);
			}
		}
	}
}
void TCompLookTarget::renderInMenu() {
	IMGUI_SHOW_BOOL(visible);
}