#include "mcv_platform.h"
#include "windows/app.h"
#include "render/render.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

	CApp& app = CApp::get();

	if (!app.createWindow(hInstance, nCmdShow))
		return -1;

	if (!Render.createDevice() )
		return -1;

	if (!app.start())
		return -1;

	app.mainLoop();
  
  app.stop();

  Render.destroyDevice();

	return 0;
}
