#include "mcv_platform.h"
#include "io.h"
#include "imgui/imgui.h"
#include "windows/app.h"
#include <windowsx.h>

extern CIOModule* io = nullptr;
static int i = 0;
bool isKeyPressed(int key_code) {
	return (::GetAsyncKeyState(key_code) & 0x8000) != 0;
}

bool CIOModule::start() {
	mouse.start(CApp::get().getHWnd());
	//mouse.capture();

	return true;
}

void CIOModule::stop() {
}

void CIOModule::update(float dt) {
	mouse.update(dt);
	keys.update(dt);
	joystick.update(dt);
}

void CIOModule::release_all() {
	mouse.release();
	keys.release();
	joystick.release();
}

void CIOModule::render() {
	mouse.wheel = 0;
}

bool CIOModule::onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	switch (message) {
	case WM_SETFOCUS:
		mouse.capture();
		return true;
		break;
	case WM_KILLFOCUS:
		release_all();
		return true;
		break;

	case WM_MOUSEMOVE:
		mouse.setSysMouseLoc(x, y);
		return true;
		break;

	case WM_MOUSEWHEEL:
		mouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		return true;
		break;

	case WM_KEYDOWN:
		keys.sysSysStatus((int)wParam, true);
		return true;
		break;

	case WM_KEYUP:
		keys.sysSysStatus((int)wParam, false);
		return true;
		break;
	}
	return false;
}