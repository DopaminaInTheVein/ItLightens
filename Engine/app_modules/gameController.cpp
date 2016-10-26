#include "mcv_platform.h"
#include "gameController.h"

#include "app_modules/io/input_wrapper.h"
#include "app_modules/gui/gui.h"
#include "debug/debug_itlightens.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "lang_manager/lang_manager.h"

#include "components/entity.h"
#include "components/entity_tags.h"
#include "app_modules/render/module_render_deferred.h"
#include "app_modules/navmesh/navmesh_manager.h"

extern CRenderDeferredModule * render_deferred;

bool CGameController::start()
{
	return true;
}

DIFFICULTIES CGameController::GetDifficulty() const
{
	return game_difficulty;
}
void CGameController::SetDifficulty(int diff)
{
	assert(diff >= 0 && diff < DIFFICULTIES::DIFF_SIZE);
	game_difficulty = (DIFFICULTIES)diff;
	UpdateDifficulty();
}

void CGameController::UpdateDifficulty()
{
	Damage::init();
	getHandleManager<CEntity>()->each([](CEntity * e) {
		e->sendMsg(TMsgDifficultyChanged());
	});
}

void CGameController::Setup()
{
	auto file = CApp::get().file_options_json;
	auto values = readIniAtrData(file, "game");
	SetDifficulty((int)values["difficulty"]);
	Damage::init();
}
int CGameController::GetGameState() const {
	int res = game_state;
	if (Gui && Gui->IsUiControl()) res = STOPPED;
	return res;
}
void CGameController::SetGameState(int state) {
	if (game_state == state) return;
	game_state = state;
}

int CGameController::GetLoadingState() const { return loading_state; }
void CGameController::SetLoadingState(float state) {
	loading_state = state;
}
void CGameController::AddLoadingState(float delta) {
	loading_state += delta;
	CApp::get().SetLoadingState(loading_state);
}

void CGameController::TogglePauseState() {
	if (game_state == RUNNING)
		game_state = STOPPED;

	else if (game_state == STOPPED)
		game_state = RUNNING;
}

void CGameController::TogglePauseIntroState() {
	if (game_state == RUNNING)
		game_state = SPECIAL_ACTION;

	else if (game_state == SPECIAL_ACTION)
		game_state = RUNNING;
}

void CGameController::UpdateGeneralInputs() {
	if (!ImGui::GetIO().WantTextInput) { //not input wanted from imgui
										 //exit game
		if (controller->IsPausePressed() && game_state == RUNNING) {
			if (Gui && !Gui->IsUiControl()) {
				logic_manager->throwEvent(CLogicManagerModule::EVENT::OnPause, "");
			}
		}
#ifndef FINAL_BUILD
		//toggle console log
		if (controller->isToogleConsoleLoguttonPressed()) {
			Debug->setOpen(!*Debug->getStatus());
		}

		//toggle command log
		if (controller->isToogleCommandLogButtonPressed()) {
			Debug->ToggleConsole();
		}

		//lock/unlock free camera
		if (controller->isCameraReleaseButtonPressed()) {
			free_camera = !free_camera;
		}

		if (controller->isPauseDebugPressed()) {
			TogglePauseState();
		}
#endif
		//pause/unpause game (intro mode)
		if (controller->isPauseGameButtonPressed()) {
			TogglePauseIntroState();
		}
	}
}

void CGameController::update(float dt) {
	if (!h_game_controller.isValid()) h_game_controller = tags_manager.getFirstHavingTag("game_controller");
	UpdateGeneralInputs();
}

bool CGameControllerforcedUpdate() { return true; }

bool * CGameController::GetFxGlowPointer() {
	return &fx_glow;
}

bool * CGameController::GetFxPolarizePointer() {
	return &fx_polarize;
}

bool CGameController::GetFxGlow() {
	return fx_glow;
}

bool CGameController::GetFxPolarize() {
	return fx_polarize;
}

bool * CGameController::GetFreeCameraPointer() {
	return &free_camera;
}

bool CGameController::GetFreeCamera() const {
	return free_camera;
}
bool CGameController::IsCinematic() const {
	return cinematic;
}
bool CGameController::IsCamManual() const {
	return manual_control;
}
void CGameController::SetCinematic(bool new_cinematic) {
	cinematic = new_cinematic;
}

bool * CGameController::GetCullingRenderPointer() {
	return &render_culling_box;
}

bool CGameController::GetCullingRender() const {
	return render_culling_box;
}

void CGameController::setHandleController(CHandle h) {
	h_game_controller = h;
}

bool CGameController::isSenseVisionEnabled()
{
	bool res = false;
	if (h_game_controller.isValid()) {
		GET_COMP(sv, h_game_controller, TCompSenseVision);
		if (sv) res = sv->isSenseVisionEnabled();
	}
	return res;
}

const char* CGameController::getName() const {
	return "game_controller";
}

CHandle CGameController::getHandleGameController() const {
	return h_game_controller;
}

TCompSenseVision * CGameController::getSenseVisionComp()
{
	if (h_game_controller.isValid()) {
		GET_COMP(sv, h_game_controller, TCompSenseVision);
		if (sv) return sv;
	}
	return nullptr;
}

void CGameController::OnLoadedLevel(bool new_level, bool load_game)
{
	//Maybe levels and checkpoints should be in GameController as well
	// Navmesh
	if (new_level) {
		CNavmeshManager::initNavmesh(CApp::get().getCurrentRealLevel());
		render_deferred->UpdateStaticShadowMaps();
	}

	char params[128];
	sprintf(params, "\"%s\", \"%s\"", CApp::get().getCurrentLogicLevel().c_str(), CApp::get().getCurrentRealLevel().c_str());
	auto game_event = load_game ? CLogicManagerModule::EVENT::OnLoadedLevel : CLogicManagerModule::EVENT::OnLevelStart;

	logic_manager->throwEvent(game_event, std::string(params));
	if (!Gui->IsUiControl())
		GameController->SetGameState(CGameController::RUNNING);

	GameController->SetLoadingState(100.f);
}