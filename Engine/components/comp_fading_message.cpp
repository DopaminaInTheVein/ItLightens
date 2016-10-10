#include "mcv_platform.h"
#include "comp_fading_message.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"

#define FONT_JSON "./data/json/font.json"

#define NUM_LINES 4
#define NUM_CHARS_LINE 42
#define TOTAL_CHAR (NUM_LINES * NUM_CHARS_LINE)

#define INIT_POS VEC3(0.16f, 0.16f, 0.35f)

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
	cur_line = 0;
	cur_char_line = 0;
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
	}
	else if (enabled) {
		hideAll();
	}

	VEC3 new_pos1 = min_ortho + orthorect * VEC3(0.12f, 0.09f, 0.35f);
	//new_pos1.z = 0.35f;

	auto text_input = atts.getString("text", "defaultText");
	text = Font::getVChar(text_input);
	permanent = atts.getBool("permanent", false);
	std::string who = atts.getString("icon", "default");
	ttl = timeForLetter * text.size() + 4.0f;
	numchars = 0;
	shown_chars = 0;
	accumSpacing = std::vector<float>(NUM_LINES, 0.0f);

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

	cur_line = cur_char_line = 0;
	return true;
}

void TCompFadingMessage::update(float dt) {
	if (!enabled) return;

	shown_chars = numchars;

	accumTime += dt;
	while (accumTime > timeForLetter) {
		if (numchars < text.size()) {
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
	VEC3 init_pos = Gui->getWorldPos(INIT_POS);
	for (int i = shown_chars; i < numchars; ++i) {
		if (text[i].IsNewLine() || cur_char_line >= NUM_CHARS_LINE) {
			cur_line++;
			cur_char_line = 0;
			if (text[i].IsNewLine()) continue;
		}

		VEC3 offset_pos;
		offset_pos.x = (cur_char_line - accumSpacing[cur_line])*scale;
		offset_pos.y = -cur_line*letterSpacerHigh;
		offset_pos.z = i*0.001f;
		VEC3 new_pos_let = init_pos + offset_pos;

		CHandle letter_h = gui_letters[NUM_CHARS_LINE * cur_line + cur_char_line];
		moveElement(letter_h, new_pos_let);
		if (letter_h.isValid()) {
			GET_COMP(letter_gui, letter_h, TCompGui);
			if (letter_gui) {
				letter_gui->setTxCoords(text[i].GetTxtCoords());
				accumSpacing[cur_line] += text[i].GetSize();
			}
		}
		cur_char_line++;
	}
}