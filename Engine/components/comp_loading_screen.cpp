#include "mcv_platform.h"
#include "app_modules/imgui/module_imgui.h"
#include "comp_loading_screen.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "imgui/imgui_internal.h"
#include "resources/resources_manager.h"
#include "render/render.h"
#include "render/DDSTextureLoader.h"

#include "render/shader_cte.h"
#include "constants/ctes_object.h"
#include "constants/ctes_camera.h"
#include "constants/ctes_globals.h"
#include "comp_camera_main.h"
#include "app_modules/gui/comps/gui_basic.h"

#include "app_modules/gui/gui.h"
#include <math.h>

using namespace std;

bool TCompLoadingScreen::load(MKeyValue& atts)
{
	string name = atts["name"];
	return true;
}

void TCompLoadingScreen::onCreate() {
}

void TCompLoadingScreen::updateHandle(CHandle &h, std::string tag) {
	if (!h.isValid()) h = tags_manager.getFirstHavingTag(tag);
}
void TCompLoadingScreen::update(float dt) {
	updateHandle(h_loading_bar, "loading_bar");
	updateHandle(h_pag1, "loading_pag1");
	updateHandle(h_pag2, "loading_pag2");
	if (h_loading_bar.isValid()) {
		GET_COMP(gui_loading_bar, h_loading_bar, TCompGui);
		gui_loading_bar->setRenderState(loading_value / 100.f);
	}

	tooglePages();

	// update loading_value;
	loading_value = GameController->GetLoadingState();
	if (loading_value >= 100.f) {
		GameController->LoadComplete(true);
	}
}

void TCompLoadingScreen::tooglePages()
{
	if (time_page < max_time_page) {
		time_page += getDeltaTime(true);
	}
	else {
		swapPages();
		time_page = 0;
	}
}

void TCompLoadingScreen::swapPages()
{
	GET_COMP(tmx1, h_pag1, TCompTransform);
	GET_COMP(tmx2, h_pag2, TCompTransform);
	if (tmx1 && tmx2) {
		VEC3 pos1 = tmx1->getPosition();
		tmx1->setPosition(tmx2->getPosition());
		tmx2->setPosition(pos1);
	}
}

void TCompLoadingScreen::renderInMenu() {
	IMGUI_SHOW_INT(loading_value);
}
