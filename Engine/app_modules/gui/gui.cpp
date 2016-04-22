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
#include "app_modules/gameData.h"

// ImGui LIB headers
#pragma comment(lib, "imgui.lib" )

#include <Commdlg.h>

using namespace std;

#define ADD_RENDER(state, renderName) \
setRender(state, (screenRender) &CGuiModule::renderName)

#define ADD_UPDATER(state, updaterName) \
setUpdater(state, (screenUpdater) &CGuiModule::updaterName)

#define ADD_GAME_STATE(state, name) \
ADD_RENDER(state, render##name); ADD_UPDATER(state, update##name)
// ----------------------------------- START MODULE ----------------------------------- //
bool CGuiModule::start()
{
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();
	
	//Window
	initWindow();
	
	//Screens
	initScreens();

	//Hud Player
	hudPlayer = new CGuiHudPlayer(GUI::createRect(0.05f, 0.05f, .30f, 0.05f));
	
	dbg("GUI module started\n");

	return true;
}

void CGuiModule::initWindow()
{
	menu = false;
	enabled = true;
	window_flags |= ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoSavedSettings
		;
}

void CGuiModule::initScreens()
{
	screenRenders = vector<screenRender>(CGameController::GAME_STATES_SIZE, (screenRender)&CGuiModule::renderDefault);
	screenUpdaters = vector<screenUpdater>(CGameController::GAME_STATES_SIZE, (screenUpdater)&CGuiModule::updateDefault);

	//Add here Game States
	ADD_GAME_STATE(CGameController::RUNNING, OnPlay);
	ADD_GAME_STATE(CGameController::STOPPED, OnStop);

}

void inline CGuiModule::setRender(int state, screenRender render)
{
	assert(state >= 0);
	assert(state < CGameController::GAME_STATES_SIZE);
	screenRenders[state] = render;
}

void inline CGuiModule::setUpdater(int state, screenUpdater updater)
{
	assert(state >= 0);
	assert(state < CGameController::GAME_STATES_SIZE);
	screenUpdaters[state] = updater;
}

// ----------------------------------- UPDATE MODULE ----------------------------------- //
void CGuiModule::update(float dt)
{
	toogleEnabled();
	callUpdater(GameController->GetGameState(), dt);
}

void CGuiModule::toogleEnabled() {
	if (io->keys[VK_F1].becomesPressed()) {
		enabled = !enabled;
	}
}

void inline CGuiModule::callUpdater(int state, float dt)
{
	(this->*screenUpdaters[state])(dt);
}

// ----- Update Default ----- //
void CGuiModule::updateDefault(float dt)
{
	//Nothing to do?
}

// ----- Update On Play ----- //
void CGuiModule::updateOnPlay(float dt)
{
	hudPlayer->update(dt);
}

// ----- Update On Stop ----- //
void CGuiModule::updateOnStop(float dt)
{
	//Nothing to update?
}

// ----------------------------------- RENDER MODULE ----------------------------------- //
void inline CGuiModule::callRender(int state)
{
	(this->*screenRenders[state])();
}

void CGuiModule::render() {
	if (!enabled) return;
	activateZ(ZCFG_ALL_DISABLED);
	ImGui::Begin("Game GUI", &menu, ImVec2(resolution_x, resolution_y), 0.0f, window_flags);
	ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));
	callRender(GameController->GetGameState());
	ImGui::End();
	//ImGui::Render(); <-- Ya lo hace el m�dulo de ImGui!!
}

// ----- Render Default ----- //
void CGuiModule::renderDefault() {
	ImGui::TextColored(GUI::IM_RED, "GAME STATE \"%d\", doesn't have any render!", GameController->GetGameState());
}

// ----- Render On Play ----- //
void CGuiModule::renderOnPlay() {
	hudPlayer->render();
}

// ----- Render On Stop ----- //
void CGuiModule::renderOnStop() {
	hudPlayer->render();
	GUI::drawRect(GUI::createRect(.01f,.01f,.98f,.98f), GUI::IM_BLACK_TRANSP);
	// Text Pause
}

// ----------------------------------- STOP MODULE ----------------------------------- //
void CGuiModule::stop() {
	dbg("GUI module stopped");
	//ImGui_ImplDX11_Shutdown(); <-- Ya lo hace el modulo de ImGui!
}