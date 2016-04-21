#ifndef INC_MODULE_GUI_H_
#define INC_MODULE_GUI_H_

#include "app_modules/app_module.h"
#include "ui/ui_interface.h"

struct Rect {
	int x; int y;
	int sx; int sy;
	Rect(int _x, int _y, int _sx, int _sy) :
		x(_x), y(_y), sx(_sx), sy(_sy) {}
	Rect() : x(0), y(0), sx(100), sy(100) {}
};

class CGuiModule : public IAppModule {
	int resolution_x;
	int resolution_y;

public:
	bool start() override;
	void stop() override;
	void update(float dt) override;
	void render() override;
	const char* getName() const {
		return "gui";
	}
	void drawBar(const char * name, Rect r, float fraction, VEC3 color);
	//bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	//static std::string getFilePath(char *filter = "All Files (*.*)\0*.*\0", HWND owner = NULL);		//open file path
};

#endif
