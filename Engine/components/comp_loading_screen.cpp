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

#include "app_modules/gui/gui.h"
#include <math.h>

using namespace std;

bool TCompLoadingScreen::load(MKeyValue& atts)
{
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();

	string name = atts["name"];

	return true;
}

void TCompLoadingScreen::onCreate() {
}

void TCompLoadingScreen::update(float dt) {
	// update loading_value;
	loading_value = GameController->GetLoadingState();

	if (loading_value >= 100.f) {
		GameController->LoadComplete(true);
	}
}

void TCompLoadingScreen::renderInMenu() {
	IMGUI_SHOW_INT(loading_value);
}