#ifndef INC_GAME_CONTROLLER_H_
#define INC_GAME_CONTROLLER_H_

#include "app_module.h"
#include "components/comp_sense_vision.h"

enum DIFFICULTIES {
	EASY = 0,
	NORMAL,
	HARD,
	DIFF_SIZE,
};

class CGameController : public IAppModule {
	DIFFICULTIES game_difficulty = DIFFICULTIES::NORMAL;
	int		game_state = 0;
	float	loading_state = 0.f;
	bool	loading = false;

	bool    fx_polarize = true;
	bool	fx_glow = true;
	bool	render_culling_box = false;
	bool	free_camera = false;
	bool	cinematic = false;
	bool	manual_control = false;
	bool	ai_update = false;

	bool	cheat_godmode = false;

	CHandle h_game_controller;

public:
	enum {
		STARTING = 0,
		PLAY_VIDEO,
		RUNNING,
		STOPPED,
		SPECIAL_ACTION,
		VICTORY,
		GAME_STATES_SIZE
	};

	bool start();

	void Setup();

	DIFFICULTIES GetDifficulty() const;
	void SetDifficulty(int);
	void UpdateDifficulty();

	std::string GetLanguage() const;
	void SetLanguage(std::string);

	int GetGameState() const;
	void SetGameState(int state);

	int GetLoadingState() const;
	void SetLoadingState(float state);
	void AddLoadingState(float delta);
	bool IsLoadingState() { return loading; }
	void InitLoadingMenu();
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
	bool forcedUpdate() { return true; }
	void update(float dt);

	bool * GetFxGlowPointer();
	bool * GetFxPolarizePointer();
	bool GetFxGlow();
	bool GetFxPolarize();
	bool * GetFreeCameraPointer();
	bool GetFreeCamera() const;

	bool * GetCheatGodmodePointer() {
		return &cheat_godmode;
	}
	bool IsCheatGodModeActivated() const {
		return cheat_godmode;
	}

	bool IsCinematic() const;
	bool IsCamManual() const;
	void SetCinematic(bool new_cinematic);
	bool * GetCullingRenderPointer();
	bool GetCullingRender() const;
	const char* getName() const;
	void setHandleController(CHandle);

	bool isSenseVisionEnabled();
	void OnLoadedLevel(bool new_level, bool load_game);

	bool getAiUpdate() { return ai_update; }
	void setAiUpdate(bool update) { ai_update = update; }

	CHandle getHandleGameController() const;

	TCompSenseVision * getSenseVisionComp();
};

extern CGameController* GameController;

#endif