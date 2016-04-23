#ifndef INC_MODULE_GUI_BUTTON_H_
#define INC_MODULE_GUI_BUTTON_H_

#include "gui_utils.h"

class CGuiButton {
	Rect rect;

public:
	virtual void update(float dt) {}
	virtual bool render() = 0;
};

#endif
