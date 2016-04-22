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
	initWindow();
	initScreens();

	//Bar Test
	barTest = new CGuiBarColor(Rect(200, 50, 200, 20), GUI::IM_GREEN);
	//barTest = new CGuiBarColor(
	//	Rect(0.5f * resolution_x,
	//		0.5f * resolution_y,
	//		0.5f * resolution_x,
	//		0.5f * resolution_y)
	//	, GUI::IM_GREEN);
	
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
	screenUpdaters = vector<screenUpdater>(CGameController::GAME_STATES_SIZE, (screenUpdater)&CGuiModule::updateDefault);

	//Add here Game States
	ADD_GAME_STATE(CGameController::RUNNING, OnPlay);

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
	callUpdater(GameController->GetGameState(), dt);
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
	//if (io->keys[VK_CONTROL].becomesPressed()) barTest->setFraction(0.01f);
	//else if (io->keys[VK_DOWN].becomesPressed()) barTest->setFraction(1.0f);
	barTest->setValue(GAMEDATA::PLAYER::getLife());
	barTest->update(dt);
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
	barTest->draw();
}

// ----------------------------------- STOP MODULE ----------------------------------- //
void CGuiModule::stop() {
	dbg("GUI module stopped");
	//ImGui_ImplDX11_Shutdown(); <-- Ya lo hace el modulo de ImGui!
}