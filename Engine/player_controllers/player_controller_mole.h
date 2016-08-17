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
	float grab_box_energy;
	float destroy_wall_energy;
	VEC3 push_pull_direction;
	float push_box_force;
	float pull_box_force;
	struct GrabInfo {
		float y;
		float dist;
		float yaw;
		VEC3 last_correct_pos;
		VEC3 impact;
		VEC3 pos_to_grab;
		VEC3 dir_to_grab;
		GrabInfo() : y(1.f)
			, dist(1.f)
			, yaw(0.f)
			, last_correct_pos(VEC3(0.f, 0.f, 0.f))
			, impact(VEC3(0.f, 0.f, 0.f))
			, pos_to_grab(VEC3(0.f, 0.f, 0.f))
			, dir_to_grab(VEC3(1.f, 0.f, 0.f)) {}
	};
	GrabInfo grabInfo;

	//Auxi
	bool goAndTurnTo(VEC3 target);

public:

	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}

	void Init();
	void readIniFileAttr();
	bool getUpdateInfo() override;
	void myUpdate();
	void ChangeCommonState(std::string) override;
	void GoToGrab();
	void GoToPila();
	void FaceToGrab();
	void FaceToPila();
	void GrabbingBox1();
	void GrabbingPila1();
	void GrabbingBox2();
	void GrabbingPila2();
	void GrabbingImpact();
	void GrabbingImpact1();
	void GrabbingImpact2();
	void GrabbedBox();
	void GrabbedPila();
	void LeaveBox();
	void LeavePila();
	void LeavingBox();
	void LeavingPila();
	void PuttingPila();
	void DestroyWall();
	void PushBoxPreparation();
	void PushBox();

	void InitControlState();
	bool nearToBox();
	bool nearToPila();
	bool nearToPilaContainer();
	bool nearToWall();

	void GoToPilaContainer();
	void FaceToPilaContainer();

	CHandle boxGrabbed; // = false;
	CHandle boxNear; // = false;
	CHandle boxPushed;
	CHandle pilaGrabbed; // = false;
	CHandle pilaNear; // = false;
	CHandle pilaContainer;
	VEC3 pilaContainerPos;
	float camera_push_yaw;
	float camera_push_pitch;

	bool pushing_box = false;
	bool pulling_box = false;

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
	// redefinition for push_box mode
	void UpdateMoves();
	bool UpdateMovDirection();

	// JUMP
	bool canJump() override;

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

	void onGetWhoAmI(TMsgGetWhoAmI& msg) {
		msg.who = PLAYER_TYPE::MOLE;
	}

	//Cambio Malla
	//void ChangePose(string new_pose_route);

	void SetCharacterController() {};

	//Messages
	void onGrabHit(const TMsgGrabHit&);

	bool hasPila() { return pilaGrabbed.isValid(); }

	//Load and save
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);

	//Overload function for handler_manager
	player_controller_mole& player_controller_mole::operator=(player_controller_mole arg) { return arg; }
};

#endif