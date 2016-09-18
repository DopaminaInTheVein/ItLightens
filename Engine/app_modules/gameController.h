#ifndef INC_GAME_CONTROLLER_H_
#define INC_GAME_CONTROLLER_H_

#include "app_module.h"

enum DIFFICULTIES {
	EASY = 0,
	NORMAL,
	HARD,
	DIFF_SIZE,
};

class CGameController : public IAppModule {
	DIFFICULTIES game_difficulty = DIFFICULTIES::NORMAL;
	std::string game_language = "EN";
	int		game_state = 0;
	float	loading_state = 0.f;
	bool	loading = false;

	bool    fx_polarize = true;
	bool	fx_glow = true;
	bool	render_culling_box = false;
	bool	free_camera = false;
	bool	cinematic = false;
	bool	manual_control = false;
	bool    ui_control = false;

	CHandle h_game_controller;

public:
	enum {
		STARTING = 0,
		PLAY_VIDEO,
		RUNNING,
		STOPPED,
		STOPPED_INTRO,
		VICTORY,
		LOSE,
		MENU,
		GAME_STATES_SIZE
	};

	void Setup();

	DIFFICULTIES GetDifficulty() const;
	void SetDifficulty(int);

	std::string GetLanguage() const;
	void SetLanguage(std::string);

	int GetGameState() const;
	void SetGameState(int state);

	int GetLoadingState() const;
	void SetLoadingState(float state);
	void AddLoadingState(float delta);
	bool IsLoadingState() { return loading; }
	void LoadComplete(bool complete) {
		loading = !complete;
		if (complete)
			loading_state = 100.f;
		else
			loading_state = 0.f;
	}

	void TogglePauseState();
	void TogglePauseIntroState();
	void SetManualCameraState(bool state) { manual_control = state; }

	void UpdateGeneralInputs();

	void update(float dt);

	bool * GetFxGlowPointer();
	bool * GetFxPolarizePointer();
	bool GetFxGlow();
	bool GetFxPolarize();
	bool * GetFreeCameraPointer();
	bool GetFreeCamera() const;
	bool IsUiControl() const;
	bool * IsUiControlPointer();
	void SetUiControl(bool new_ui_control);

	bool IsCinematic() const;
	bool IsCamManual() const;
	void SetCinematic(bool new_cinematic);
	bool * GetCullingRenderPointer();
	bool GetCullingRender() const;
	const char* getName() const;
	void setHandleController(CHandle);

	bool isSenseVisionEnabled();
};

extern CGameController* GameController;

#endif