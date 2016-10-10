#ifndef INC_MODULE_GUI_UTILS_H_
#define INC_MODULE_GUI_UTILS_H_

struct RectNormalized {
	float x; float y;
	float sx; float sy;
	RectNormalized(float _x, float _y, float _sx, float _sy) :
		x(_x), y(_y), sx(_sx), sy(_sy) {
		assert(_x <= 1.f);
		assert(_y <= 1.f);
		assert(_sx <= 1.f);
		assert(_sy <= 1.f);
	}
	RectNormalized() : x(0), y(0), sx(1.f), sy(1.f) {}

	RectNormalized subRect(RectNormalized sub)
	{
		RectNormalized res;
		res.x = x + sub.x;
		res.y = y + sub.y;
		res.sx = sx * sub.sx;
		res.sy = sy * sub.sy;
		return res;
	}
	const RectNormalized operator/=(float v) {
		x /= v;
		y /= v;
		sx /= v;
		sy /= v;
		return *this;
	}
	const RectNormalized& operator/(float v) {
		RectNormalized res;
		res.x = x / v;
		res.y = y / v;
		res.sx = sx / v;
		res.sy = sy / v;
		return res;
	}
	const RectNormalized& operator*(float v) {
		RectNormalized res;
		res.x = x * v;
		res.y = y * v;
		res.sx = sx * v;
		res.sy = sy * v;
		return res;
	}
};

struct Rect {
	int x; int y;
	int sx; int sy;
	Rect(int _x, int _y, int _sx, int _sy) :
		x(_x), y(_y), sx(_sx), sy(_sy) {}
	Rect() : x(0), y(0), sx(100), sy(100) {}
	Rect(RectNormalized r) {
		float res_x = CApp::get().getXRes();
		float res_y = CApp::get().getYRes();
		x = r.x * res_x;
		y = r.y * res_y;
		sx = r.sx * res_x;
		sy = r.sy * res_y;
	}
};

struct Pixel {
	int x;
	int y;
	Pixel() : x(0), y(0) {}
	Pixel(int _x, int _y) : x(_x), y(_y) {}
};

#endif
