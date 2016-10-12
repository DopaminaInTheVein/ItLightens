#ifndef INC_MODULE_IO_H_
#define INC_MODULE_IO_H_

#include "app_modules/app_module.h"

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

#define MY_SHIFT 0xA0

class CIOModule : public IAppModule {
	bool gamepad_mode = false;
	void renderInMenu();
public:
	void release_all();
	bool start() override;
	void stop() override;
	void update(float dt) override;
	void render() override;
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "io";
	}
	bool IsGamePadMode() { return gamepad_mode; }
	void SetGamePadMode(bool b) { gamepad_mode = b; }

	TKeyBoard keys;
	TMouse    mouse;
	TJoystick joystick;

	bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

extern CIOModule* io;

#endif
