#ifndef INC_MODULE_GUI_H_
#define INC_MODULE_GUI_H_

#include "app_modules/app_module.h"

#include "gui_action_text.h"
#include <stack>
#define DECL_GUI_STATE(name) \
void render##name(); void update##name(float);

//Forward Declarations
class CGuiBarColor;
class CGuiMenuPause;

class CGuiModule : public IAppModule {
private:
	//Action Text
	CGuiActionText * txtAction;
	static std::stack<CHandle> cursors;
	bool ui_control = false;

public:

	CGuiModule() {}
	bool start() override;
	void stop() override;
	void update(float dt) override;
	const char* getName() const {
		return "gui";
	}
	bool forcedUpdate() override { return true; }
	CHandle getCursor();
	void pushCursor(CHandle h);
	void setCursorEnabled(bool);

	CHandle addGuiElement(std::string prefab, VEC3 pos = VEC3(0.5f, 0.5f, 0.5f), std::string tag = "", float scale = 1.0f);
	void moveGuiElement(CHandle h, VEC3 pos, float scale = 1.0f);
	VEC3 getScreenPos(VEC3 pos);
	VEC3 getUiSize();
	void updateGuiElementPositionByTag(std::string tag, VEC3 new_position);
	void removeGuiElementByTag(std::string tag);
	void removeAllGuiElementsByTag(std::string tag);

	bool IsUiControl() const;
	bool * IsUiControlPointer();
	void SetUiControl(bool new_ui_control);

	//Text Actions
	void setActionAvailable(eAction action);
};

//extern:
extern CGuiModule* Gui;

#endif
