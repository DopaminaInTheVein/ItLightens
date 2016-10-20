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
class CGuiMunition;
class CGuiModule : public IAppModule {
private:
	//Action Text
	CGuiActionText * txtAction;
	CGuiMunition * munition;
	static std::stack<CHandle> cursors;
	bool ui_control = false;

	bool initedText = false;
public:
	//Font
	float letter_sizes[256];
	std::map<std::string, Font::TCharacter> special_characters;
	void initTextConfig();
	bool isStartedText() { return initedText; }

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
	VEC3 getWorldPos(VEC3 pos);
	VEC3 getUiSize();
	void updateGuiElementPositionByTag(std::string tag, VEC3 new_position);
	void removeGuiElementByTag(std::string tag);
	void removeAllGuiElementsByTag(std::string tag);

	bool IsUiControl() const;
	bool * IsUiControlPointer();
	void SetUiControl(bool new_ui_control);

	//Text Actions
	void setActionAvailable(eAction action);

	//Font
	float getCharSize(unsigned char c) {
		if (c <= 256) {
			return letter_sizes[c];
		}
		assert(fatal("getSpaceRight: Unsigned char out of range!\n"));
	}
	Font::TCharacter getSpecialChar(std::string name) {
		std::string name_pad = name + "_pad";
		if (io->joystick.IsConnected() && special_characters.find(name_pad) != special_characters.end()) {
			return special_characters[name_pad];
		}
		return special_characters[name];
	}
};

//extern:
extern CGuiModule* Gui;

#endif
