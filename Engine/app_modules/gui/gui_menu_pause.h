#ifndef INC_MODULE_GUI_MENU_PAUSE_H_
#define INC_MODULE_GUI_MENU_PAUSE_H_

#include "gui_utils.h"

//Forward Declaration
class CGuiButton;

class CGuiMenuPause {
	Rect rect;
	CGuiButton * btnResume;
	CGuiButton * btnExit;

public:
	CGuiMenuPause();
	void update(float dt) {}
	void render();
};

#endif
