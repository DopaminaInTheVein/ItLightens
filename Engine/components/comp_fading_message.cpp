#include "mcv_platform.h"
#include "entity.h"

#include "comp_fading_message.h"
#include "comp_transform.h"
#include "comp_camera.h"
#include "player_controllers/player_controller_base.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/gui.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"
#include "app_modules/lang_manager/lang_manager.h"

#define FONT_JSON "./data/json/font.json"

#define NUM_LINES 6
#define NUM_CHARS_LINE 55
#define TOTAL_CHAR (NUM_LINES * NUM_CHARS_LINE)
#define LINE_TEXT_SIZE 31.f

#define INIT_POS VEC3(0.32f, 0.21f, 0.35f)

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
	gui_letters.resize(TOTAL_CHAR);
	for (int i = 0; i < TOTAL_CHAR; ++i) {
		gui_letters[i] = Gui->addGuiElement("ui/Fading_Letter", VEC3(0.0f, -1.0, 0.31f), "", scale);
		GET_COMP(l_gui, gui_letters[i], TCompGui);
		GET_COMP(gui_back_comp, gui_back, TCompGui);
		if (l_gui && gui_back.isValid()) l_gui->SetParent(gui_back);
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
	if (!Gui->isStartedText()) {
		Gui->initTextConfig();
	}
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
	reload(ReloadInfo());
	return true;
}

bool TCompFadingMessage::reload(const ReloadInfo& atts, float reloadAll)
{
	if (!initialized) {
		Init();
	}
	else if (enabled) {
		hideAll();
	}
	this->atts = atts;
	VEC3 new_pos1 = min_ortho + orthorect * VEC3(0.22f, 0.19f, 0.35f);

	text = Font::getVChar(lang_manager->getText(atts.text));
	text = Font::formatVChar(text, NUM_CHARS_LINE);
	permanent = atts.permanent;
	std::string who = atts.icon;
	ttl = timeForLetter * text.size() + 4.0f;

	VEC3 new_pos2 = min_ortho + orthorect * VEC3(0.55f, 0.15f, 0.3f);
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
	cur_line = cur_char_line = 0;
	partialReload = true;
	accumSpacing = 0.f;
	num_large_chars = 0;
	if (reloadAll) {
		numchars = 0;
		accumTime = 0.0f;
		partialReload = false;
	}
	return true;
}

void TCompFadingMessage::update(float dt) {
	if (!enabled) return;

	if (!partialReload) {
		shown_chars = numchars;
	}
	else {
		partialReload = false;
	}

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
	//Gui->initTextConfig();

	bool b = false;
	int gState = GameController->GetGameState();
	if (gState != CGameController::RUNNING) return;
	VEC3 init_pos = Gui->getWorldPos(INIT_POS);
	for (int i = shown_chars; i < numchars; ++i) {
		if (text[i].IsNewLine() /*|| cur_char_line >= NUM_CHARS_LINE*/) {
			cur_line++;
			cur_char_line = 0;
			accumSpacing = 0;
			if (text[i].IsNewLine()) continue;
		}
		VEC3 offset_pos;
		offset_pos.x = accumSpacing*scale;
		offset_pos.y = -cur_line*letterSpacerHigh;
		offset_pos.z = i*0.001f;
		VEC3 new_pos_let = init_pos + offset_pos;
		int letter_index = NUM_CHARS_LINE * cur_line + cur_char_line;
		if (letter_index >= TOTAL_CHAR) {
			//assert(fatal("Demasiadas letras para Fading message!\n"));
			break;
		}
		CHandle letter_h = gui_letters[letter_index + num_large_chars];
		moveElement(letter_h, new_pos_let);
		if (letter_h.isValid()) {
			GET_COMP(letter_gui, letter_h, TCompGui);
			if (letter_gui) {
				letter_gui->setTxCoords(text[i].GetTxtCoords());
				float size_letter = 1.0f;// text[i].GetSize();
				GET_COMP(letter_tmx, letter_h, TCompTransform);
				//letter_tmx->setScale(VEC3(ceil(size_letter), 1.f, 1.f));
				//Color
				VEC4 color = text[i].GetColor();
				letter_gui->SetColor(color);
				accumSpacing += size_letter;
				if (text[i].isSpecial()) {
					if (text[i].GetSize() == 2) {
						++num_large_chars;
						VEC3 offset_pos2;
						offset_pos2.x = accumSpacing*scale;
						offset_pos2.y = -cur_line*letterSpacerHigh;
						offset_pos2.z = i*0.001f;
						VEC3 new_pos_let2 = init_pos + offset_pos2;
						CHandle letter_h2 = gui_letters[letter_index + num_large_chars];
						moveElement(letter_h2, new_pos_let2);
						if (letter_h2.isValid()) {
							GET_COMP(letter_gui2, letter_h2, TCompGui);
							if (letter_gui2) {
								letter_gui2->setTxCoords(text[i].GetTxtCoords2());
								//float size_letter = 1.0f;// text[i].GetSize();
								GET_COMP(letter_tmx2, letter_h2, TCompTransform);
								//letter_tmx2->setScale(VEC3(ceil(size_letter), 1.f, 1.f));
								//Color
								letter_gui2->SetColor(color);
								accumSpacing += size_letter;
							}
						}
					}

					accumSpacing -= text[i].GetSpaceRight();
				}
				else {
					accumSpacing -= Gui->letter_sizes[text[i].getCharInt()];
				}
			}
		}
		cur_char_line++;
	}
}

void TCompFadingMessage::onLanguageChanged(const TMsgLanguageChanged &msg)
{
	reload(atts, false);
}

void TCompFadingMessage::onControlsChanged(const TMsgControlsChanged &msg)
{
	if (lang_manager->isControllerMessage(atts.text))
		reload(atts, false);
}