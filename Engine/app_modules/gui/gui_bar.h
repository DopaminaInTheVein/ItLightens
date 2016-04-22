#ifndef INC_MODULE_GUI_BAR_H_
#define INC_MODULE_GUI_BAR_H_

#include "gui.h"
#include "geometry/geometry.h"

class CGuiBar {
protected:
	Rect rectangle;
	float current;
	float target;
	float speed = 2.f;

public:
	CGuiBar() {}
	CGuiBar(Rect r) : rectangle(r), target(0.f), current(0.f) {}
	CGuiBar(Rect r, float f) : rectangle(r), current(f), target(f) {}

	Rect getRectangle() { return rectangle; }
	float getFraction() { return target;  }

	void setRectangle(Rect new_rectangle) { rectangle = new_rectangle; }
	void setPosition(int x, int y) { rectangle.x = x; rectangle.y = y; }
	void setSize(int width, int height) { setWidth(width); setHeight(height); }
	void setWidth(int width) { rectangle.sx = width; }
	void setHeight(int height) { rectangle.sy = height; }
	void setFraction(float new_fraction) {
		target = new_fraction;
		dbg("Set Target: %f\n", target);
	}

	void update(float dt)
	{
		float delta = current < target ? 
			  speed * dt
			: -speed * dt
			;
		current = current < target ? 
			  clamp(current + delta, current, target)
			: clamp(current + delta, target, current);
		dbg("Update Bar: current:%f, target:%f\n", current, target);
	}
	virtual void draw() = 0;
};

#endif
