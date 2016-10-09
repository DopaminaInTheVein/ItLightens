#include "mcv_platform.h"
#include "gui_munition.h"

#include "player_controllers/player_controller_base.h"
#include "player_controllers/player_controller_cientifico.h"
#include "gui.h"
#include "comps/gui_basic.h"
#include "components/entity.h"

VEC3 CGuiMunition::posIcon = VEC3(0.9f, 0.05f, 0.41f);
VEC3 CGuiMunition::posTxt = VEC3(0.925f, 0.01, 0.42f);
float CGuiMunition::scaleText = 0.4f;
float CGuiMunition::scaleIcon = 0.4f;

void CGuiMunition::update(float dt)
{
	if (CPlayerBase::handle_player != player) {
		player = CPlayerBase::handle_player;
		// Changed, remove previous
		icon.destroy();
		letter_x.destroy();
		number.destroy();

		// New gui elements
		if (player.isValid()) {
			TMsgGetWhoAmI msgWho;
			player.sendMsgWithReply(msgWho);
			player_type = msgWho.who;
			if (player_type == PLAYER_TYPE::SCIENTIST) {
				icon = Gui->addGuiElement("ui/icons/bomb", posIcon, "", scaleIcon);
				number = Gui->addGuiElement("ui/letter", posTxt, "", scaleText);
			}
		}
	}

	//Update gui elements
	if (!player.isValid()) return;
	if (player_type == SCIENTIST) {
		GET_COMP(sci, player, player_controller_cientifico);
		GET_COMP(gui_number, number, TCompGui);
		if (!sci || !gui_number) return;
		if (gui_number) gui_number->setTxLetter('0' + sci->getMunition());
	}
}