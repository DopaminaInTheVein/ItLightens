#include "mcv_platform.h"

#include "gui_hud_player.h"
#include "gui_utils.h"
#include "app_modules/gameData.h"
#include "gui_bar_color.h"
#include "gui.h"

CGuiHudPlayer::CGuiHudPlayer(Rect r) {
	barSmall = new CGuiBarColor(r, GUI::IM_BLUE_LIGHT);
	barBig = new CGuiBarColor(r, GUI::IM_BLUE_DARK);
}

void CGuiHudPlayer::update(float dt) {
	float life = GameData::getLifeNormalized();
	float lifeEvol = GameData::getLifeEvolution();
	float lifeMax = GameData::getLifeMax();

	barSmall->setValue(clamp(life, 0.f, lifeEvol / lifeMax));
	barBig->setValue(clamp(life, 0.f, 1.0f));

	barSmall->update(dt);
	barBig->update(dt);
}

void CGuiHudPlayer::render() {
	barBig->draw();
	barSmall->draw(false);
}