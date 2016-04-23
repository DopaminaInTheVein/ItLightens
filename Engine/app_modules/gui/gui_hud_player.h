#ifndef INC_MODULE_GUI_HUD_PLAYER_H_
#define INC_MODULE_GUI_HUD_PLAYER_H_

#include "gui_utils.h"

//Forward Declaration
class CGuiBarColor;
class Rect;

class CGuiHudPlayer {
	Rect rect;
	float sizeFont;
	CGuiBarColor * barSmall;
	CGuiBarColor * barBig;
public:
	CGuiHudPlayer(Rect r);

	void update(float dt);
	void render();
};

#endif
