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
	enum eObjSci {
		EMPTY = 0,
		DISABLE_BEACON,
		MAGNETIC_BOMB,
		STATIC_BOMB,
		OBJ_SCI_SIZE
	};

	int objs_amoung[OBJ_SCI_SIZE];

	// Map for debug on ImGui
	static std::map<int, std::string> out;
	static std::vector<std::string> objs_names;
	//static std::vector<VEC3> wb_positions;

	VEC3 directionLateral = VEC3(0, 0, 0);
	VEC3 directionForward = VEC3(0, 0, 1);
	VEC3 directionJump = VEC3(0, 0, 0);
	float rotate = 0;
	bool canRepairDrone = false;
	CHandle       drone;

	eObjSci obj = EMPTY;
	CHandle bomb_handle;

	//Main attributes
	float t_waiting;
	float t_to_explode;
	float t_create_beacon;
	float t_create_StaticBomb;
	float t_create_MagneticBomb;
	float t_explode_static_bomb;
	float t_create_beacon_energy;
	float t_create_StaticBomb_energy;
	float t_create_MagneticBomb_energy;

	void UpdateInputActions() override;
	void WorkBenchActions();

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;

public:

	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}

	player_controller_cientifico() {}
	void Init() override;
	void init() { Init(); }
	void readIniFileAttr();
	void Idle() override;
	void Moving() override;
	void RecalcScientist();

	//Create game objects
	void spawnBomb(const std::string& prefab);

	// Player states
	void CreateBomb();
	void UseBomb();
	void RepairDrone();
	//void CreateStaticBomb();
	//void AddDisableBeacon();
	//void UseStaticBomb();

	void DisabledState();
	void InitControlState();
	CEntity* getMyEntity();

	void onGetWhoAmI(TMsgGetWhoAmI& msg) { msg.who = PLAYER_TYPE::SCIENTIST; }
	void myUpdate() override;
	//void update_msgs() override;

	void SetCharacterController() {}
	void onCanRepairDrone(const TMsgCanRechargeDrone& msg);

	void renderInMenu();

	void UpdateUnpossess() override;
	//Overload function for handler_manager
	player_controller_cientifico& player_controller_cientifico::operator=(player_controller_cientifico arg) { return arg; }
};

#endif