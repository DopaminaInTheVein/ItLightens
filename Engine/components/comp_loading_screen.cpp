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
	VEC3 position;
	switch (loading_value) {
	case 5:
		text = "a";
		position.x = 0.15;
		position.y = 0.15;
		position.z = 0.75;
		break;
	case 20:
		text = "b";
		position.x = 0.25;
		position.y = 0.25;
		position.z = 0.75;
		break;
	case 45:
		text = "c";
		position.x = 0.35;
		position.y = 0.35;
		position.z = 0.75;
		break;
	case 60:
		text = "d";
		position.x = 0.45;
		position.y = 0.45;
		position.z = 0.75;
		break;
	case 80:
		text = "e";
		position.x = 0.55;
		position.y = 0.55;
		position.z = 0.75;
		break;
	case 100:
		text = "f";
		position.x = 0.65;
		position.y = 0.65;
		position.z = 0.75;
		break;
	default:
		text = "g";
	}
	position.x += 0.4f;
	//text = to_string(loading_value);

	if (loading_value == 100) {
		for (int i = 0; i < text.length(); i++) {
			Gui->removeGuiElementByTag(("Loading_" + std::to_string(id) + "_" + std::to_string(i)));
		}
		GameController->SetGameState(CGameController::RUNNING);
		GameController->SetLoadingState(101);
		// Delete de la barra y la imagen de fondo

	}
	else {
		printLetters(position);
	}
	text.clear();
}

void TCompLoadingScreen::render() const {
	PROFILE_FUNCTION("TCompLoadingScreen render");
	
	bool b = false;
	
	ImGui::Begin("Game GUI", &b, ImVec2(resolution_x, resolution_y), 0.0f, flags);
	ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));
	
	GUI::drawText(0.45, 0.65, GImGui->Font, 20, obtainColorFromString("#FFFFFFFF"), "HOOOOOOOOOOLAAAAAA");
	
	ImGui::End();
		
}

void TCompLoadingScreen::printLetters(VEC3 posi) const {
	//PROFILE_FUNCTION("TCompFadingMessage printLetters");

	bool b = false;

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
		CHandle letter_h = Gui->addGuiElement("ui/Fading_Letter", posi, ("Loading_" + std::to_string(id) + "_" + std::to_string(i)));
		CEntity * letter_e = letter_h;
		TCompGui * letter_gui = letter_e->get<TCompGui>();
		assert(letter_gui);
		RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
		letter_gui->setTxCoords(textCords);
	}
}

void TCompLoadingScreen::renderInMenu() {
	IMGUI_SHOW_INT(loading_value);
}