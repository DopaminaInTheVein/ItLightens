#ifndef INC_PLAYER_CONTROLLER_CIENTIFICO_H_
#define INC_PLAYER_CONTROLLER_CIENTIFICO_H_



#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"

#include "camera\camera.h"

#include "player_controller_base.h"
#include "poss_controller.h"



class CEntity;

template< class TObj >
class CObjectManager;

class player_controller_cientifico : public PossController {

	// Map for debug on ImGui
	std::map<int, std::string> out;


	CObjectManager<player_controller_cientifico> * om = nullptr;

	VEC3 directionLateral = VEC3(0, 0, 0);
	VEC3 directionForward = VEC3(0, 0, 1);
	VEC3 directionJump = VEC3(0, 0, 0);
	float rotate = 0;

	enum Obj_scientist {
		EMPTY = 0,
		DISABLE_BEACON,
		MAGNETIC_BOMB,
		MAGNETIC_BOMB_GAME,
		STATIC_BOMB,
		STATIC_BOMB_GAME
	};


	int obj = EMPTY;
	CHandle bomb_handle;

	const float t_create_beacon = 1.0f;
	const float t_create_StaticBomb = 1.0f;
	const float t_create_MagneticBomb = 1.0f;
	const float t_explode_static_bomb = 5.0f;
	float t_waiting = 0.0f;
	float t_to_explode = 5.0f;

	void UpdateInputActions() override;
	void ExplodeBomb();


public:

	player_controller_cientifico() {}
	void Init() override;
	void init() { Init(); }


	//void onSetCamera(const TMsgSetCamera& msg);
	void SetMyEntity();

	//Create game objects
	void createMagneticBombEntity();
	void createStaticBombEntity();

	// Player states
	void CreateMagneticBomb();
	void CreateDisableBeacon();
	void CreateStaticBomb();
	void AddDisableBeacon();
	void UseMagneticBomb();
	void UseStaticBomb();

	void DisabledState();
	void InitControlState();
	CEntity* getMyEntity();

	void update_msgs() override;
	void myUpdate() override;

	void renderInMenu();

	void UpdateUnpossess() override;

	//Overload function for handler_manager
	player_controller_cientifico& player_controller_cientifico::operator=(player_controller_cientifico arg) { return arg; }
};

#endif