#include "mcv_platform.h"
#include "io.h"
#include "imgui/imgui.h"
#include "windows/app.h"
#include <windowsx.h>

extern CIOModule* io = nullptr;

bool isKeyPressed(int key_code) {
  return ( ::GetAsyncKeyState(key_code) & 0x8000 ) != 0;
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
}

void CIOModule::render() {
  mouse.wheel = 0;
}

bool CIOModule::onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_MOUSEMOVE: {
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    mouse.setSysMouseLoc(x, y);
    return true;
    break;
    }
  case WM_MOUSEWHEEL: {
    mouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
    break;
  }
  case WM_KEYDOWN:
    keys.sysSysStatus((int)wParam, true);
    break;
  case WM_KEYUP:
    keys.sysSysStatus((int)wParam, false);
    break;
  }
  return false;
}
