#ifndef INC_MODULE_GUI_UTILS_H_
#define INC_MODULE_GUI_UTILS_H_

#define DECL_IMGUI_COLOR(NAME, r,g,b,a) \
static const ImColor NAME = ImColor(r, g, b, a)

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
};

struct Rect {
	int x; int y;
	int sx; int sy;
	Rect(int _x, int _y, int _sx, int _sy) :
		x(_x), y(_y), sx(_sx), sy(_sy) {}
	Rect() : x(0), y(0), sx(100), sy(100) {}
	Rect(RectNormalized r){
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

namespace GUI {
	DECL_IMGUI_COLOR(IM_WHITE,				1.0f, 1.0f, 1.0f, 1.0f);
	DECL_IMGUI_COLOR(IM_BLACK,				0.0f, 0.0f, 0.0f, 1.0f);
	DECL_IMGUI_COLOR(IM_BLACK_TRANSP,		0.0f, 0.0f, 0.0f, 0.8f);
	DECL_IMGUI_COLOR(IM_RED,				1.0f, 0.0f, 0.0f, 1.0f);
	DECL_IMGUI_COLOR(IM_RED_DARK_TRANS,		0.3f, 0.0f, 0.0f, 0.8f);
	DECL_IMGUI_COLOR(IM_GREEN,				0.0f, 1.0f, 0.0f, 1.0f);
	DECL_IMGUI_COLOR(IM_BLUE_LIGHT,			0.2f, 0.2f, 1.0f, 0.8f);
	DECL_IMGUI_COLOR(IM_BLUE_DARK,			0.0f, 0.0f, 0.8f, 0.8f);

	
	Rect createRect(float x, float y, float sx, float sy);
	void drawRect(Rect, ImColor);
	void drawText(float x, float y, ImFont * font, float size, ImColor color, const char * text);
	void drawText(Pixel p, ImFont * font, float size, ImColor color, const char * text);
}


#endif
