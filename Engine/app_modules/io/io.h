#ifndef INC_MODULE_IO_H_
#define INC_MODULE_IO_H_

#include "app_modules/app_module.h"

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

#define MY_SHIFT 0xA0

class CIOModule : public IAppModule {
  void renderInMenu();
  void release_all();
public:
  bool start() override;
  void stop() override;
  void update(float dt) override;
  void render() override;
  bool forcedUpdate() { return true; }
  const char* getName() const {
    return "io";
  }

  TKeyBoard keys;
  TMouse    mouse;
  TJoystick joystick;

  bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

};

extern CIOModule* io;

#endif
