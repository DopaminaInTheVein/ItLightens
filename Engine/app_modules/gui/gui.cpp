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
#include "gui_menu_pause.h"

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

// ------ External functions --------//
void CGuiModule::setActionAvailable(eAction action) {
	assert(txtAction);
	txtAction->setState(action);
}

// ----------------------------------- START MODULE ----------------------------------- //
bool CGuiModule::start()
{
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();

	//Pause and Menu
	//bigRect = GUI::createRect(0.00f, 0.00f, 1.00f, 1.00f);
	//menuPause = new CGuiMenuPause();

	//Font
	imFont = imIO.Fonts->AddFontDefault();

	//Window
	initWindow();

	//Screens
	initScreens();

	//Hud Player
	//hudPlayerRect = GUI::createRect(0.05f, 0.05f, .30f, 0.05f);
	//hudPlayer = new CGuiHudPlayer(hudPlayerRect);

	//Action Text
	txtAction = new CGuiActionText(0.7f, 0.05f);

	ImGui::GetStyle().WindowPadding.x = 0.0f;
	ImGui::GetStyle().WindowPadding.y = 0.0f;
	ImGui::GetStyle().FramePadding.x = 0.0f;
	ImGui::GetStyle().FramePadding.y = 0.0f;
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;

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
		| ImGuiWindowFlags_NoSavedSettings;
	//| ImGuiWindowFlags_ShowBorders;
}

void CGuiModule::initScreens()
{
	screenRenders = vector<screenRender>(CGameController::GAME_STATES_SIZE, (screenRender)&CGuiModule::renderDefault);
	screenUpdaters = vector<screenUpdater>(CGameController::GAME_STATES_SIZE, (screenUpdater)&CGuiModule::updateDefault);

	//Add here Game States
	ADD_GAME_STATE(CGameController::RUNNING, OnPlay);
	ADD_GAME_STATE(CGameController::STOPPED, OnStop);
	ADD_GAME_STATE(CGameController::STOPPED_INTRO, OnStopIntro);
	ADD_GAME_STATE(CGameController::MENU, OnMenu);
	//ADD_GAME_STATE(CGameController::LOSE, OnDead);
	ADD_GAME_STATE(CGameController::VICTORY, OnVictory);
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

bool CGuiModule::forcedUpdate()
{
	return true;
}

void CGuiModule::toogleEnabled() {
	if (controller->changegui()) {
		enabled = !enabled;
	}
}

void inline CGuiModule::callUpdater(int state, float dt)
{
	(this->*screenUpdaters[state])(dt);
}

// ----- Update Default ----- //
void CGuiModule::updateDefault(float dt) {}

// ----- Update On Play ----- //
void CGuiModule::updateOnPlay(float dt)
{
	if (controller->isStopGameButtonPressed()) {
		GameController->SetGameState(CGameController::STOPPED);
	}
	//hudPlayer->update(dt);
	txtAction->update(dt);
}

// ----- Update On Stop ----- //
void CGuiModule::updateOnStop(float dt) {
	if (controller->isStopGameButtonPressed()) {
		GameController->SetGameState(CGameController::RUNNING);
	}
}

// ----- Update On Stop Intro----- //
void CGuiModule::updateOnStopIntro(float dt) {}

// ----- Update On Menu ----- //
void CGuiModule::updateOnMenu(float dt) {
	if (!enabled) CApp::get().exitGame();
}

// ----- Update On Dead ----- //
void CGuiModule::updateOnDead(float dt) {
	//if (controller->IsPausePressed()) {
	//	GameController->SetGameState(CGameController::RUNNING);
	//	CApp::get().restartLevelNotify();
	//}
	//else if (controller->IsBackPressed()) {
	//	CApp::get().exitGame();
	//}
}

// ----- Update On Victory ----- //
void CGuiModule::updateOnVictory(float dt) {
	if (controller->IsPausePressed()) {
		CApp::get().restartLevelNotify();
	}
	else if (controller->IsBackPressed()) {
		GameController->SetGameState(CGameController::RUNNING);
		CApp::get().exitGame();
	}
}

// ----------------------------------- RENDER MODULE ----------------------------------- //
void inline CGuiModule::callRender(int state)
{
	(this->*screenRenders[state])();
}

void CGuiModule::render() {
	if (!enabled) return;
	window_actived = there_is_text;
	there_is_text = false;
	if (window_actived) {
		activateZ(ZCFG_ALL_DISABLED);
		ImGui::Begin("Game GUI", &menu, ImVec2(resolution_x, resolution_y), 0.0f, window_flags);
		ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));
		callRender(GameController->GetGameState());
		ImGui::End();
	}
	else {
		callRender(GameController->GetGameState());
	}

	txtAction->setState(eAction::NONE);
	//ImGui::Render(); <-- Ya lo hace el módulo de ImGui!!
}

