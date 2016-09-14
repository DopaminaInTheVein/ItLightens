#include "mcv_platform.h"
#include "comp_text.h"
//#include "comp_tags.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "app_modules/gui/comps/gui_basic.h"
#include "app_modules/imgui/module_imgui.h"
#include "render/render.h"

#include <math.h>

void TCompText::forceTTLZero() {
	ttl = -0.1f;
}

bool TCompText::load(MKeyValue& atts)
{
	CHandle thisHan = CHandle(this).getOwner();

	getHandleManager<TCompText>()->each([](TCompText * mess) {
		mess->forceTTLZero();
	}
	);

	text = atts.getString("text", "defaultText");
	letter_posx_ini = atts.getFloat("pos_x", 0.0f);
	letter_posy_ini = atts.getFloat("pos_y", 0.0f);
	printed = false;
	ttl = 1.0f;
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
	return true;
}

void TCompText::update(float dt) {
	if (ttl >= 0.0f && !printed) {
		printLetters();
	}
	else if (ttl < 0.0f) {
		int letteri = 0;
		for (int j = 0; j < lineText.size(); ++j) {
			for (int i = 0; i < lineText[j].size(); ++i) {
				Gui->removeGuiElementByTag(("Text_Message_Letter_" + std::to_string(id) + "_" + std::to_string(letteri)));
				++letteri;
			}
		}
		CHandle h = CHandle(this).getOwner();
		h.destroy();
	}
}
void TCompText::printLetters() {
	int gState = GameController->GetGameState();
	if (gState != CGameController::RUNNING) return;
	int letteri = 0;
	for (int j = 0; j < lineText.size(); ++j) {
		for (int i = 0; i < lineText[j].size(); ++i) {
			char letter = lineText[j][i];
			int ascii_tex_pos = letter;
			int ascii_tex_posx = ascii_tex_pos % 16;
			int ascii_tex_posy = ascii_tex_pos / 16;

			float texture_pos_x = ((float)ascii_tex_posx) * letterBoxSize;
			float texture_pos_y = ((float)ascii_tex_posy) * letterBoxSize;
			float sx = letterBoxSize;
			float sy = letterBoxSize;

			float letter_posx = letter_posx_ini + 0.375f + i * sizeFontX;
			float letter_posy = letter_posy_ini - 0.15f - j * sizeFontY;

			CHandle letter_h = Gui->addGuiElement("ui/Fading_Letter", VEC3(letter_posx, letter_posy, 0.50f), ("Text_Message_Letter_" + std::to_string(id) + "_" + std::to_string(letteri)));
			CEntity * letter_e = letter_h;
			TCompGui * letter_gui = letter_e->get<TCompGui>();
			assert(letter_gui);
			RectNormalized textCords(texture_pos_x, texture_pos_y, sx, sy);
			letter_gui->setTxCoords(textCords);
			letteri++;
		}
	}
	printed = true;
}