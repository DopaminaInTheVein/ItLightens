#include "mcv_platform.h"
#include "gameController.h"

#include "input/input_wrapper.h"

#include "debug\debug_itlightens.h"
#include "app_modules/logic_manager/logic_manager.h"

#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_sense_vision.h"

int CGameController::GetGameState() const { 
	return game_state; 
}
void CGameController::SetGameState(int state) {
	if (game_state == state) return;
	game_state = state;
	switch (game_state) {
	case CGameController::LOSE:
		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDead, "");
		break;
	}
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
		game_state = STOPPED_INTRO;

	else if (game_state == STOPPED_INTRO)
		game_state = RUNNING;
}

void CGameController::UpdateGeneralInputs() {
	if (!ImGui::GetIO().WantTextInput) { //not input wanted from imgui
										 //exit game
		if (controller->IsPausePressed()) {
			if (game_state == RUNNING) {
				SetGameState(MENU);
				//controller->ChangeMouseState(false);
				logic_manager->throwEvent(CLogicManagerModule::EVENT::OnPause, "");
			}
			else if (game_state == MENU) {
				SetGameState(RUNNING);
				controller->ChangeMouseState(true);
			}
			//CApp& app = CApp::get();
			//app.exitGame();
		}

		/*restart game
		if (controller->IsPausePressed()) {
			CApp::get().has_check_point = false;
			CApp::get().restartLevelNotify();
		}
		*/
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

		//pause/unpause game
		//if (io->keys['P'].becomesPressed()) {
		//	TogglePauseState();
		//}
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
bool CGameController::IsUiControl() const
{
	return ui_control;
}
bool * CGameController::IsUiControlPointer()
{
	return &ui_control;
}
void CGameController::SetUiControl(bool new_ui_control)
{
	ui_control = new_ui_control;
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
	GET_COMP(sv, h_game_controller, TCompSenseVision);
	return sv ? sv->isSenseVisionEnabled() : false;
}

const char* CGameController::getName() const {
	return "game_controller";
}