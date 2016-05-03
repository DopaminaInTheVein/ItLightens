#ifndef INC_MODULE_GUI_H_
#define INC_MODULE_GUI_H_

#include "app_modules/app_module.h"

#include "gui_hud_player.h"
#include "imgui/imgui_internal.h"

#define DECL_GUI_STATE(name) \
void render##name(); void update##name(float);

//Forward Declarations
class CGuiBarColor;
class CGuiMenuPause;

class CGuiModule : public IAppModule {
private:
	//Screen Resolution
	int resolution_x;
	int resolution_y;

	//IO, font
	ImGuiIO& imIO = ImGui::GetIO();
	ImFont* imFont;
	//static ImGuiState       GImDefaultState;
	//ImGuiState*             GImGui = &GImDefaultState;

	//Hud Player
	Rect hudPlayerRect;
	CGuiHudPlayer * hudPlayer;

	//Pause
	Rect bigRect;

	//Menu
	CGuiMenuPause * menuPause;

	//Game States Screens
	DECL_GUI_STATE(Default);
	DECL_GUI_STATE(OnPlay);
	DECL_GUI_STATE(OnStop);
	DECL_GUI_STATE(OnStopIntro);
	DECL_GUI_STATE(OnMenu);

	//Renders & Updaters Management
	typedef void (CGuiModule::*screenRender)();
	typedef void (CGuiModule::*screenUpdater)(float);
	std::vector<screenUpdater> screenUpdaters;
	std::vector<screenRender> screenRenders;

	void inline setUpdater(int state, screenUpdater render);
	void inline setRender(int state, screenRender render);
	void inline callUpdater(int state, float dt);
	void inline callRender(int state);
	
	//ImGui Window
	bool enabled;
	bool menu;
	ImGuiWindowFlags window_flags;

public:
	CGuiModule() {}
	bool start() override;
	void toogleEnabled();
	void initWindow();
	void initScreens();
	void stop() override;
	void update(float dt) override;
	void render() override;
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "gui";
	}

	//bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	//static std::string getFilePath(char *filter = "All Files (*.*)\0*.*\0", HWND owner = NULL);		//open file path
};

#endif
