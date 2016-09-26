#ifndef INC_MODULE_GUI_H_
#define INC_MODULE_GUI_H_

#include "app_modules/app_module.h"

#include "gui_action_text.h"

#define DECL_GUI_STATE(name) \
void render##name(); void update##name(float);

//Forward Declarations
class CGuiBarColor;
class CGuiMenuPause;

class CGuiModule : public IAppModule {
private:
	//Action Text
	CGuiActionText * txtAction;

public:
	CGuiModule() {}
	bool start() override;
	void stop() override;
	void update(float dt) override;
	const char* getName() const {
		return "gui";
	}

	CHandle addGuiElement(std::string prefab, VEC3 pos = VEC3(0.5f, 0.5f, 0.5f), std::string tag = "", float scale = 1.0f);
	VEC3 getScreenPos(VEC3 pos);
	VEC3 getUiSize();
	void updateGuiElementPositionByTag(std::string tag, VEC3 new_position);
	void removeGuiElementByTag(std::string tag);
	void removeAllGuiElementsByTag(std::string tag);

	//Text Actions
	void setActionAvailable(eAction action);
};

//extern:
extern CGuiModule* Gui;

#endif
