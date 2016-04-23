#ifndef INC_MODULE_GUI_BUTTON_STD_H_
#define INC_MODULE_GUI_BUTTON_STD_H_

#include "gui_button.h"

//Forward Declaration
class ImFont;

class CGuiButtonStd : public CGuiButton {
	Rect rect;
	float sizeFont;
	char * text;

public:
	CGuiButtonStd(Rect r, float sizeFont, char * txt);
	bool render();
};

#endif
