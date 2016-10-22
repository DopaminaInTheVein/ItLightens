#include "mcv_platform.h"
#include "comp_life.h"

#include "render\draw_utils.h"

bool TCompLife::dead = false;

void TCompLife::update(float elapsed) {
	CHandle me = CHandle(this).getOwner();
	/*if (me.hasTag("AI_poss") && !me.hasTag("player")) {
		energyDamageScale = -0.1f;
	}*/
	if (player_life && !dead) {
		if (modifier != 0 && energyDamageScale != 0)
			currentlife -= energyDamageScale*modifier*elapsed;

		if (currentlife < 0 && !dead) {
			dead = true;
			//GameController->SetGameState(CGameController::LOSE);
			currentlife = 0;
		}

		else if (currentlife > maxlife)
			currentlife = maxlife;

		if (GameController->IsCheatGodModeActivated()) {
			currentlife = maxlife;
		}
	}

	shader_ctes_globals.current_life_player = currentlife;
	shader_ctes_globals.max_life_player = maxlife;
}