#include "mcv_platform.h"
#include "comp_trigger_lua.h"
#include "components/entity.h"

using namespace std;

void TTriggerLua::onTriggerEnter(const TMsgTriggerIn& msg) {
	executeTrigger(logic_manager->OnEnter);//, msg.other);
}

void TTriggerLua::onTriggerExit(const TMsgTriggerOut& msg) {
	executeTrigger(logic_manager->OnLeave);//, msg.other);
}

void TTriggerLua::onTriggerAction(PLAYER_TYPE playerType) {
	switch (playerType) {
	case PLAYER_TYPE::PLAYER:
		executeTrigger(logic_manager->OnAction);//, msg.other);
		break;
	case PLAYER_TYPE::SCIENTIST:
		executeTrigger(logic_manager->OnActionSci);//, msg.other);
		break;
	case PLAYER_TYPE::MOLE:
		executeTrigger(logic_manager->OnActionMole);//, msg.other);
		break;
	}
}

void TTriggerLua::onTriggerInside(const TMsgTriggerIn& msg) {
	eAction actionAvalable = getActionAvailable();
	if (actionAvalable != NONE && mActionable) {
		Gui->setActionAvailable(actionAvalable);
		if (io->keys['E'].becomesPressed() || io->mouse.left.becomesPressed()) {
			mActionable = false;
			onTriggerAction(playerType);
		}
	}
}
eAction TTriggerLua::getActionAvailable() {
	//Resultado
	eAction action = NONE;

	// Obtenemos player...
	// ... Intentamos por el handle del ultimo que entro en el trigger
	CHandle player = last_msg_in.other;
	// ... Si no lo volvemos a buscar por tag
	if (!player.isValid() || !last_msg_in.other.hasTag("player")) {
		player = tags_manager.getFirstHavingTag("player");
	}

	if (player.isValid()) {
		CEntity * ePlayer = player;

		TMsgGetWhoAmI msg;
		ePlayer->sendMsgWithReply(msg);
		playerType = msg.who;

		switch (playerType) {
		case PLAYER_TYPE::PLAYER:
			action = mAction;
			break;
		case PLAYER_TYPE::MOLE:
			action = mActionMole;
			break;
		case PLAYER_TYPE::SCIENTIST:
			action = mActionSci;
			break;
		}
	}
	return action;
}

void TTriggerLua::onSetActionable(const TMsgSetActivable& msg) {
	setActionable(msg.activable);
}

void TTriggerLua::setActionable(bool actionable) {
	mActionable = actionable;
}

void TTriggerLua::executeTrigger(CLogicManagerModule::EVENT logicEvent) { //, CHandle handle) {
	CEntity* eMe = CHandle(this).getOwner();
	logic_manager->throwEvent(logicEvent, string(eMe->getName()), CHandle(this).getOwner());
}

//Load actions
//-----------------------------------------------------------------------------
#define loadAction(actionString, actionEnum)				\
string s##actionEnum = atts.getString(actionString, "");	\
if (s##actionEnum == "activate") {							\
	actionEnum = ACTIVATE;									\
}															\
else if (s##actionEnum == "put"){							\
	actionEnum = PUT;										\
}															\
else {														\
	actionEnum = NONE;										\
}															\
mActionable = mActionable || (mAction != NONE)
//-----------------------------------------------------------------------------
bool TTriggerLua::load(MKeyValue& atts) {
	mActionable = false;
	loadAction("action", mAction);
	loadAction("actionSci", mActionSci);
	loadAction("actionMole", mActionMole);

	//Desactivado inicialmente?
	if (mActionable) {
		mActionable = atts.getBool("actived", true);
	}

	return true;
}

//bool mActionable;
//eAction mAction;
//eAction mActionSci;
//eAction mActionMole;
//PLAYER_TYPE playerType = PLAYER;
void TTriggerLua::renderInMenu() {
	ImGui::Checkbox("Actionable", &mActionable);
	ImGui::Text("Player Type: %d", playerType);
	ImGui::Text("action player: %d", mAction);
	ImGui::Text("action mole: %d", mActionMole);
	ImGui::Text("action scientist: %d", mActionSci);
}