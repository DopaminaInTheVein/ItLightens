#ifndef INC_PLAYER_CONTROLLER_H_
#define INC_PLAYER_CONTROLLER_H_

#include "logic/aicontroller.h"
#include "logic/sbb.h"

#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"

#include "camera/camera.h"

#include "player_controller_base.h"
#include "skeleton_controllers/skc_player.h"

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
	
	//Polarity
	//--------------------------------------------------------------------
	VHandles polarityForces;
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
	//CHandle					pose_no_ev;
	//CHandle					pose_idle;
	//CHandle					pose_run;
	//CHandle					pose_jump;

	//Polarity Constants
	float	POL_RCLOSE					= 0.5f;
	float	POL_RFAR					= 9.0f; // 5 Meters arround
	float	POL_HORIZONTALITY			= 1.f;
	float	POL_INTENSITY				= 1000.f;
	float	POL_REPULSION				= .5f;
	float	POL_RESISTANCE				= .5f;
	float	POL_INERTIA					= 0.5f;
	float	POL_SPEED_ORBITA			= 0.2f;
	float	POL_ATRACTION_ORBITA		= 1.f;
	float	POL_NO_LEAVING_FORCE		= 0.99f;
	float	POL_ORBITA_UP_EXTRA_FORCE	= 1.f;
	float	POL_REAL_FORCE_Y_ORBITA		= 0.05f;
	float	POL_OSCILE_Y				= .2f;

	//Damage Fonts Actived
	float damageCurrent = 0.f;
	int damageFonts[Damage::SIZE] = { 0 };

	//TCompRenderStaticMesh*	actual_render = nullptr;

	int						curr_evol = 0;
	int						pol_state = 0;
	int						pol_state_prev = 0;
	int						last_pol_state = 0;
	bool					pol_orbit = false;
	bool					pol_orbit_prev = false;

	bool					affectPolarized = false;

	bool					canRecEnergy = false;
	bool					canPassWire = false;

	VEC3					endPointWire = VEC3(0, 0, 0);
	VEC3					lastForces;

	std::string				damage_source = "none";

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

	//void ChangePose(CHandle new_pos_h);

	void createEvolveLight();
	void createDevolveLight();

	void rechargeEnergy();

	void UpdateMoves() override;
	void UpdateInputActions() override;
	void UpdateOverCharge();
	void UpdateDamage();
	void startOverCharge();
	void doOverCharge();
	void Evolve(eEvol);
	void UpdateActionsTrigger();
	void SetCharacterController();

	void RecalcAttractions();
	VEC3 calcForceEffect(const PolarityForce& force);//VEC3 point_pos, bool atraction);
	VEC3 calcFinalForces(const VEC3& all_forces, const PolarityForce& nearestForce);
	void polarityMoveResistance(const PolarityForce& force);
	//--------------------------------------------------------------------

protected:
	void myUpdate();
	void ChangeCommonState(std::string);

	// the states, as maps to functions
	static map<string, statehandler> statemap;

	//Anims
	SkelControllerPlayer animController;

public:
	// Added because GUI
	float getMaxLife() { return max_life; }

	map<string, statehandler>* getStatemap();

	void Init();
	void Idle();
	void myExtraIdle();
	void readIniFileAttr();

	bool canJump() override;
	void Jump() override;
	void Jumping();
	void DoubleJump();
	void Falling();
	void DoubleFalling();

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
	int GetPolarityInt() const { return pol_state; }
	void onGetPolarity(TMsgGetPolarity& msg) { msg.polarity = (pols)pol_state; }
	void onGetWhoAmI(TMsgGetWhoAmI& msg);

	//Render In Menu
	void renderInMenu();

	//Overload function for handler_manager
	player_controller& player_controller::operator=(player_controller arg) { return arg; }
};

#endif