#include "mcv_platform.h"
#include "comp_fading_message.h"
//#include "comp_tags.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "imgui/imgui_internal.h"
#include "app_modules/imgui/module_imgui.h"
#include "resources/resources_manager.h"
#include "render/render.h"
#include "render/DDSTextureLoader.h"

#include "render/shader_cte.h"
#include "constants/ctes_object.h"
#include "constants/ctes_camera.h"
#include "constants/ctes_globals.h"
#include <math.h>

extern CShaderCte< TCteCamera > shader_ctes_camera;

void TCompFadingMessage::forceTTLZero() {
	ttl = -0.1f;
}

bool TCompFadingMessage::load(MKeyValue& atts)
{
	CHandle thisHan = CHandle(this).getOwner();

	getHandleManager<TCompFadingMessage>()->each([](TCompFadingMessage * mess) {
		mess->forceTTLZero();
	}
	);

	text = atts.getString("text", "defaultText");
	//ttl = atts.getFloat("ttl", 0.1f);
	ttl = timeForLetter * text.length() + 2.0f;
	numchars = 0;
	shown_chars = 0;

	std::string endline = "\n";
	int ini = -1;
	size_t pos = text.find(endline, 0);
	while (pos != text.npos)
	{
		lineText.push_back(text.substr(ini + 1, pos));
		ini = pos;
		pos = text.find(endline, pos + 1);
	}
	lineText.push_back(text.substr(ini + 1, pos));

	Gui->addGuiElement("ui/Fading_Background", VEC3(0.5f, 0.060f, 0.48f), "Fading_Message_Background");
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (player.hasTag("raijin")) {
		Gui->addGuiElement("ui/Fading_Icon_RAI", VEC3(0.11f, 0.06f, 0.50f), "Fading_Message_Icon_RAI");
	}
	else if (player.hasTag("AI_mole")) {
		Gui->addGuiElement("ui/Fading_Icon_MOL", VEC3(0.11f, 0.06f, 0.50f), "Fading_Message_Icon_MOL");
	}
	else if (player.hasTag("AI_cientifico")) {
		Gui->addGuiElement("ui/Fading_Icon_SCI", VEC3(0.11f, 0.06f, 0.50f), "Fading_Message_Icon_SCI");
	}
	return true;
}

void TCompFadingMessage::update(float dt) {
	static float accumTime = 0.0f;
	shown_chars = numchars;

	accumTime += dt;
	while (accumTime > timeForLetter) {
		if (numchars < text.length()) {
			++numchars;
		}
		accumTime -= timeForLetter;
	}

	if (ttl >= 0.0f) {
		ttl -= dt;
	}
	else {
		CHandle h = CHandle(this).getOwner();
		h.destroy();

		CHandle player = tags_manager.getFirstHavingTag(getID("player"));
		if (player.hasTag("raijin")) {
			Gui->removeGuiElementByTag("Fading_Message_Icon_RAI");
		}
		else if (player.hasTag("AI_mole")) {
			Gui->removeGuiElementByTag("Fading_Message_Icon_MOL");
		}
		else if (player.hasTag("AI_cientifico")) {
			Gui->removeGuiElementByTag("Fading_Message_Icon_SCI");
		}
		for (int i = 0; i < text.length(); i++) {
			Gui->removeGuiElementByTag(("Fading_Message_Letter" + std::to_string(i)));
		}
		Gui->removeGuiElementByTag("Fading_Message_Background");
	}
}
void TCompFadingMessage::render() const {
	PROFILE_FUNCTION("TCompFadingMessage render");

	bool b = false;
	int gState = GameController->GetGameState();
	if (gState != CGameController::RUNNING) return;

	for (int i = shown_chars; i < numchars; ++i) {
		int line = 0;
		int linechars = lineText[line].length();
		int linechars_prev = 0;
		while (linechars < shown_chars) {
			++line;
			linechars_prev = linechars;
			linechars += lineText[line].length();
		}

		char letter = text[i];
		int ascii_tex_pos = letter;
		int ascii_tex_posx = ascii_tex_pos % 16;
		int ascii_tex_posy = ascii_tex_pos / 16;

		float texture_pos_x = ((float)ascii_tex_posx) / 16.0f;
		float texture_pos_y = ((float)ascii_tex_posy) / 16.0f;
		float sx = letterBoxSize;
		float sy = letterBoxSize;

		float letter_posx = 0.55f + (i - linechars_prev - 2 * line) * sizeFontX;
		float letter_posy = 0.07f - line*sizeFontY;
		Gui->addGuiElement("ui/Fading_Letter", VEC3(letter_posx, letter_posy, 0.49f), ("Fading_Message_Letter" + std::to_string(i)));
	}
}