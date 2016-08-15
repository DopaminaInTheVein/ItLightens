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
class ClHandle;
struct PolarityForce;
struct TCompRenderStaticMesh;

class player_controller : public CPlayerBase {
	//enums declaration
	//--------------------------------------------------------------------
	enum eEvol {		//evolution states
		first = 0,
		second,
	};
	const string polarize_name[3] = { "neutral", "minus", "plus" };
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
	ClHandle					currentPossessable;
	ClHandle					currentStunable;

	//Polarity Constants
	float	POL_MIN_DISTANCE = 0.1f;
	float	POL_MAX_DISTANCE = 20.0f;
	float	POL_INTENSITY = 3.5f;
	float	POL_REPULSION = 1.25f;
	float	POL_INERTIA_TIME = 1.0f;

	//Damage Fonts Actived
	float damageCurrent = 0.1f;
	int damageFonts[Damage::SIZE] = { 0 };

	//TCompRenderStaticMesh*	actual_render = nullptr;

	int					curr_evol = 0;
	int					pol_state = 0;
	int					pol_state_prev = 0;
	int					last_pol_state = 0;

	bool					affectPolarized = false;
	bool					forward_jump = false;

	ClHandle				generatorNear = ClHandle();
	bool					canPassWire = false;
	bool					canRechargeDrone = false;
	bool					canNotRechargeDrone = false;

	ClHandle			drone;

	VEC3					endPointWire = VEC3(0, 0, 0);
	vector<VEC3>			all_forces;
	VEC3					inertia_force;
	float					inertia_time = 0.f;
	vector<float>			force_ponderations;

	std::string			damage_source = "none";

	//private functions
	//--------------------------------------------------------------------

	TCompTransform * getEntityTransform() {
		return transform;
	}

	PolarityForce getPolarityForce(ClHandle forceHandle);

	bool isDamaged();
	//float getLife();
	//void setLife(float);
	bool nearStunable();

	void recalcPossassable();
	void UpdatePossession();

	//void ChangePose(ClHandle new_pos_h);

	void createEvolveLight();
	void createDevolveLight();

	void rechargeEnergy();
	void gainLife(float amoung);

	void UpdateMoves() override;
	void UpdateInputActions() override;
	void UpdateOverCharge();
	void UpdateDamage();
	void startOverCharge();
	void doOverCharge();
	void Evolve(eEvol);
	void UpdateActionsTrigger();

	void RecalcAttractions();
	VEC3 calcForceEffect(const PolarityForce& force);//VEC3 point_pos, bool atraction);
	VEC3 calcFinalForces(vector<VEC3>& forces, vector<float>& ponderations);
	//--------------------------------------------------------------------

protected:
	void myUpdate();
	void ChangeCommonState(std::string);

	// the states, as maps to functions
	static map<string, statehandler> statemap;

	//Anims
	SkelControllerPlayer * animController;

public:
	// Added because GUI
	float getMaxLife() { return max_life; }

	map<string, statehandler>* getStatemap();

	bool getUpdateInfo() override;

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
	//void UpdateAnimation() { animController.update(); }

	//input Messages
	void onLeaveFromPossession(const TMsgPossessionLeave&);
	void onWirePass(const TMsgWirePass& msg);
	void onCanRec(const TMsgCanRec& msg);
	bool canRecharge();
	void onCanRechargeDrone(const TMsgCanRechargeDrone& msg);
	void onCanNotRechargeDrone(const TMsgCanNotRechargeDrone& msg);
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