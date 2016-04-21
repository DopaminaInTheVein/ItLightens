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

#include "app_modules/gameController.h"

// ImGui LIB headers
#pragma comment(lib, "imgui.lib" )

#include <Commdlg.h>

#define ADD_RENDER(state, renderName) \
setRender(state, (screenRender) &CGuiModule::renderName);

// ----------------------------------- START MODULE ----------------------------------- //
bool CGuiModule::start()
{
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();
	initWindow();
	initScreens();
	dbg("GUI module started\n");

	return true;
}

void CGuiModule::initWindow()
{
	menu = false;
	window_flags |= ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoSavedSettings
		;
}

void CGuiModule::initScreens()
{
	screenRenders = vector<screenRender>(CGameController::GAME_STATES_SIZE, (screenRender)&CGuiModule::renderDefault);

	//Add here renders
	//setRender(CGameController::RUNNING, (screenRender) &CGuiModule::renderOnPlay);
	ADD_RENDER(CGameController::RUNNING, renderOnPlay);
}

void inline CGuiModule::setRender(int state, screenRender render)
{
	assert(state >= 0);
	assert(state < CGameController::GAME_STATES_SIZE);
	screenRenders[state] = render;
}

// ----------------------------------- UPDATE MODULE ----------------------------------- //
void CGuiModule::update(float dt)
{

}

// ----------------------------------- RENDER MODULE ----------------------------------- //
void inline CGuiModule::callRender(int state)
{
	(this->*screenRenders[state])();
}

void CGuiModule::render() {
	activateZ(ZCFG_ALL_DISABLED);
	ImGui::Begin("Game GUI", &menu, ImVec2(resolution_x, resolution_y), 0.0f, window_flags);
	callRender(GameController->GetGameState());
	ImGui::End();
	//ImGui::Render(); <-- Ya lo hace el módulo de ImGui!!
}

// ----- Render Default ----- //
void CGuiModule::renderDefault() {
	ImGui::TextColored(GUI::IM_RED, "GAME STATE \"%d\", doesn't have any render!", GameController->GetGameState());
}

// ----- Render On Play ----- //
void CGuiModule::renderOnPlay() {
	CGuiBarColor lifeBar(Rect(10, 10, 100, 20), GUI::IM_GREEN);
	lifeBar.draw(0.5f);
}

// ----------------------------------- STOP MODULE ----------------------------------- //
void CGuiModule::stop() {
	dbg("GUI module stopped");
	//ImGui_ImplDX11_Shutdown(); <-- Ya lo hace el modulo de ImGui!
}