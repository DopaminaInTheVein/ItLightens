#include "mcv_platform.h"
#include "comp_room_limit.h"
#include "utils/XMLParser.h"
#include "handle/handle.h"
#include "entity.h"
#include "entity_tags.h"
#include "comp_msgs.h"
#include "player_controllers/player_controller_mole.h"
#include "player_controllers/player_controller_cientifico.h"
#include "app_modules/logic_manager/logic_manager.h"

bool TCompRoomLimit::load(MKeyValue & atts)
{
	return true;
}

void TCompRoomLimit::mUpdate(float dt)
{
	//nothing to do
}

void TCompRoomLimit::onTriggerEnter(const TMsgTriggerIn & msg)
{
	CHandle h_out = msg.other;
	if (!h_out.hasTag("player")) { return; }
	if (h_out.hasTag("raijin")) { return; }
	CEntity * pe = h_out;

	TMsgAISetPossessed msg2;
	msg2.possessed = false;
	pe->sendMsg(msg2);

	player_controller_mole * mole = pe->get<player_controller_mole>();
	player_controller_cientifico * cientifico = pe->get<player_controller_cientifico>();
	if (mole) {
		mole->UpdateUnpossess();
		mole->onSetEnable(false);
	}
	else if (cientifico) {
		cientifico->UpdateUnpossess();
		cientifico->onSetEnable(false);
	}
	logic_manager->throwEvent(logic_manager->OnUnpossess, "");
}

void TCompRoomLimit::onCreate(const TMsgEntityCreated & msg)
{
}

void TCompRoomLimit::init()
{
}