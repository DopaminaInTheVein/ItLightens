#include "mcv_platform.h"
#include "comp_fading_message.h"
//#include "comp_tags.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"

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
	ttl = timeForLetter * text.length() + 2.0f;
	numchars = 0;
	shown_chars = 0;
	id = std::rand();
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

	Gui->addGuiElement("ui/Fading_Background", VEC3(0.5f, 0.02f, 0.40f), "Fading_Message_Background_" + std::to_string(id));
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (player.hasTag("raijin")) {
		Gui->addGuiElement("ui/Fading_Icon_RAI", VEC3(0.12f, 0.09f, 0.49f), "Fading_Message_Icon_RAI_" + std::to_string(id));
	}
	else if (player.hasTag("AI_mole")) {
		Gui->addGuiElement("ui/Fading_Icon_MOL", VEC3(0.12f, 0.09f, 0.49f), "Fading_Message_Icon_MOL_" + std::to_string(id));
	}
	else if (player.hasTag("AI_cientifico")) {
		Gui->addGuiElement("ui/Fading_Icon_SCI", VEC3(0.12f, 0.09f, 0.49f), "Fading_Message_Icon_SCI_" + std::to_string(id));
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
		printLetters();
		ttl -= dt;
	}
	else {
		CHandle h = CHandle(this).getOwner();
		h.destroy();

		CHandle player = tags_manager.getFirstHavingTag(getID("player"));
		if (player.hasTag("raijin")) {
			Gui->removeGuiElementByTag("Fading_Message_Icon_RAI_" + std::to_string(id));
		}
		else if (player.hasTag("AI_mole")) {
			Gui->removeGuiElementByTag("Fading_Message_Icon_MOL_" + std::to_string(id));
		}
		else if (player.hasTag("AI_cientifico")) {
			Gui->removeGuiElementByTag("Fading_Message_Icon_SCI_" + std::to_string(id));
		}
		for (int i = 0; i < text.length(); i++) {
			Gui->removeGuiElementByTag(("Fading_Message_Letter_" + std::to_string(id) + "_" + std::to_string(i)));
		}
		Gui->removeGuiElementByTag("Fading_Message_Background_" + std::to_string(id));
	}
}
void TCompFadingMessage::printLetters() const {
	bool b = false;
	int gState = GameController->GetGameState();
	if (gState != CGameController::RUNNING) return;

	for (int i = shown_chars; i < numchars; ++i) {
		if ((i < text.length() - 1 && text[i] == '\\' && text[i + 1] == 'n') || (i > 1 && text[i - 1] == '\\' && text[i] == 'n')) {
			continue;
		}
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

		float texture_pos_x = ((float)ascii_tex_posx) * letterBoxSize;
		float texture_pos_y = ((float)ascii_tex_posy) * letterBoxSize;
		float sx = letterBoxSize;
		float sy = letterBoxSize;

		float letter_posx = 0.50f + (i - linechars_prev - fminf(line, 1.0f)) * sizeFontX;
		float letter_posy = 0.01f - line*sizeFontY;
		CHandle letter_h = Gui->addGuiElement("ui/Fading_Letter", VEC3(letter_posx, letter_posy, 0.49f), ("Fading_Message_Letter_" + std::to_string(id) + "_" + std::to_string(i)));
		CEntity * letter_e = letter_h;
		TCompGui * letter_gui = letter_e->get<TCompGui>();
		assert(letter_gui);
		RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
		letter_gui->setTxCoords(textCords);
	}
}