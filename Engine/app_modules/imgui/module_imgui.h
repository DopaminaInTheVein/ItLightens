#ifndef INC_MODULE_IMGUI_H_
#define INC_MODULE_IMGUI_H_

#include "app_modules/app_module.h"


class CImGuiModule : public IAppModule {
public:
  bool start() override;
  void stop() override;
  void update(float dt) override;
  void render() override;
  const char* getName() const {
    return "imgui";
  }
  bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

#endif
