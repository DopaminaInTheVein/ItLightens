#ifndef INC_MODULE_GUI_BAR_COLOR_H_
#define INC_MODULE_GUI_BAR_COLOR_H_

#include "gui.h"
#include "gui_bar.h"
#include "geometry/geometry.h"

class CGuiBarColor : CGuiBar{
	ImColor color;
	int border;

	void drawForegroundBar(float fraction);

public:
	CGuiBarColor(Rect r, ImColor c) : color(c) {
		CGuiBar::fraction = 0.f;
		CGuiBar::rectangle = r;
		border = 5;
	};
	int getBorder();
	float getAlpha();
	void setAlpha(float);
	void setBorder(int);
	virtual void draw(float fraction);
};

#endif
