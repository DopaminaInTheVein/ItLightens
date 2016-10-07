#include "mcv_platform.h"
#include "comp_fading_message.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"

#define FONT_JSON "./data/json/font.json"

void moveElement(CHandle h, VEC3 pos)
{
	if (h.isValid()) {
		GET_COMP(ht, h, TCompTransform);
		if (ht) ht->setPosition(pos);
	}
}

void TCompFadingMessage::Init() {
	gui_back = Gui->addGuiElement("ui/Fading_Background", VEC3(0.5f, -1.0f, 0.3f), "Fading_Message_Background");
	gui_rai = Gui->addGuiElement("ui/Fading_Icon_RAI", VEC3(0.12f, -1.0f, 0.35f), "Fading_Message_Icon_RAI");
	gui_mol = Gui->addGuiElement("ui/Fading_Icon_MOL", VEC3(0.12f, -1.0f, 0.35f), "Fading_Message_Icon_MOL");
	gui_sci = Gui->addGuiElement("ui/Fading_Icon_SCI", VEC3(0.12f, -1.0f, 0.35f), "Fading_Message_Icon_SCI");

	// Size = 4 lines * 50 chars per line
	gui_letters.resize(200);
	for (int i = 0; i < 200; ++i) {
		gui_letters[i] = Gui->addGuiElement("ui/Fading_Letter", VEC3(0.0f, -1.0, 0.31f), ("Fading_Message_Letter_" + std::to_string(i)), scale);
	}
	initialized = true;
}

void TCompFadingMessage::hideAll() {
	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	if (!h_ui_cam.isValid()) return;
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	if (!ui_cam) return;
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();

	VEC3 new_pos1 = min_ortho + (max_ortho - min_ortho) * VEC3(0.5f, -1.0f, 0.3f);
	VEC3 new_pos2 = min_ortho + (max_ortho - min_ortho) * VEC3(0.12f, -1.0f, 0.35f);
	VEC3 new_pos3 = min_ortho + (max_ortho - min_ortho) * VEC3(0.0f, -1.0f, 0.0f);

	moveElement(gui_back, new_pos1);
	moveElement(gui_rai, new_pos2);
	moveElement(gui_mol, new_pos2);
	moveElement(gui_sci, new_pos2);
	for (CHandle letter : gui_letters) {
		moveElement(letter, new_pos3);
	}
	RenderManager.ModifyUI();
	enabled = false;
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
	if (!initialized) {
		Init();
	}
	if (enabled) {
		hideAll();
	}

	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	if (!h_ui_cam.isValid()) return false;
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	if (!ui_cam) return false;
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();

	VEC3 new_pos1 = min_ortho + (max_ortho - min_ortho) * VEC3(0.12f, 0.09f, 0.35f);

	text = atts.getString("text", "defaultText");
	ttl = timeForLetter * text.length() + 4.0f;
	numchars = 0;
	shown_chars = 0;
	lineText.resize(0);
	accumSpacing.resize(0);
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

	moveElement(gui_back, min_ortho + (max_ortho - min_ortho) * VEC3(0.5f, 0.02f, 0.3f));

	CHandle player = CPlayerBase::handle_player;
	if (player.isValid()) {
		if (player.hasTag("raijin")) {
			moveElement(gui_rai, new_pos1);
		}
		else if (player.hasTag("AI_mole")) {
			moveElement(gui_mol, new_pos1);
		}
		else if (player.hasTag("AI_cientifico")) {
			moveElement(gui_sci, new_pos1);
		}
	}
	accumSpacing.resize(lineText.size(), 0.0f);

	if (!init_configuration) {
		init_configuration = true;
		initSpaceLetters();
	}
	enabled = true;
	shown_chars = 0;
	numchars = 0;
	accumTime = 0.0f;
	return true;
}

void TCompFadingMessage::update(float dt) {
	if (!enabled) return;

	shown_chars = numchars;

	accumTime += dt;
	while (accumTime > timeForLetter) {
		if (numchars < text.length()) {
			++numchars;
		}
		accumTime -= timeForLetter;
	}
	bool modify_ui = shown_chars != numchars;

	if (ttl < 0.0f) {
		hideAll();
	}
	else {
		printLetters();
		ttl -= dt;
	}
	if (modify_ui) RenderManager.ModifyUI();
}

void TCompFadingMessage::printLetters() {
	bool b = false;
	int gState = GameController->GetGameState();
	if (gState != CGameController::RUNNING) return;

	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	if (!h_ui_cam.isValid()) return;
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	if (!ui_cam) return;
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();

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

		float letter_posx = 0.16f + (i - linechars_prev - fminf(line, 1.0f) - accumSpacing[line])*letterSpacer;
		float letter_posy = 0.20f - line*letterSpacerHigh;

		CHandle letter_h = gui_letters[50 * line + i - linechars_prev];
		moveElement(letter_h, min_ortho + (max_ortho - min_ortho) * VEC3(letter_posx, letter_posy, 0.31f + i*0.001f));
		if (letter_h.isValid()) {
			GET_COMP(letter_gui, letter_h, TCompGui);
			if (letter_gui) {
				unsigned char letter = text[i];
				letter_gui->setTxLetter(text[i]);
				accumSpacing[line] += letterSpacing[letter];
			}
		}
	}
}