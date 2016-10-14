#ifndef INC_WINDOWS_APP_H_
#define INC_WINDOWS_APP_H_

#include <windows.h>
#include "utils/timer.h"
#include <set>

#define DATA_JSON "data/json/data.json"

class IAppModule;

// ------------------------------------------------------
class CApp {
public:
	typedef std::vector<IAppModule*> VModules;
	std::set<std::string> has_check_point;
	// Remove copy ctor
	CApp(const CApp&) = delete;

	// Enable default ctor
	CApp();

	std::string file_options_json;
	std::string file_initAttr_json;

	bool max_screen;
	bool long_frame = false;
	// ask_window, pixels of the application, render otherwise
	int getXRes(bool ask_window = false);
	int getYRes(bool ask_window = false);
	bool maxScreen();
	bool windowedScreen();
	bool createWindow(HINSTANCE new_hInstance, int nCmdShow);
	void mainLoop();
	HINSTANCE getHInstance() { return hInstance; }
	HWND getHWnd() { return hWnd; }
	bool start();
	void stop();
	void changeScene(std::string level);
	void loadEntities(std::string file_name);
	void loadedLevelNotify(bool new_level);
	//void restart();
	void restartLevelNotify();
	void saveLevel();
	void clearSaveData();
	std::string getCurrentRealLevel();
	std::string getRealLevel(std::string logic_level);
	std::string getCurrentLogicLevel();
	void exitGame();
	void SetLoadingState(float loading);

	std::map<std::string, std::string> GetNameScenes();
	void ReloadNameScenes();

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
	bool	  loading = false;

	std::string current_level = "";
	std::string next_level = "";

	VModules  all_modules;
	VModules  mod_update;
	VModules  mod_renders;
	VModules  mod_init_order;
	VModules  mod_wnd_proc;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void generateFrame();
	void update(float elapsed);
	void render();

	//Levels
	void initNextLevel();
	void restartLevel();
	void showLoadingScreen();
};

#endif
