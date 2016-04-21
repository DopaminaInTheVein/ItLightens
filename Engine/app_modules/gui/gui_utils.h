#ifndef INC_MODULE_GUI_UTILS_H_
#define INC_MODULE_GUI_UTILS_H_

#include "gui.h"
#define DECL_IMGUI_COLOR(NAME, r,g,b,a) \
static const ImColor NAME = ImColor(r, g, b, a)

namespace GUI {
	DECL_IMGUI_COLOR(IM_BLACK, 0.f, 0.f, 0.f, 1.f);
	DECL_IMGUI_COLOR(IM_RED, 1.f, 0.f, 0.f, 1.f);
	DECL_IMGUI_COLOR(IM_GREEN, 0.f, 1.f, 0.f, 1.f);
	

	void drawRect(Rect, ImColor);
}


#endif
