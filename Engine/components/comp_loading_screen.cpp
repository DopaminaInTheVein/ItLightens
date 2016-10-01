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

	return true;
}

void TCompLoadingScreen::onCreate(const TMsgEntityCreated&) {
	//text = "";
	//for (int i = 0; i < 100; i++) {
	//	text += "g";
	//}
	//printLetters();
}

void TCompLoadingScreen::update(float dt) {
	// update loading_value;
	loading_value = GameController->GetLoadingState();
	//numchars = loading_value;

	if (loading_value >= 100.f) {
		GameController->LoadComplete(true);
		// Delete de la barra y la imagen de fondo
	//	for (int i = 0; i < numchars; ++i) {
	//		//position.x = 0.4f + i*0.0085f;
	//		Gui->removeAllGuiElementsByTag("loading" + to_string(i));
	//	}
	//	Gui->removeAllGuiElementsByTag("loading");
	//	updateLetters(false);
	//	//CHandle(this).destroy();
	}
	//else {
	//	updateLetters(true);
	//}
}

void TCompLoadingScreen::render() const {
	//do nothing
}

//void TCompLoadingScreen::printLetters() const {
//	PROFILE_FUNCTION("TCompFadingMessage printLetters");
//
//	bool b = false;
//	VEC3 position;
//	position.y = -0.25f;
//	position.z = 0.75f;
//
//	for (int i = 0; i < 100; ++i) {
//		if ((i < text.length() - 1 && text[i] == '\\' && text[i + 1] == 'n') || (i > 1 && text[i - 1] == '\\' && text[i] == 'n')) {
//			continue;
//		}
//		int line = 0;
//		int linechars_prev = 0;
//
//		char letter = text[i];
//		int ascii_tex_pos = letter;
//		int ascii_tex_posx = ascii_tex_pos % 16;
//		int ascii_tex_posy = ascii_tex_pos / 16;
//
//		float texture_pos_x = ((float)ascii_tex_posx) / 16.0f;
//		float texture_pos_y = ((float)ascii_tex_posy) / 16.0f;
//		float sx = letterBoxSize / 16.0f;
//		float sy = letterBoxSize / 16.0f;
//
//		position.x = 0.4f + i*0.0085f;
//
//		CHandle letter_h = Gui->addGuiElement("ui/Loading_bar", position, "loading" + to_string(i));
//		CEntity * letter_e = letter_h;
//		TCompGui * letter_gui = letter_e->get<TCompGui>();
//		assert(letter_gui);
//		RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
//		letter_gui->setTxCoords(textCords);
//	}
//}

//void TCompLoadingScreen::updateLetters(bool loaded) const {
//	PROFILE_FUNCTION("TCompFadingMessage printLetters");
//	VEC3 position;
//	if (loaded)
//		position.y = 0.10f;
//	else
//		position.y = -0.25f;
//	position.z = 0.75f;
//
//	for (int i = 0; i < numchars; ++i) {
//		position.x = 0.4f + i*0.0085f;
//		Gui->updateGuiElementPositionByTag("loading" + to_string(i), position);
//	}
//}

void TCompLoadingScreen::renderInMenu() {
	IMGUI_SHOW_INT(loading_value);
}