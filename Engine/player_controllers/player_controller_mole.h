#ifndef INC_PLAYER_CONTROLLER_MOLE_H_
#define INC_PLAYER_CONTROLLER_MOLE_H_

#include "poss_controller.h"

#include "player_controller_base.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "components/comp_msgs.h"
#include "components/comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/static_mesh.h"

#include "camera/camera.h"

class CEntity;
class SkelControllerMole;

template< class TObj >
class CObjectManager;

class player_controller_mole : public PossController {
	CObjectManager<player_controller_mole> *om;
	float mole_max_speed;
	SkelControllerMole * animController;

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;

	// Main attributes
	//energies
	float grab_box_energy;
	float destroy_wall_energy;
	struct GrabInfo {
		float y;
		float dist;
		float yaw;
		VEC3 last_correct_pos;
		GrabInfo() : y(1.f), dist(1.f), yaw(0.f) {}
	};
	GrabInfo grabInfo;

public:

	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}

	void Init();
	void readIniFileAttr();
	bool getUpdateInfo() override;
	void myUpdate();
	void ChangeCommonState(std::string) override;
	void TurnToGrab();
	void GrabbingBox1();
	void GrabbingBox2();
	void GrabbingImpact();
	void GrabbingImpact1();
	void GrabbingImpact2();
	void GrabbedBox();
	void LeaveBox();
	void DestroyWall();

	void InitControlState();
	bool nearToBox();
	bool nearToWall();
	CHandle boxGrabbed; // = false;
	CHandle boxNear; // = false;
	string selectedBox = "";
	//int selectedBoxi = 0;
	int selectedWallToBreaki = 0;
	____TIMER_DECLARE_(t_grab_hit);

	//Cambio Malla
	//TCompRenderStaticMesh* mesh;
	//string pose_idle_route;
	//string pose_run_route;
	//string pose_jump_route;
	//string pose_box_route;
	//string pose_wall_route;

	void update_msgs() override;

	void UpdateInputActions();
	void UpdateMovingWithOther();
	void UpdateUnpossess();

	TCompTransform * getEntityTransform() {
		return transform;
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

	void onGetWhoAmI(TMsgGetWhoAmI& msg) { msg.who = PLAYER_TYPE::MOLE; }

	//Cambio Malla
	//void ChangePose(string new_pose_route);

	void SetCharacterController() {};

	//Messages
	void onGrabHit(const TMsgGrabHit&);

	//Overload function for handler_manager
	player_controller_mole& player_controller_mole::operator=(player_controller_mole arg) { return arg; }
};

#endif