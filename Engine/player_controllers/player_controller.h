#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

#include "logic/aicontroller.h"
#include "logic/sbb.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"

#include "camera/camera.h"

#include "player_controller_base.h"

template< class TObj >
class CObjectManager;
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


	float					pol_speed = 0;

	//Is damaged?
	____TIMER_DECLARE_VALUE_(timerDamaged, 1.0f);

	float					full_height;
	float					min_height;

	//Possession
	float					DIST_LEAVING_POSSESSION;
	float					possessionReach;

	//evolution
	float					evolution_limit;
	float					max_life;
	float					init_life;

	//energy
	float					jump_energy;
	float					stun_energy;

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
	int						pol_state_prev			= 0;
	int						last_pol_state			= 0;
	bool					pol_orbit				= false;
	bool					pol_orbit_prev			= false;

	bool					affectPolarized			= false;

	bool					canRecEnergy			= false;
	bool					canPassWire				= false;

	VEC3					endPointWire			= VEC3(0,0,0);
	VEC3					lastForces;

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
	VEC3 PolarityForce(VEC3 point_pos, bool atraction);
	//--------------------------------------------------------------------


protected:
	void myUpdate();

	// the states, as maps to functions
	static map<string, statehandler> statemap;

public:

	map<string, statehandler>* getStatemap();

	void Init();
	void Idle();
	void readIniFileAttr();

	void DoubleJump();
	void DoubleFalling();

	void Jumping();
	void Falling();

	void update_msgs() override;

	//input Messages
	void onLeaveFromPossession(const TMsgPossessionLeave&);
	void onWirePass(const TMsgWirePass& msg);
	void onCanRec(const TMsgCanRec& msg);
	void onPolarize(const TMsgPolarize& msg);

	//output messages
	void SendMessagePolarizeState();

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif