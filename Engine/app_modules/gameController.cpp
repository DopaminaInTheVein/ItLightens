#include "mcv_platform.h"
#include "gameController.h"

#include "input/input_wrapper.h"

#include "debug\debug_itlightens.h"
#include "app_modules/logic_manager/logic_manager.h"

int CGameController::GetGameState() const { return game_state; }
void CGameController::SetGameState(int state) { game_state = state; }

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
				controller->ChangeMouseState(false);
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
#ifndef NDEGUG
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

		//pause/unpause game (intro mode)
		if (controller->isPauseGameButtonPressed()) {
			TogglePauseIntroState();
		}
#endif
	}
}

void CGameController::update(float dt) {
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
void CGameController::SetCinematic(bool new_cinematic) {
	cinematic = new_cinematic;
}

bool * CGameController::GetCullingRenderPointer() {
	return &render_culling_box;
}

bool CGameController::GetCullingRender() const {
	return render_culling_box;
}

const char* CGameController::getName() const {
	return "game_controller";
}