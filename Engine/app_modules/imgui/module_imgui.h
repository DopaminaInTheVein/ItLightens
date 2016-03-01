#ifndef INC_MODULE_IMGUI_H_
#define INC_MODULE_IMGUI_H_

#include "app_modules/app_module.h"
#include "ui\ui_interface.h"

class CImGuiModule : public IAppModule {

	CUI ui;

public:
	bool start() override;
	void stop() override;
	void update(float dt) override;
	void render() override;
	const char* getName() const {
		return "imgui";
	}
	bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	static std::string getFilePath(char *filter = "All Files (*.*)\0*.*\0", HWND owner = NULL);		//open file path
};

#endif
