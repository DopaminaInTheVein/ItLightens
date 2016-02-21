#include "mcv_platform.h"
#include "app.h"
#include "app_modules/app_module.h"

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
{}

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
	wcex.lpszClassName = "TutorialWindowClass";
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex))
		return false;

	// Create window
	RECT rc = { 0, 0, xres, yres };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow("TutorialWindowClass", "Direct3D 11 Tutorial 2: Rendering a Triangle",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!hWnd)
		return false;

	ShowWindow(hWnd, nCmdShow);

	return true;
}

// -------------------------------------------------
void CApp::generateFrame() {
	float delta_time = 1.0f / 60.f;
	update(delta_time);
	render();
	Render.swapChain();
}

// -------------------------------------------------
void CApp::mainLoop() {
	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		// Check if windows has some msg for us
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			generateFrame();
		}
	}
}