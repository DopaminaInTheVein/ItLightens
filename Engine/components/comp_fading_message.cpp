#include "mcv_platform.h"
#include "comp_fading_message.h"
//#include "comp_tags.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"

#define FONT_JSON "./data/json/font.json"

void TCompFadingMessage::forceTTLZero() {
	ttl = -0.1f;
}

float TCompFadingMessage::letterSpacing[256] = { 0.f };
bool TCompFadingMessage::init_configuration = false;
void TCompFadingMessage::initSpaceLetters()
{
	auto general = readIniAtrData(FONT_JSON, "general");
	auto space_values = readIniAtrData(FONT_JSON, "space_right");
	float size = general["size"];
	float default_space = 1.f - space_values["default"] / size;
	for (int i = 0; i < 256; i++) letterSpacing[i] = default_space;
	for (auto entry : space_values) {
		unsigned char letter_char = entry.first.at(0);
		letterSpacing[letter_char] = 1.f - entry.second / size;
	}
}

bool TCompFadingMessage::load(MKeyValue& atts)
{
	CHandle thisHan = CHandle(this).getOwner();

	getHandleManager<TCompFadingMessage>()->each([](TCompFadingMessage * mess) {
		mess->forceTTLZero();
	}
	);

	text = atts.getString("text", "defaultText");
	ttl = timeForLetter * text.length() + 4.0f;
	numchars = 0;
	shown_chars = 0;
	id = std::rand();
	std::string endline = "\n";
	int ini = 0;
	//int line
	size_t pos = text.find(endline, 0);
	while (pos != text.npos)
	{
		lineText.push_back(text.substr(ini, pos - ini));
		ini = pos + 1;
		pos = text.find(endline, ini);
	}
	lineText.push_back(text.substr(ini, pos - ini));

	gui_letters.push_back(Gui->addGuiElement("ui/Fading_Background", VEC3(0.5f, 0.02f, 0.40f), "Fading_Message_Background_" + std::to_string(id)));
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (player.hasTag("raijin")) {
		gui_letters.push_back(Gui->addGuiElement("ui/Fading_Icon_RAI", VEC3(0.12f, 0.09f, 0.49f), "Fading_Message_Icon_" + std::to_string(id)));
	}
	else if (player.hasTag("AI_mole")) {
		gui_letters.push_back(Gui->addGuiElement("ui/Fading_Icon_MOL", VEC3(0.12f, 0.09f, 0.49f), "Fading_Message_Icon_" + std::to_string(id)));
	}
	else if (player.hasTag("AI_cientifico")) {
		gui_letters.push_back(Gui->addGuiElement("ui/Fading_Icon_SCI", VEC3(0.12f, 0.09f, 0.49f), "Fading_Message_Icon_" + std::to_string(id)));
	}
	accumSpacing.resize(lineText.size(), 0.0f);

	if (!init_configuration) {
		init_configuration = true;
		initSpaceLetters();
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

	if (ttl < 0.0f) {
		for (CHandle h_letter : gui_letters) {
			h_letter.destroy();
		}
		CHandle h = CHandle(this).getOwner();
		h.destroy();
	}
	else {
		printLetters();
		ttl -= dt;
	}

	/*
		Gui->removeGuiElementByTag("Fading_Message_Icon_" + std::to_string(id));
		for (int i = 0; i < text.length(); i++) {
			Gui->removeGuiElementByTag(("Fading_Message_Letter_" + std::to_string(id) + "_" + std::to_string(i)));
		}
		Gui->removeGuiElementByTag("Fading_Message_Background_" + std::to_string(id));
	*/
}
void TCompFadingMessage::printLetters() {
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
			linechars += lineText[line].length() + 1;
		}

		unsigned char letter = text[i];
		int ascii_tex_pos = letter;
		int ascii_tex_posx = ascii_tex_pos % 16;
		int ascii_tex_posy = ascii_tex_pos / 16;

		float texture_pos_x = ((float)ascii_tex_posx) * letterBoxSize;
		float texture_pos_y = ((float)ascii_tex_posy) * letterBoxSize;
		float sx = letterBoxSize;
		float sy = letterBoxSize;
		float letter_posx = 0.16f + (i - linechars_prev - fminf(line, 1.0f) - accumSpacing[line])*letterSpacer;
		float letter_posy = 0.20f - line*letterSpacerHigh;

		CHandle letter_h = Gui->addGuiElement("ui/Fading_Letter", VEC3(letter_posx, letter_posy, 0.49f + i*0.001f), ("Fading_Message_Letter_" + std::to_string(id) + "_" + std::to_string(i)), scale);
		CEntity * letter_e = letter_h;
		TCompGui * letter_gui = letter_e->get<TCompGui>();
		assert(letter_gui);
		RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
		letter_gui->setTxCoords(textCords);
		accumSpacing[line] += letterSpacing[ascii_tex_pos]; //SBB::readLetterSpacingVector()[ascii_tex_pos];
		gui_letters.push_back(letter_h);
	}
}