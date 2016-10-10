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

#define FONT_JSON "./data/json/font.json"

float TCompText::letterSpacing[256] = { 0.f };
bool TCompText::init_configuration = false;
void TCompText::initTextConfig()
{
	if (!init_configuration) {
		init_configuration = true;
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
}

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
	initTextConfig();

	return true;
}

void TCompText::update(float dt) {
	if (ttl < 0.0f) {
		for (CHandle h_letter : gui_letters) {
			h_letter.destroy();
		}
		CHandle h = CHandle(this).getOwner();
		h.destroy();
	}
	else if (!printed) {
		for (CHandle h_letter : gui_letters) {
			h_letter.destroy();
		}
		printLetters();
	}
	else {
		if (colorChangeSpeed > 0.0f) {
			float accumLag = 0.0f;
			for (CHandle h_letter : gui_letters) {
				GET_COMP(letter_gui, h_letter, TCompGui);
				letter_gui->setTargetColorAndSpeed(colorTarget, colorChangeSpeed, accumLag, loop);
				accumLag += colorChangeSpeedLag;
			}
		}
	}
}

void TCompText::setup(std::string set_id, std::string set_text, float set_posx, float set_posy, std::string set_textColor, float set_scale, std::string set_textColorTarget, float set_textColorSpeed, float set_textColorSpeedLag, float set_posz, bool loop) {
	id = set_id;
	assert(id != "");
	SetPosScreen(VEC3(set_posx, set_posy, set_posz));
	SetSize(set_scale);
	SetColor(set_textColor);
	SetColorTarget(set_textColorTarget, set_textColorSpeed);
	SetLetterLag(set_textColorSpeedLag);
	ttl = 1.0f;
	SetText(set_text);
	this->loop = loop;
}

void TCompText::SetText(std::string new_text)
{
	text = new_text;
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
	printed = false;
}
void TCompText::SetPosWorld(VEC3 pos)
{
	SetPosScreen(Gui->getScreenPos(pos));
}
void TCompText::SetPosScreen(VEC3 pos)
{
	letter_posx_ini = pos.x;
	letter_posy_ini = pos.y;
	letter_posz_ini = pos.z;
	printed = false;
}
void TCompText::SetSize(float size)
{
	scale = size;
	printed = false;
}
void TCompText::SetColor(std::string rrggbbaa)
{
	color = colorTarget = obtainColorNormFromString(rrggbbaa);
}
void TCompText::SetColorTarget(std::string rrggbbaa, float speed)
{
	colorTarget = obtainColorNormFromString(rrggbbaa);
	colorChangeSpeed = speed;
}
void TCompText::SetLetterLag(float letter_lag)
{
	colorChangeSpeedLag = letter_lag;
}

void TCompText::printLetters() {
	int gState = GameController->GetGameState();
	int letteri = 0;
	float accumLag = 0.0f;
	VEC3 ui_scale = VEC3(1.f, 1.f, 1.f) / Gui->getUiSize();
	float scale_x = scale *ui_scale.x;
	float scale_y = scale * line_separation * ui_scale.y;
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

			float letter_posx = letter_posx_ini + (i - accumSpacing[j]) * scale_x;
			float letter_posy = letter_posy_ini - j * scale_y;

			CHandle letter_h = Gui->addGuiElement("ui/letter", VEC3(letter_posx, letter_posy, letter_posz_ini + letteri*0.001), ("Text_Message_Letter_" + id), scale);
			CEntity * letter_e = letter_h;
			TCompGui * letter_gui = letter_e->get<TCompGui>();
			assert(letter_gui);
			letter_gui->SetParent(MY_OWNER);
			RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
			letter_gui->setTxCoords(textCords);
			letteri++;
			accumSpacing[j] += letterSpacing[ascii_tex_pos];
			letter_gui->SetColor(color);
			if (colorChangeSpeed > 0.0f) {
				letter_gui->setTargetColorAndSpeed(colorTarget, colorChangeSpeed, accumLag, loop);
			}
			gui_letters.push_back(letter_h);
			accumLag += colorChangeSpeedLag;
		}
	}
	printed = true;
}

void TCompText::Move(VEC3 movement)
{
	for (auto h : gui_letters) {
		GET_COMP(tmx, h, TCompTransform);
		tmx->setPosition(movement);
	}
}
void TCompText::SetZ(float z)
{
	float offset = 0.f;
	for (auto h : gui_letters) {
		GET_COMP(tmx, h, TCompTransform);
		tmx->setZ(z + offset);
		offset += 0.001f;
	}
}