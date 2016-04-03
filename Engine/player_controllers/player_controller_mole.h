#ifndef INC_PLAYER_CONTROLLER_MOLE_H_
#define INC_PLAYER_CONTROLLER_MOLE_H_

#include "poss_controller.h"

#include "player_controller_base.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "components/comp_msgs.h"
#include "logic/ai_mole.h"
#include "components/comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/static_mesh.h"

#include "camera/camera.h"

class CEntity;

template< class TObj >
class CObjectManager;

class player_controller_mole : public PossController {
	CObjectManager<player_controller_mole> *om;
	float mole_max_speed;

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;

	// Main attributes
	//energies
	float grab_box_energy;
	float destroy_wall_energy;

public:

	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}

	void Init();
	void readIniFileAttr();

	void GrabBox();
	void LeaveBox();
	void DestroyWall();

	void InitControlState();
	bool nearToBox();
	bool nearToWall();
	bool boxGrabbed = false;
	string selectedBox = "";
	int selectedBoxi = 0;
	int selectedWallToBreaki = 0;

	//Cambio Malla
	TCompRenderStaticMesh* mesh;
	string pose_idle_route;
	string pose_run_route;
	string pose_jump_route;
	string pose_box_route;
	string pose_wall_route;

	void update_msgs() override;

	void UpdateInputActions();
	void UpdateMovingWithOther();
	void UpdateUnpossess();

	TCompTransform * getEntityTransform() {
		CEntity * ent = myParent;
		return ent->get<TCompTransform>();
	}
	CEntity* player_controller_mole::getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}
	CEntity * getEntityBoxPointer(int i) {
		CHandle han = SBB::readHandlesVector("wptsBoxes")[i];
		CEntity * ent = han;
		return ent;
	}
	CHandle getEntityWallHandle(int i) {
		return SBB::readHandlesVector("wptsBreakableWall")[i];
	}

	//Cambio Malla
	void ChangePose(string new_pose_route);

	void SetCharacterController();

	//Overload function for handler_manager
	player_controller_mole& player_controller_mole::operator=(player_controller_mole arg) { return arg; }
};

#endif