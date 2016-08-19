#include "mcv_platform.h"
#include "windows/app.h"
#include "render/render.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	CApp& app = CApp::get();

	CLog::reset();
	CLog::append("Start It Lightens Engine.");

	CLog::append("Creating window...");
	if (!app.createWindow(hInstance, nCmdShow))
		return -1;
	CLog::append("Window created.");
	CLog::append("Creating device...");
	if (!Render.createDevice())
		return -1;
	CLog::append("Device created.");

	CLog::append("Starting app...");
	if (!app.start())
		return -1;
	CLog::append("App started.");

	CLog::append("Start main loop.");
	app.mainLoop();
	CLog::append("Main loop ends.");

	CLog::append("Stopping application...");
	app.stop();
	CLog::append("Application stoped.");

	CLog::append("Destroying render device...");
	Render.destroyDevice();
	CLog::append("Render device destroyed.");

	return 0;
}