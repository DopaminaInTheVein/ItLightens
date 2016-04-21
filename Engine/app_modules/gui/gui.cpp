#include "mcv_platform.h"
#include "gui.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "app_modules/entities.h"
#include "handle/handle_manager.h"
#include "components/entity_tags.h"

#include "handle/object_manager.h"
#include "gui_utils.h"
#include "gui_bar_color.h"

// ImGui LIB headers
#pragma comment(lib, "imgui.lib" )


#include <Commdlg.h>

bool CGuiModule::start() {
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();
	dbg("GUI module started\n");

	return true;
}

void CGuiModule::stop() {
	dbg("GUI module stopped");
	//ImGui_ImplDX11_Shutdown();
}

void CGuiModule::update(float dt) {
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoSavedSettings
		;
	bool menu = false;
	ImGui::Begin("Game GUI", &menu, ImVec2(resolution_x, resolution_y), 0.0f, window_flags);

	//Barra de vida
	CGuiBarColor lifeBar(Rect(10, 10, 100, 20), GUI::IM_GREEN);
	static float fraction = 0.5f;
	static int sign = 1;
	lifeBar.draw(fraction);
	if (fraction < 1.f) fraction += getDeltaTime();
	else fraction -= getDeltaTime();

	ImGui::End();
}

void CGuiModule::render() {
	activateZ(ZCFG_ALL_DISABLED);
	//ImGui::Render();
}

void CGuiModule::drawBar(const char* name, Rect r, float fraction, VEC3 color) {
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoSavedSettings
		;
	bool menu = false;
	ImGui::Begin(name, &menu, ImVec2(resolution_x, resolution_y), 0.0f, window_flags);
	
	ImGui::SetWindowPos(ImVec2(r.x, r.y));
	//ImGui::Color
	ImGui::ProgressBar(fraction, ImVec2(r.sx, r.sy));
	
	ImGui::End();
}