#include "mcv_platform.h"
#include "app.h"
#include "app_modules/app_module.h"
//#include "utils/timer.h"

#include "utils/directory_watcher.h"
#include "resources/resources_manager.h"

// -------------------------------------------------
static CApp* app = nullptr;
CApp& CApp::get() {
	if (!app)
		app = new CApp();
	assert(app);
	return *app;
}

// -------------------------------------------------
CApp::CApp()
	: xres(800)
	, yres(600)
{
	file_options_json = "./options.json";
	file_initAttr_json = "./inicialization.json";

	std::map<std::string, float> options = readIniAtrData(file_options_json, "screen");
	
	//"0 != " to convert uint to bool more efficient
	max_screen = 0 != (int)options["full_screen"];
	xres = (int)options["xres"];
	yres = (int)options["yres"];

	// Create window
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	xres_max = desktop.right;
	yres_max = desktop.bottom;
}

int CApp::getXRes() {
	if (!max_screen)
		return xres;
	else
		return xres_max;
}
int CApp::getYRes() {
	if (!max_screen)
		return yres;
	else
		return yres_max;
}

// -------------------------------------------------
bool CApp::maxScreen() {
	SetWindowLong(getHWnd(), GWL_STYLE, WS_POPUPWINDOW);
	SetWindowPos(getHWnd(), HWND_TOP, 0, 0, xres_max, yres_max,
		SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	
	return true;
}

// -------------------------------------------------
bool CApp::windowedScreen() {
	
	
	SetWindowLong(getHWnd(), GWL_STYLE, WS_OVERLAPPEDWINDOW);
	SetWindowPos(getHWnd(), HWND_TOP, 0, 0,xres, yres,
		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);
	return true;
}

// ------------------------------------
LRESULT CALLBACK CApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	// Forward the wnd msg to each module registered to this
	auto mods = CApp::get().getModulesToWndProc();
	bool processed = false;
	for (auto m : mods)
		processed |= m->onSysMsg(hWnd, message, wParam, lParam);
	if (processed)
		return true;

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case CDirectoyWatcher::WM_FILE_CHANGED: {
		char* filename = (char*)lParam;
		if (filename) {
			Resources.onFileChanged(filename);
			delete[] filename;
		}
		break; }

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
bool CApp::createWindow(HINSTANCE new_hInstance, int nCmdShow)
{
	hInstance = new_hInstance;

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "It Lightens";
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, getXRes(), getYRes() };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow("It Lightens", "It Lightens: Milestone 2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);

	if (!hWnd)
		return false;

	if(max_screen) maxScreen();
	ShowWindow(hWnd, nCmdShow);
	return true;
}

// -------------------------------------------------
void CApp::generateFrame() {
  PROFILE_FRAME_BEGINS();
  PROFILE_FUNCTION("generateFrame");
  float delta_time = timer_app.deltaAndReset();
  delta_time = getDeltaTime();
  const float max_delta_time = 5.f / 60.f;      // 5 frames
  if (delta_time > max_delta_time)
    delta_time = max_delta_time;
	update(delta_time);
	render();
	Render.swapChain();
}

// -------------------------------------------------
void CApp::mainLoop() {
	// Main message loop
	MSG msg = { 0 };

	static CTimer timer;
	timer_app = timer;

	while (WM_QUIT != msg.message)
	{
		// Check if windows has some msg for us
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_CALL_RESTART) {
				stop();
				start();
			}
		}
		else
		{
			generateFrame();
		}
	}
}