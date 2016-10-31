#ifndef INC_MODULE_IMGUI_H_
#define INC_MODULE_IMGUI_H_

#include "app_modules/app_module.h"

class CEditorLights;
class CEditorMessages;

class CImGuiModule : public IAppModule {
	static ImGuiTextFilter filter;

	//editor
	CEditorLights* m_pLights_editor;
	CEditorMessages* m_pMessages_editor;
	bool show_imgui = true;

public:
	void StartLightEditor();
	void StartMessagesEditor();
	bool start() override;
	void stop() override;
	void update(float dt) override;
	void render() override;
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "imgui";
	}
	bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	static std::string getFilePath(char *filter = "All Files (*.*)\0*.*\0", HWND owner = NULL);		//open file path
	static std::string getFilterText();
};

#endif
