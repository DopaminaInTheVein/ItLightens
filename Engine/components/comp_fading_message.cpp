#include "mcv_platform.h"
#include "comp_fading_message.h"
//#include "comp_tags.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"

#define FONT_JSON "./data/json/font.json"

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
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();

	VEC3 new_pos1 = min_ortho + (max_ortho - min_ortho) * VEC3(0.5f, -1.0f, 0.3f);
	VEC3 new_pos2 = min_ortho + (max_ortho - min_ortho) * VEC3(0.12f, -1.0f, 0.35f);
	VEC3 new_pos3 = min_ortho + (max_ortho - min_ortho) * VEC3(0.0f, -1.0f, 0.0f);

	CEntity * gui_backe = gui_back;
	CEntity * gui_reie = gui_rai;
	CEntity * gui_mole = gui_mol;
	CEntity * gui_scie = gui_sci;
	TCompTransform * gui_backt = gui_backe->get<TCompTransform>();
	gui_backt->setPosition(new_pos1);

	TCompTransform * gui_rait = gui_reie->get<TCompTransform>();
	gui_rait->setPosition(new_pos2);
	TCompTransform * gui_molt = gui_mole->get<TCompTransform>();
	gui_molt->setPosition(new_pos2);
	TCompTransform * gui_scit = gui_scie->get<TCompTransform>();
	gui_scit->setPosition(new_pos2);
	for (CEntity * lettere : gui_letters) {
		TCompTransform * gui_lettert = lettere->get<TCompTransform>();
		gui_lettert->setPosition(new_pos3);
	}
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
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();

	VEC3 new_pos1 = min_ortho + (max_ortho - min_ortho) * VEC3(0.12f, 0.09f, 0.35f);

	CHandle thisHan = CHandle(this).getOwner();

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

	CEntity * gui_backe = gui_back;
	TCompTransform * gui_backt = gui_backe->get<TCompTransform>();
	gui_backt->setPosition(min_ortho + (max_ortho - min_ortho) * VEC3(0.5f, 0.02f, 0.3f));

	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (player.hasTag("raijin")) {
		CEntity * gui_reie = gui_rai;
		TCompTransform * gui_rait = gui_reie->get<TCompTransform>();
		gui_rait->setPosition(new_pos1);
	}
	else if (player.hasTag("AI_mole")) {
		CEntity * gui_mole = gui_mol;
		TCompTransform * gui_molt = gui_mole->get<TCompTransform>();
		gui_molt->setPosition(new_pos1);
	}
	else if (player.hasTag("AI_cientifico")) {
		CEntity * gui_scie = gui_sci;
		TCompTransform * gui_scit = gui_scie->get<TCompTransform>();
		gui_scit->setPosition(new_pos1);
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

	if (ttl < 0.0f) {
		hideAll();
	}
	else {
		printLetters();
		ttl -= dt;
	}
}
void TCompFadingMessage::printLetters() {
	bool b = false;
	int gState = GameController->GetGameState();
	if (gState != CGameController::RUNNING) return;

	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
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

		CHandle letter_h = gui_letters[50 * line + i - linechars_prev];
		CEntity * letter_e = letter_h;
		TCompGui * letter_gui = letter_e->get<TCompGui>();
		TCompTransform * letter_trans = letter_e->get<TCompTransform>();
		VEC3 new_pos = min_ortho + (max_ortho - min_ortho) * VEC3(letter_posx, letter_posy, 0.31f + i*0.001f);
		letter_trans->setPosition(new_pos);
		assert(letter_gui);
		RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
		letter_gui->setTxCoords(textCords);
		accumSpacing[line] += letterSpacing[ascii_tex_pos]; //SBB::readLetterSpacingVector()[ascii_tex_pos];
	}
}