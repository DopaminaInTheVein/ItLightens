#include "mcv_platform.h"
#include "mouse.h"
#include "io.h"

// The º 
#define VK_TOGGLE_CONSOLE 220

TMouse::TMouse() 
: captured_by_app( false )
{
}

void TMouse::update(float dt) {
  if(captured_by_app)
    centerSysMouse();
  
  left.setCurrentStatus(isKeyPressed(VK_LBUTTON));
  left.update(dt);
  right.setCurrentStatus(isKeyPressed(VK_RBUTTON));
  right.update(dt);
  middle.setCurrentStatus(isKeyPressed(VK_MBUTTON));
  middle.update(dt);

  if (io->keys[VK_TOGGLE_CONSOLE].becomesPressed())
    toggle();
}

void TMouse::start(HWND app_hWnd) {
  hWnd = app_hWnd;
  // Get client area of our app window
  RECT r;
  ::GetClientRect(hWnd, &r);
  cx = (r.right - r.left) / 2;
  cy = (r.bottom - r.top) / 2;
}

void TMouse::centerSysMouse() {
  POINT desktop_coords = { cx, cy };
  ::ClientToScreen(hWnd, &desktop_coords);
  ::SetCursorPos(desktop_coords.x, desktop_coords.y);
}

void TMouse::toggle() {
  if (captured_by_app)
    release();
  else
    capture();
}

void TMouse::capture() {
  captured_by_app = true;
  dx = dy = 0;
  ::ShowCursor(FALSE);
  centerSysMouse();
}

void TMouse::release() {
  captured_by_app = false;
  ::ShowCursor(TRUE);
  dx = dy = 0;
}

void TMouse::setSysMouseLoc(int new_x, int new_y) {
  if (captured_by_app) {
    dx = new_x - cx;
    dy = new_y - cy;
  }
  else {
    x = new_x;
    y = new_y;
  }
}
