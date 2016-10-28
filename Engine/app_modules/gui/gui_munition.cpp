#include "mcv_platform.h"
#include "gui_munition.h"

#include "player_controllers/player_controller_base.h"
#include "player_controllers/player_controller_cientifico.h"
#include "gui.h"
#include "comps/gui_basic.h"
#include "components/entity.h"
#include "components/comp_render_static_mesh.h"

VEC3 CGuiMunition::posIcon = VEC3(0.9f, 0.05f, 0.41f);
VEC3 CGuiMunition::posTxt = VEC3(0.05f, 0.06, 0.9f);
float CGuiMunition::scaleText = 0.5f;
float CGuiMunition::scaleIcon = 0.4f;

void CGuiMunition::update(float dt)
{
	//if (CPlayerBase::handle_player != player) {
	player = CPlayerBase::handle_player;
	// Changed, remove previous
	setVisible(icon, false);
	setVisible(number_back, false);
	//letter_x.destroy();
	setVisible(number, false);

	polq = tags_manager.getFirstHavingTag("ui_pol_q");
	pole = tags_manager.getFirstHavingTag("ui_pol_e");
	// New gui elements
	if (player.isValid()) {
		TMsgGetWhoAmI msgWho;
		player.sendMsgWithReply(msgWho);
		player_type = msgWho.who;
		if (player_type == PLAYER_TYPE::SCIENTIST) {
			icon = tags_manager.getFirstHavingTag("bomb_icon");//Gui->addGuiElement("ui/icons/bomb", posIcon, "", scaleIcon);
			number_back = tags_manager.getFirstHavingTag("bomb_counter");//Gui->addGuiElement("ui/letter", posTxt, "", scaleText);
			setVisible(icon, true);
			setVisible(number_back, true);
			setVisible(number, true);
			setVisible(polq, false);
			setVisible(pole, false);
			if (!number.isValid()) number = Gui->addGuiElement("ui/letter", posTxt, "", scaleText);
		}
		else {
			setVisible(polq, true);
			setVisible(pole, true);
		}
	}
	//}

	//Update gui elements
	if (player.isValid() && !Gui->IsUiControl()) {
		if (player_type == SCIENTIST) {
			GET_COMP(sci, player, player_controller_cientifico);
			GET_COMP(gui_number, number, TCompGui);
			if (!sci || !gui_number) return;
			if (gui_number) gui_number->setTxLetter('0' + sci->getMunition());
			if (sci->getMunition() > 0) gui_number->SetColor(VEC4(0.f, 0.f, 1.f, 1.f));
			else gui_number->SetColor(VEC4(1.f, 0.f, 0.f, 1.f));
		}
	}
	else {
		setVisible(polq, false);
		setVisible(pole, false);
		setVisible(number, false);
		setVisible(number_back, false);
		setVisible(icon, false);
	}
}

void CGuiMunition::setVisible(CHandle h, bool visible)
{
	GET_COMP(stm, h, TCompRenderStaticMesh);
	if (stm) {
		if (visible) stm->Show();
		else stm->Hide();
	}
}