#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

#include "logic/aicontroller.h"
#include "logic/sbb.h"



#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"

#include "camera/camera.h"

#include "player_controller_base.h"

class CHandle;
struct TCompRenderStaticMesh;

class player_controller : public CPlayerBase {

	//enums declaration
	//--------------------------------------------------------------------
	enum eEvol {		//evolution states
		first = 0,
		second,
	};

	enum ePolarized {		//polarized state
		NEUTRAL = 0,
		MINUS,
		PLUS,
	};
	//--------------------------------------------------------------------

	//internal struct
	//--------------------------------------------------------------------
	struct TForcePoint {
		VEC3 point;
		int pol;
		TForcePoint(VEC3 new_point, int new_pol) {
			pol = new_pol;
			point = new_point;
		}
		inline bool operator==(TForcePoint other) {
			if (this->point == other.point)
				return true;
			else
				return false;
		}
	};
	//--------------------------------------------------------------------

	CObjectManager<player_controller> *om;

	//Is damaged?
	____TIMER_DECLARE_VALUE_(timerDamaged, 1.0f);

	float					full_height				= 0.5f;
	float					min_height				= 0.2f;

	//Possession
	float					DIST_LEAVING_POSSESSION	= 1.0f;
	float					possessionReach			= 3.0f;

	//evolution
	float					evolution_limit			= 10.0f;
	float					max_life					= 50.0f;
	float					init_life				= 10.0f;

	//possesion handles
	CHandle					currentPossessable;
	CHandle					currentStunable;
	
	//poses handles
	CHandle					pose_no_ev;
	CHandle					pose_idle;
	CHandle					pose_run;
	CHandle					pose_jump;

	TCompRenderStaticMesh*	actual_render			= nullptr;

	int						curr_evol				= 0;
	int						pol_state				= 0;
	int						last_pol_state			= 0;

	bool						affectPolarized			= false;

	bool						canRecEnergy				= false;
	bool						canPassWire				= false;

	VEC3						endPointWire				= VEC3(0,0,0);

	std::vector<TForcePoint> force_points;

	//private functions
	//--------------------------------------------------------------------

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

	bool isDamaged();
	bool nearStunable();

	void recalcPossassable();
	void UpdatePossession();

	void ChangePose(CHandle new_pos_h);

	void rechargeEnergy();

	void UpdateMoves() override;
	void UpdateInputActions() override;
	void UpdateActionsTrigger();
	void SetCharacterController();

	void RecalcAttractions();
	VEC3 AttractMove(VEC3 point_pos);
	//--------------------------------------------------------------------


protected:
	void myUpdate();

public:
	void Init();
	void Idle();

	void DoubleJump();
	void DoubleFalling();

	void Jumping();
	void Falling();

	void update_msgs() override;

	//input Messages
	void onLeaveFromPossession(const TMsgPossessionLeave&);
	void onDamage(const TMsgDamage&);
	void onWirePass(const TMsgWirePass& msg);
	void onCanRec(const TMsgCanRec& msg);
	void onPolarize(const TMsgPolarize& msg);

	//output messages
	void SendMessagePolarizeState();

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif