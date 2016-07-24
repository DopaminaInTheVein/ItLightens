#ifndef INC_WINDOWS_APP_H_
#define INC_WINDOWS_APP_H_

#include <windows.h>
#include "utils\timer.h"

class IAppModule;

// ------------------------------------------------------
class CApp {
public:
	typedef std::vector<IAppModule*> VModules;
	std::string sceneToLoad = "";

	// Remove copy ctor
	CApp(const CApp&) = delete;

	// Enable default ctor
	CApp();

	std::string file_options_json;
	std::string file_initAttr_json;
	bool max_screen;
	int getXRes();
	int getYRes();
	bool maxScreen();
	bool windowedScreen();
	bool createWindow(HINSTANCE new_hInstance, int nCmdShow);
	void mainLoop();
	HINSTANCE getHInstance() { return hInstance; }
	HWND getHWnd() { return hWnd; }
	bool start();
	void stop();
	void changeScene(std::string level);
	void loadedLevelNotify();
	//void restart();
	void restartLevel();
	void restartLevelNotify();
	std::string getCurrentRealLevel();
	std::string getCurrentLogicLevel();
	void exitGame();

	VModules& getModulesToWndProc() {
		return mod_wnd_proc;
	}

	static CApp& get();

	CTimer timer_app;

private:

	// Windows stuff
	HINSTANCE hInstance;
	HWND      hWnd;
	int       xres, yres;
	int		  xres_max, yres_max;
	int		  game_state = 0;

	VModules  all_modules;
	VModules  mod_update;
	VModules  mod_renders;
	VModules  mod_init_order;
	VModules  mod_wnd_proc;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void generateFrame();
	void update(float elapsed);
	void render();

public:
};

#endif
