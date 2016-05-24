#ifndef	INC_COMP_SNOOZER_H_
#define INC_COMP_SNOOZER_H_

#include "app_modules/gui/gui.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "components/comp_base.h"

class CPrefabCompiler;

struct TCompSnoozer : public TCompBase {
	CPrefabCompiler * components;
public:
	//Messages
	void onPreload(const TMsgPreload& msg);
	void onAwake(const TMsgAwake& msg);

	//Render In Menu
	void renderInMenu();
};

#endif