// ----- Render Default ----- //
void CGuiModule::renderDefault() {
	ImGui::TextColored(GUI::IM_RED, "GAME STATE \"%d\", doesn't have any render!", GameController->GetGameState());
}

// ----- Render On Play ----- //
void CGuiModule::renderOnPlay() {
	//hudPlayer->render();
	txtAction->render();
}

// ----- Render On Stop ----- //
void CGuiModule::renderOnStop() {
	//hudPlayer->render();
	//ImGuiState& g = *GImGui;
	//g.FontSize = resolution_y;
	//GUI::drawRect(bigRect, GUI::IM_BLACK_TRANSP);

	//// Text Pause
	//GUI::drawText(0.4f, 0.4f, GImGui->Font, 0.1f, GUI::IM_WHITE, "PAUSA");
	////ImGui::GetWindowDrawList()->AddText(g.Font, g.FontSize, ImVec2(0,0), GUI::IM_WHITE, "PAUSE");
}

// ----- Render On Stop Intro ----- //
void CGuiModule::renderOnStopIntro() {
	// Text Pause Intro
	//Rect upperRect = GUI::createRect(.00f, .00f, 1.f, .12f);
	//Rect lowerRect = GUI::createRect(.00f, .88f, 1.f, 1.f);
	//GUI::drawRect(upperRect, GUI::IM_BLACK);
	//GUI::drawRect(lowerRect, GUI::IM_BLACK);
}

// ----- Render On Dead ----- //
void CGuiModule::renderOnDead() {
	//hudPlayer->render();
	ImGuiState& g = *GImGui;
	g.FontSize = resolution_y;
	//GUI::drawRect(bigRect, GUI::IM_BLACK_TRANSP);

	// Text Dead
	GUI::drawText(0.3f, 0.4f, GImGui->Font, 0.1f, GUI::IM_WHITE, "Has muerto");
	GUI::drawText(0.3f, 0.5f, GImGui->Font, 0.05f, GUI::IM_WHITE, "Intro: Reintentar");
}

// ----- Render On Victory ----- //
void CGuiModule::renderOnVictory() {
	//hudPlayer->render();
	ImGuiState& g = *GImGui;
	g.FontSize = resolution_y;
	//GUI::drawRect(bigRect, GUI::IM_BLACK_TRANSP);

	// Text Victory
	GUI::drawText(0.3f, 0.3f, GImGui->Font, 0.1f, GUI::IM_WHITE, "Victory!!");
	GUI::drawText(0.3f, 0.5f, GImGui->Font, 0.05f, GUI::IM_WHITE, "ESC: Exit game");
	GUI::drawText(0.3f, 0.6f, GImGui->Font, 0.05f, GUI::IM_WHITE, "Enter: Play again");
}

// ----- Render On Menu ----- //
void CGuiModule::renderOnMenu() {
	renderOnPlay();
	//GUI::drawRect(bigRect, GUI::IM_BLACK_TRANSP);
	//menuPause->render();
}

// ----------------------------------- STOP MODULE ----------------------------------- //
void CGuiModule::stop() {
	dbg("GUI module stopped");
	//ImGui_ImplDX11_Shutdown(); <-- Ya lo hace el modulo de ImGui!
}

//------------------------------------------------------------------------------------//
CHandle CGuiModule::addGuiElement(std::string prefab, VEC3 pos, std::string tag, float scale)
{
	CHandle h = createPrefab(prefab);
	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();
	VEC3 new_pos = min_ortho + (max_ortho - min_ortho) * pos;
	GET_COMP(tmx, h, TCompTransform);
	tmx->setPosition(new_pos);
	if (scale != 1.0f) {
		tmx->setScale(VEC3(scale, scale, scale));
	}
	if (!tag.empty()) {
		TMsgSetTag msgTag;
		msgTag.add = true;
		msgTag.tag = tag;
		CEntity* e = h;
		e->sendMsg(msgTag);
	}

	dbg("gui_element created\n");
	return h;
}

void CGuiModule::removeGuiElementByTag(std::string tag)
{
	CHandle handle = tags_manager.getFirstHavingTag(tag.c_str());
	if (handle.isValid()) { handle.destroy(); }
}

void CGuiModule::removeAllGuiElementsByTag(std::string tag)
{
	VHandles handles = tags_manager.getHandlesByTag(tag.c_str());
	for (auto handle : handles) { handle.destroy(); }
}

void CGuiModule::updateGuiElementPositionByTag(std::string tag, VEC3 new_position) {
	CHandle handle = tags_manager.getFirstHavingTag(tag.c_str());
	if (handle.isValid()) {
		CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
		GET_COMP(ui_cam, h_ui_cam, TCompCamera);
		VEC3 min_ortho = ui_cam->getMinOrtho();
		VEC3 max_ortho = ui_cam->getMaxOrtho();
		VEC3 new_pos = min_ortho + (max_ortho - min_ortho) * new_position;
		GET_COMP(tmx, handle, TCompTransform);
		tmx->setPosition(new_pos);
	}
}