#ifndef INC_MODULE_GUI_UTILS_H_
#define INC_MODULE_GUI_UTILS_H_

#include "gui.h"
#define DECL_IMGUI_COLOR(NAME, r,g,b,a) \
static const ImColor NAME = ImColor(r, g, b, a)

struct Rect;

namespace GUI {
	DECL_IMGUI_COLOR(IM_BLACK,		0.0f, 0.0f, 0.0f, 1.f);
	DECL_IMGUI_COLOR(IM_RED,		1.0f, 0.0f, 0.0f, 1.f);
	DECL_IMGUI_COLOR(IM_GREEN,		0.0f, 1.0f, 0.0f, 1.f);
	DECL_IMGUI_COLOR(IM_BLUE_LIGHT, 0.2f, 0.2f, 1.0f, 0.8f);
	DECL_IMGUI_COLOR(IM_BLUE_DARK,	0.0f, 0.0f, 0.8f, 0.8f);

	
	Rect createRect(float x, float y, float sx, float sy);
	void drawRect(Rect, ImColor);
}


#endif
