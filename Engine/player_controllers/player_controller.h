#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

#include "logic/aicontroller.h"
#include "logic/sbb.h"

#include "handle/handle.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/comp_msgs.h"
#include "components/entity.h"

#include "camera/camera.h"

#include "player_controller_base.h"

struct TCompRenderStaticMesh;


class player_controller : public CPlayerBase {
	CObjectManager<player_controller> *om;
	int topolarizedplus = -1;
	int topolarizedminus = -1;
	bool polarizedMove = false;
	bool tominus = false;
	bool toplus = false;

	float polarizedCurrentSpeed = 0.0f;

	//Is damaged?
	____TIMER_DECLARE_VALUE_(timerDamaged, 1.0f);
	bool isDamaged();

	//Possession
	float DIST_LEAVING_POSSESSION = 1.0f;
	CHandle currentPossessable;
	float possessionReach = 10.0f;

	TCompTransform * getEntityTransform() {
		SetMyEntity();
		CEntity * ent = myParent;
		return ent->get<TCompTransform>();
	}

	CHandle getMinusPointHandle(int i) {
		return SBB::readHandlesVector("wptsMinusPoint")[i];
	}
	CHandle getPlusPointHandle(int i) {
		return SBB::readHandlesVector("wptsPlusPoint")[i];
	}

	void recalcPossassable();
	void UpdatePossession();

	CHandle pose_idle;	
	CHandle pose_run;	
	CHandle pose_jump;	

	TCompRenderStaticMesh* actual_render = nullptr;

	void ChangePose(CHandle new_pos_h);

	void UpdateMoves() override;

protected:
	void myUpdate();

public:
	void Init();
	void Idle();

	void DoubleJump();
	void DoubleFalling();

	void Jumping();
	void Falling();

	void AttractToMinus();
	void AttractToPlus();
	void AttractMove(CEntity * ent);
	bool nearMinus();
	bool nearPlus();

	void update_msgs() override;
	//Mensajes
	void onLeaveFromPossession(const TMsgPossessionLeave&);
	void onDamage(const TMsgDamage&);

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif