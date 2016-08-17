#ifndef INC_GAME_CONTROLLER_H_
#define INC_GAME_CONTROLLER_H_

#include "app_module.h"

class CGameController : public IAppModule {
	int		game_state = 0;

	bool    fx_polarize = true;
	bool	fx_glow = true;
	bool	render_culling_box = false;
	bool	free_camera = false;
	bool	cinematic = false;

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

	void TogglePauseState();
	void TogglePauseIntroState();

	void UpdateGeneralInputs();

	void update(float dt);

	bool * GetFxGlowPointer();
	bool * GetFxPolarizePointer();
	bool GetFxGlow();
	bool GetFxPolarize();
	bool * GetFreeCameraPointer();
	bool GetFreeCamera() const;
	bool IsCinematic() const;
	void SetCinematic(bool new_cinematic);
	bool * GetCullingRenderPointer();
	bool GetCullingRender() const;
	const char* getName() const;
};

extern CGameController* GameController;

#endif