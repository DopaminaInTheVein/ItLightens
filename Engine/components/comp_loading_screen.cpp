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

#include <math.h>

extern CShaderCte< TCteObject > shader_ctes_object;
extern CShaderCte< TCteCamera > shader_ctes_camera;

bool TCompLoadingScreen::load(MKeyValue& atts)
{
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();

	string name = atts["name"];

	Gui->addGuiElement("ui/loading", VEC3(0.f, 0.f, 0.75f));

	return true;
}

void TCompLoadingScreen::update(float dt) {
	// update loading_value;
	loading_value = GameController->GetLoadingState();
	if (loading_value < 100) {
		GameController->SetGameState(CGameController::LOADING);
	}
	else {
		GameController->SetGameState(CGameController::RUNNING);
		Gui->removeGuiElementByTag("loading");
	}
}

void TCompLoadingScreen::render() const {
#ifndef NDEBUG
	PROFILE_FUNCTION("TCompLoadingScreen render");

	bool b = false;

	ImGui::Begin("Game GUI", &b, ImVec2(resolution_x, resolution_y), 0.0f, flags);
	ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));

	GUI::drawText(0.45, 0.65, GImGui->Font, 20, obtainColorFromString("#FFFFFFFF"), to_string(loading_value).c_str());

	/*Rect rect = GUI::createRect(screen_x, screen_y, 0.1f, 0.1f);
	GUI::drawRect(rect, obtainColorFromString("#00FF00FF"));*/

	ImGui::End();
#endif
}