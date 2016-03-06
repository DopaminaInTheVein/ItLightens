#include "mcv_platform.h"
#include "joystick.h"
#include "io.h"

// The º 
#define VK_TOGGLE_CONSOLE 220

TJoystick::TJoystick()
: captured_by_app( false )
{
}

void TJoystick::update(float dt) {
  
	button_A.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_A.update(dt);
	button_B.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_B.update(dt);
	button_X.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_X.update(dt);
	button_Y.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_Y.update(dt);
	button_R.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_R.update(dt);
	button_L.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_L.update(dt);
	button_RT.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_RT.update(dt);
	button_LT.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_LT.update(dt);
	button_START.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_START.update(dt);
	button_BACK.setCurrentStatus(isKeyPressed(VK_LBUTTON));
	button_BACK.update(dt);

}

void TJoystick::start(HWND app_hWnd) {
  hWnd = app_hWnd;

}
