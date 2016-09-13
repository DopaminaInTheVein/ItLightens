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

bool TCompLoadingScreen::load(MKeyValue& atts)
{
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();
	
	string name = atts["name"];
	
	//Gui->addGuiElement("ui/loading", VEC3(0.f, 0.f, 0.75f));
	
	return true;
}

void TCompLoadingScreen::update(float dt) {
	// update loading_value;
	loading_value = GameController->GetLoadingState();
	text = to_string(loading_value);

	if (loading_value >= 100) {
		for (int i = 0; i < text.length(); i++) {
			Gui->removeGuiElementByTag(("Loading_" + std::to_string(id) + "_" + std::to_string(i)));
		}
		GameController->SetGameState(CGameController::RUNNING);
	}
	else {
		printLetters();
	}
}

void TCompLoadingScreen::render() const {
	PROFILE_FUNCTION("TCompLoadingScreen render");
	
	bool b = false;
	
	ImGui::Begin("Game GUI", &b, ImVec2(resolution_x, resolution_y), 0.0f, flags);
	ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));
	
	GUI::drawText(0.45, 0.65, GImGui->Font, 20, obtainColorFromString("#FFFFFFFF"), "HOOOOOOOOOOLAAAAAA");
	
	ImGui::End();
		
}

void TCompLoadingScreen::printLetters() const {
	//PROFILE_FUNCTION("TCompFadingMessage printLetters");

	bool b = false;
	int gState = GameController->GetGameState();
	if (gState != CGameController::LOADING) return;

	for (int i = 0; i < numchars; ++i) {
		if ((i < text.length() - 1 && text[i] == '\\' && text[i + 1] == 'n') || (i > 1 && text[i - 1] == '\\' && text[i] == 'n')) {
			continue;
		}
		int line = 0;
		int linechars_prev = 0;

		char letter = text[i];
		int ascii_tex_pos = letter;
		int ascii_tex_posx = ascii_tex_pos % 16;
		int ascii_tex_posy = ascii_tex_pos / 16;

		float texture_pos_x = ((float)ascii_tex_posx) / 16.0f;
		float texture_pos_y = ((float)ascii_tex_posy) / 16.0f;
		float sx = letterBoxSize / 16.0f;
		float sy = letterBoxSize / 16.0f;

		float letter_posx = 0.50f + (i - linechars_prev - fminf(line, 1.0f)) * sizeFontX;
		float letter_posy = 0.50f - line*sizeFontY;
		CHandle letter_h = Gui->addGuiElement("ui/Fading_Letter", VEC3(letter_posx, letter_posy, 0.49f), ("Loading_" + std::to_string(id) + "_" + std::to_string(i)));
		CEntity * letter_e = letter_h;
		TCompGui * letter_gui = letter_e->get<TCompGui>();
		assert(letter_gui);
		RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
		letter_gui->setTxCoords(textCords);
	}
}