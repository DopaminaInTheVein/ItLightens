#ifndef INC_GAME_CONTROLLER_H_
#define INC_GAME_CONTROLLER_H_

#include "debug\debug_itlightens.h"
#include "app_module.h"
#include "io\io.h"

class CGameController : public IAppModule {

	int		game_state = 0;

	bool    fx_polarize = true;
	bool	fx_glow		= true;

	bool	free_camera = false;

public:
	enum{
		STARTING = 0,
		RUNNING,
		STOPPED,
		STOPPED_INTRO,
		VICTORY,
		LOSE,
		MENU,
		GAME_STATES_SIZE
	};

	int GetGameState() const { return game_state; }
	void SetGameState(int state) { game_state = state; }

	void TogglePauseState() {
		if (game_state == RUNNING)
			game_state = STOPPED;

		else if (game_state == STOPPED)
			game_state = RUNNING;
	}

	void TogglePauseIntroState() {
		if (game_state == RUNNING)
			game_state = STOPPED_INTRO;

		else if (game_state == STOPPED_INTRO)
			game_state = RUNNING;
	}

	void UpdateGeneralInputs() {
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
				CApp& app = CApp::get();
				app.restart();
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
			if (io->keys['P'].becomesPressed()) {
				TogglePauseState();
			}

			//pause/unpause game (intro mode)
			if (io->keys['I'].becomesPressed()) {
				TogglePauseIntroState();
			}
		}
	}

	void update(float dt) {
		UpdateGeneralInputs();
	}

	bool forcedUpdate() { return true; }

	bool * GetFxGlowPointer() {
		return &fx_glow;
	}

	bool * GetFxPolarizePointer() {
		return &fx_polarize;
	}

	bool GetFxGlow() {
		return fx_glow;
	}

	bool GetFxPolarize() {
		return fx_polarize;
	}

	bool * GetFreeCameraPointer() {
		return &free_camera;
	}

	bool GetFreeCamera() const {
		return free_camera;
	}


	const char* getName() const {
		return "game_controller";
	}


};

extern CGameController* GameController;

#endif