#ifndef	INC_COMP_TRIGGER_LUA
#define INC_COMP_TRIGGER_LUA

#include "comp_trigger.h"

#include "app_modules/gui/gui.h"
#include "app_modules/logic_manager/logic_manager.h"

class TTriggerLua : public TTrigger {
	bool mActionable;
	eAction mAction;
	eAction mActionSci;
	eAction mActionMole;
	PLAYER_TYPE playerType = PLAYER;

public:
	void mUpdate(float dt) override {} //not needed
	eAction getActionAvailable();
	void onTriggerInside(const TMsgTriggerIn& msg) override;
	void onTriggerEnter(const TMsgTriggerIn& msg) override;
	void onTriggerExit(const TMsgTriggerOut& msg) override;
	void onTriggerAction(PLAYER_TYPE playerType = PLAYER);
	void executeTrigger(CLogicManagerModule::EVENT logicEvent); //, CHandle handle);
	void setActionable(bool);
	void render() {}
	void init() {}

	void fixedUpdate(float elapsed) {
		(void)(elapsed);
	}
	bool load(MKeyValue& atts);
	void renderInMenu() {}

	//Messages
	void onSetActionable(const TMsgSetActivable& msg);

};

#endif