#ifndef INC_MODULE_GUI_H_
#define INC_MODULE_GUI_H_

#include "app_modules/app_module.h"

#include "gui_hud_player.h"

#define DECL_GUI_STATE(name) \
void render##name(); void update##name(float);

struct Rect {
	int x; int y;
	int sx; int sy;
	Rect(int _x, int _y, int _sx, int _sy) :
		x(_x), y(_y), sx(_sx), sy(_sy) {}
	Rect() : x(0), y(0), sx(100), sy(100) {}
};

//Forward Declarations
class CGuiBarColor;

class CGuiModule : public IAppModule {
private:
	//Screen Resolution
	int resolution_x;
	int resolution_y;

	//Bar Test
	CGuiHudPlayer * hudPlayer;

	//Game States Screens
	DECL_GUI_STATE(Default);
	DECL_GUI_STATE(OnPlay);

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
	bool start() override;
	void toogleEnabled();
	void initWindow();
	void initScreens();
	void stop() override;
	void update(float dt) override;
	void render() override;
	const char* getName() const {
		return "gui";
	}

	//bool onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	//static std::string getFilePath(char *filter = "All Files (*.*)\0*.*\0", HWND owner = NULL);		//open file path
};

#endif
