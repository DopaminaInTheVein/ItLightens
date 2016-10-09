#ifndef INC_MODULE_GUI_MUNITION_H_
#define INC_MODULE_GUI_MUNITION_H_

#include "gui_utils.h"

class CGuiMunition {
private:
	CHandle player;
	CHandle icon;
	CHandle letter_x;
	CHandle number;
	PLAYER_TYPE player_type;
public:
	static VEC3 posTxt;
	static VEC3 posIcon;
	static float scaleText;
	static float scaleIcon;
	void update(float dt);
};

#endif
