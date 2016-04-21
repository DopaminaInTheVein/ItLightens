#ifndef INC_MODULE_GUI_BAR_H_
#define INC_MODULE_GUI_BAR_H_

#include "gui.h"
#include "geometry/geometry.h"

class CGuiBar {
protected:
	Rect rectangle;
	float fraction;

public:
	CGuiBar() {}
	CGuiBar(Rect r) : rectangle(r), fraction(0.f) {}
	CGuiBar(Rect r, float f) : rectangle(r), fraction(f) {}

	Rect getRectangle() { return rectangle; }
	float getFraction() { return fraction;  }

	void setRectangle(Rect new_rectangle) { rectangle = new_rectangle; }
	void setPosition(int x, int y) { rectangle.x = x; rectangle.y = y; }
	void setSize(int width, int height) { setWidth(width); setHeight(height); }
	void setWidth(int width) { rectangle.sx = width; }
	void setHeight(int height) { rectangle.sy = height; }
	float setFraction(float new_fraction) { fraction = new_fraction; }

	virtual void draw() { draw(fraction); }
	virtual void draw(float fraction) = 0;
};

#endif
