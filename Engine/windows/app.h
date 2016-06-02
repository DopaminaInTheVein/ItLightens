#ifndef INC_WINDOWS_APP_H_
#define INC_WINDOWS_APP_H_

#include <windows.h>

class IAppModule;

// ------------------------------------------------------
class CApp {

public:
  typedef std::vector<IAppModule*> VModules;

  // Remove copy ctor
  CApp(const CApp&) = delete;

  // Enable default ctor
  CApp();

  bool createWindow(HINSTANCE new_hInstance, int nCmdShow);
  void mainLoop();
  HWND getHWnd() { return hWnd; }
  bool start();
  void stop();

  VModules& getModulesToWndProc() {
    return mod_wnd_proc;
  }

  static CApp& get();

  int getXRes() const { return xres; }
  int getYRes() const { return yres; }

private:

	// Windows stuff
	HINSTANCE hInstance;
	HWND      hWnd;
  int       xres, yres;

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

