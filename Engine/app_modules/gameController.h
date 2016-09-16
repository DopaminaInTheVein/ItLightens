#ifndef INC_GAME_CONTROLLER_H_
#define INC_GAME_CONTROLLER_H_

#include "app_module.h"

class CGameController : public IAppModule {
	int		game_state = 0;
	int		loading_state = 0;

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

	int GetGameState() const;
	void SetGameState(int state);

	int GetLoadingState() const;
	void SetLoadingState(int state);
	bool loadFinished() { return loading_state > 100; }

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