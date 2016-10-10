#include "mcv_platform.h"
#include "comp_fading_message.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"

#define FONT_JSON "./data/json/font.json"

void TCompFadingMessage::moveElement(CHandle h, const VEC3 pos)
{
	if (h.isValid()) {
		GET_COMP(ht, h, TCompTransform);
		if (ht) ht->setPosition(pos);
	}
}

void TCompFadingMessage::Init() {
	gui_back = Gui->addGuiElement("ui/Fading_Background", VEC3(0.5f, -1.0f, 0.3f), "");
	gui_rai = Gui->addGuiElement("ui/Fading_Icon_RAI", VEC3(0.12f, -1.0f, 0.35f), "");
	gui_mol = Gui->addGuiElement("ui/Fading_Icon_MOL", VEC3(0.12f, -1.0f, 0.35f), "");
	gui_sci = Gui->addGuiElement("ui/Fading_Icon_SCI", VEC3(0.12f, -1.0f, 0.35f), "");

	// Size = 4 lines * 75 chars per line
	gui_letters.resize(300);
	for (int i = 0; i < 300; ++i) {
		gui_letters[i] = Gui->addGuiElement("ui/Fading_Letter", VEC3(0.0f, -1.0, 0.31f), "", scale);
	}
	initialized = true;

	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	if (!h_ui_cam.isValid()) return;
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	if (!ui_cam) return;
	min_ortho = ui_cam->getMinOrtho();
	max_ortho = ui_cam->getMaxOrtho();
	orthorect = max_ortho - min_ortho;
}

void TCompFadingMessage::hideAll() {
	VEC3 new_pos1 = min_ortho + orthorect * VEC3(0.5f, -1.0f, 0.3f);
	//new_pos1.z = 0.3f;
	VEC3 new_pos2 = min_ortho + orthorect * VEC3(0.12f, -1.0f, 0.35f);
	//new_pos2.z = 0.35f;
	VEC3 new_pos3 = min_ortho + orthorect * VEC3(0.0f, -1.0f, 0.0f);
	//new_pos3.z = 0.0f;

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

bool TCompFadingMessage::load(MKeyValue& atts)
{
	if (!initialized) {
		Init();
		TCompText::initTextConfig();
	}
	else if (enabled) {
		hideAll();
	}

	VEC3 new_pos1 = min_ortho + orthorect * VEC3(0.12f, 0.09f, 0.35f);
	//new_pos1.z = 0.35f;

	text = atts.getString("text", "defaultText");
	permanent = atts.getBool("permanent", false);
	std::string who = atts.getString("icon", "default");
	ttl = timeForLetter * text.length() + 4.0f;
	numchars = 0;
	shown_chars = 0;
	lineText.resize(0);
	//id = std::rand();
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

	accumSpacing = std::vector<float>(lineText.size(), 0.0f);

	VEC3 new_pos2 = min_ortho + orthorect * VEC3(0.5f, 0.02f, 0.3f);
	//new_pos2.z = 0.3f;
	moveElement(gui_back, new_pos2);

	CHandle icon;
	if (who == "raijin") icon = gui_rai;
	else if (who == "mole") icon = gui_mol;
	else if (who == "scientist") icon = gui_sci;
	else if (who == "nobody") icon = CHandle();
	else {
		CHandle player = CPlayerBase::handle_player;
		if (player.isValid()) {
			if (player.hasTag("raijin")) icon = gui_rai;
			if (player.hasTag("AI_mole")) icon = gui_mol;
			else if (player.hasTag("AI_cientifico")) icon = gui_sci;
		}
	}
	if (icon.isValid()) moveElement(icon, new_pos1);

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
		if (!permanent) ttl -= dt;
	}
	if (modify_ui) RenderManager.ModifyUI();
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
		while (linechars < i) {
			++line;
			linechars_prev = linechars;
			linechars += lineText[line].length() + 1;
		}

		float letter_posx = 0.16f + (i - linechars_prev - fminf(line, 1.0f) - accumSpacing[line])*letterSpacer;
		float letter_posy = 0.16f - line*letterSpacerHigh;

		CHandle letter_h = gui_letters[75 * line + i - linechars_prev];
		VEC3 new_pos_let = min_ortho + orthorect * VEC3(letter_posx, letter_posy, 0.35f + i*0.001f);
		moveElement(letter_h, new_pos_let);
		if (letter_h.isValid()) {
			GET_COMP(letter_gui, letter_h, TCompGui);
			if (letter_gui) {
				unsigned char letter = text[i];
				letter_gui->setTxLetter(text[i]);
				accumSpacing[line] += TCompText::getSpaceRight(letter);
			}
		}
	}
}