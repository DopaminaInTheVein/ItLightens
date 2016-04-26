#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

#include "logic/aicontroller.h"
#include "logic/sbb.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"

#include "camera/camera.h"

#include "player_controller_base.h"

#include "logic/damage.h"

template< class TObj >
class CObjectManager;
class CHandle;
struct PolarityForce;
struct TCompRenderStaticMesh;

class player_controller : public CPlayerBase {

	//enums declaration
	//--------------------------------------------------------------------
	enum eEvol {		//evolution states
		first = 0,
		second,
	};

	const string polarize_name[3] = {"neutral", "minus", "plus" };
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

	//Damages
	float					dmg_water;
	float					dmg_absorbed;

	//possesion handles
	CHandle					currentPossessable;
	CHandle					currentStunable;
	
	//poses handles
	CHandle					pose_no_ev;
	CHandle					pose_idle;
	CHandle					pose_run;
	CHandle					pose_jump;

	//Polarity Constants
	float	POL_RADIUS					= 5.f;
	float	POL_RADIUS_STRONG			= 1.f;
	float	POL_HORIZONTALITY			= 1.f;
	float	POL_INTENSITY				= 100.f;
	float	POL_REPULSION				= 1.f;
	float	POL_INERTIA					= 0.99f;
	float	POL_SPEED_ORBITA			= 0.2f;
	float	POL_ATRACTION_ORBITA		= 1.f;
	float	POL_NO_LEAVING_FORCE		= 0.99f;
	float	POL_ORBITA_UP_EXTRA_FORCE	= 1.f;
	float	POL_REAL_FORCE_Y_ORBITA		= 0.05f;

	//Damage Fonts Actived
	float damageCurrent = 0.f;
	int damageFonts[Damage::SIZE] = { 0 };

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

	std::string				damage_source			= "none";

	//std::vector<TForcePoint> force_points;
	VHandles				polarityForces;

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
	PolarityForce getPolarityForce(CHandle forceHandle);

	bool isDamaged();
	float getLife();
	void setLife(float);
	bool nearStunable();

	void recalcPossassable();
	void UpdatePossession();

	void ChangePose(CHandle new_pos_h);

	void rechargeEnergy();

	void UpdateMoves() override;
	void UpdateInputActions() override;
	void UpdateOverCharge();
	void startOverCharge();
	void doOverCharge();
	void Evolve(eEvol);
	void UpdateActionsTrigger();
	void SetCharacterController();

	void RecalcAttractions();
	VEC3 calcForceEffect(const PolarityForce& force);//VEC3 point_pos, bool atraction);
	//--------------------------------------------------------------------


protected:
	void myUpdate();

	// the states, as maps to functions
	static map<string, statehandler> statemap;

public:
	// Added because GUI
	float getMaxLife() { return max_life; }
	
	map<string, statehandler>* getStatemap();

	void Init();
	void Idle();
	void myExtraIdle();
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
	void onSetDamage(const TMsgDamageSpecific& msg);

	//output messages
	void SendMessagePolarizeState();

	//Gets
	string GetPolarity();

	//Render In Menu
	void renderInMenu();

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif