#include "mcv_platform.h"
#include "comp_text.h"
//#include "comp_tags.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"
#include "render/render.h"
#include "render\draw_utils.h"

#include <math.h>

void TCompText::forceTTLZero() {
	ttl = -0.1f;
}

bool TCompText::load(MKeyValue& atts)
{
	id = atts.getString("id", "");
	assert(id != "");
	text = atts.getString("text", "defaultText");
	letter_posx_ini = atts.getFloat("pos_x", 0.0f);
	letter_posy_ini = atts.getFloat("pos_y", 0.0f);
	scale = atts.getFloat("scale", 1.0f);
	color = obtainColorNormFromString(atts.getString("color", "#FFFFFFFF"));
	colorTarget = obtainColorNormFromString(atts.getString("colorTarget", "#FFFFFFFF"));
	colorChangeSpeed = atts.getFloat("colorSpeed", 0.0f);
	colorChangeSpeedLag = atts.getFloat("colorSpeedLag", 0.0f);
	lineText.resize(0);
	printed = false;
	ttl = 1.0f;
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

	accumSpacing.resize(lineText.size());
	for (int i = 0; i < accumSpacing.size(); ++i) {
		accumSpacing[i] = 0.0f;
	}
	return true;
}

void TCompText::update(float dt) {
	if (ttl >= 0.0f && !printed) {
		printLetters();
	}
	else if (ttl < 0.0f) {
		for (CHandle h_letter : gui_letters) {
			h_letter.destroy();
		}
		CHandle h = CHandle(this).getOwner();
		h.destroy();
	}
}

void TCompText::setup(std::string set_id, std::string set_text, float set_posx, float set_posy, std::string set_textColor, float set_scale, std::string set_textColorTarget, float set_textColorSpeed, float set_textColorSpeedLag) {
	id = set_id;
	assert(id != "");
	text = set_text;
	letter_posx_ini = set_posx;
	letter_posy_ini = set_posy;
	scale = set_scale;
	color = obtainColorNormFromString(set_textColor);
	colorTarget = obtainColorNormFromString(set_textColorTarget);
	colorChangeSpeed = set_textColorSpeed;
	colorChangeSpeedLag = set_textColorSpeedLag;
	printed = false;
	ttl = 1.0f;
	lineText.resize(0);
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

	accumSpacing.resize(lineText.size());
	for (int i = 0; i < accumSpacing.size(); ++i) {
		accumSpacing[i] = 0.0f;
	}
}

void TCompText::setAttr(float new_x, float new_y, float new_scale) {
	int letteri = 0;
	accumSpacing.resize(lineText.size());
	for (int i = 0; i < accumSpacing.size(); ++i) {
		accumSpacing[i] = 0.0f;
	}
	for (int j = 0; j < lineText.size(); ++j) {
		for (int i = 0; i < lineText[j].size(); ++i) {
			unsigned char letter = lineText[j][i];
			int ascii_tex_pos = letter;
			CEntity * e_letter = gui_letters[letteri];
			TCompGui * letter_gui = e_letter->get<TCompGui>();
			TCompTransform * letter_trans = e_letter->get<TCompTransform>();
			VEC3 pos_letter = letter_trans->getPosition();
			pos_letter.x = new_x + i * letterBoxSize*new_scale - accumSpacing[j] * new_scale;
			pos_letter.y = new_y - j * letterBoxSize*new_scale*2.5f;
			letter_trans->setPosition(pos_letter);
			letter_trans->setScaleBase(VEC3(new_scale, new_scale, new_scale));
			letteri++;
			accumSpacing[j] += SBB::readLetterSpacingVector()[ascii_tex_pos];
		}
	}
}

void TCompText::printLetters() {
	int gState = GameController->GetGameState();
	if (gState != CGameController::RUNNING) return;
	int letteri = 0;
	float accumLag = 0.0f;
	for (int j = 0; j < lineText.size(); ++j) {
		for (int i = 0; i < lineText[j].size(); ++i) {
			unsigned char letter = lineText[j][i];
			int ascii_tex_pos = letter;
			int ascii_tex_posx = ascii_tex_pos % 16;
			int ascii_tex_posy = ascii_tex_pos / 16;

			float texture_pos_x = ((float)ascii_tex_posx) * letterBoxSize;
			float texture_pos_y = ((float)ascii_tex_posy) * letterBoxSize;
			float sx = letterBoxSize;
			float sy = letterBoxSize;

			float letter_posx = letter_posx_ini + i * letterBoxSize*scale - accumSpacing[j] * scale;
			float letter_posy = letter_posy_ini - j * letterBoxSize*scale*2.5f;

			CHandle letter_h = Gui->addGuiElement("ui/letter", VEC3(letter_posx, letter_posy, 0.50f + letteri*0.001), ("Text_Message_Letter_" + id), scale);
			CEntity * letter_e = letter_h;
			TCompGui * letter_gui = letter_e->get<TCompGui>();
			assert(letter_gui);
			RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
			letter_gui->setTxCoords(textCords);
			letteri++;
			accumSpacing[j] += SBB::readLetterSpacingVector()[ascii_tex_pos];
			letter_gui->SetColor(color);
			if (colorChangeSpeed > 0.0f) {
				letter_gui->setTargetColorAndSpeed(colorTarget, colorChangeSpeed, accumLag);
			}
			gui_letters.push_back(letter_h);
			accumLag += colorChangeSpeedLag;
		}
	}
	printed = true;
}