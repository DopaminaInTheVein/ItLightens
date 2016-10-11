#include "mcv_platform.h"
#include "comp_text.h"
//#include "comp_tags.h"
#include "entity.h"
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
	auto text_input = atts.getString("text", "defaultText");
	text = Font::getVChar(text_input);
	letter_posx_ini = atts.getFloat("pos_x", 0.0f);
	letter_posy_ini = atts.getFloat("pos_y", 0.0f);
	scale = atts.getFloat("scale", 1.0f);
	color = obtainColorNormFromString(atts.getString("color", "#FFFFFFFF"));
	colorTarget = obtainColorNormFromString(atts.getString("colorTarget", "#FFFFFFFF"));
	colorChangeSpeed = atts.getFloat("colorSpeed", 0.0f);
	colorChangeSpeedLag = atts.getFloat("colorSpeedLag", 0.0f);
	printed = false;
	ttl = 1.0f;
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
	text = Font::getVChar(new_text);
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

	int cur_line = 0;
	VEC3 init_pos = VEC3(letter_posx_ini, letter_posy_ini, letter_posz_ini);
	VEC3 cur_pos = init_pos;
	for (auto tchar : text) {
		if (tchar.IsNewLine()) {
			cur_pos.x = init_pos.x;
			cur_pos.y -= scale_y;
			continue;
		}

		CHandle letter_h = Gui->addGuiElement("ui/letter", cur_pos, ("Text_Message_Letter_" + id), scale);
		if (letter_h.isValid()) {
			GET_COMP(lgui, letter_h, TCompGui);
			if (lgui) {
				gui_letters.push_back(letter_h);
				lgui->SetParent(MY_OWNER);
				lgui->setTxCoords(tchar.GetTxtCoords());
				cur_pos.x += tchar.GetSize() * scale_x;
				cur_pos.z += 0.001f;
				if (tchar.GetSize() > 1.f) {
					GET_COMP(ltmx, letter_h, TCompTransform);
					ltmx->setScale(VEC3(ceilf(tchar.GetSize()), 1.f, 1.f));
				}

				//Color
				lgui->SetColor(color);
				if (colorChangeSpeed > 0.0f) {
					lgui->setTargetColorAndSpeed(colorTarget, colorChangeSpeed, accumLag, loop);
				}
				accumLag += colorChangeSpeedLag;
			}
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