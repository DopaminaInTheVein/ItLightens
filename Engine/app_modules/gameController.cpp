#include "mcv_platform.h"
#include "gameController.h"

#include "debug\debug_itlightens.h"
#include "io\io.h"
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
		if (io->keys[VK_ESCAPE].becomesPressed() || io->joystick.button_BACK.becomesPressed()) {
			if (game_state == RUNNING) {
				SetGameState(MENU);
				io->mouse.release();
			}
			else if (game_state == MENU) {
				SetGameState(RUNNING);
				io->mouse.capture();
			}
			//CApp& app = CApp::get();
			//app.exitGame();
		}

		//restart game
		if (io->keys[VK_RETURN].becomesPressed() || io->joystick.button_START.becomesPressed()) {
			CApp::get().restartLevelNotify();
		}

		//toggle console log
		if (io->keys['L'].becomesPressed()) {
			Debug->setOpen(!*Debug->getStatus());
		}

		//toggle command log
		if (io->keys['O'].becomesPressed()) {
			Debug->ToggleConsole();
		}

		//lock/unlock free camera
		if (io->keys['K'].becomesPressed()) {
			free_camera = !free_camera;
		}

		//pause/unpause game
		//if (io->keys['P'].becomesPressed()) {
		//	TogglePauseState();
		//}

		//pause/unpause game (intro mode)
		if (io->keys['I'].becomesPressed()) {
			TogglePauseIntroState();
		}
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