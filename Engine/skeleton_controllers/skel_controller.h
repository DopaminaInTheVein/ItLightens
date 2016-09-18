#ifndef INC_SKELETON_CONTROLLER_H_
#define INC_SKELETON_CONTROLLER_H_

#include "components/comp_base.h"
#include "skeleton/skeleton_manager.h"

// --- Anims State --- //
#define AST_IDLE				"idle"
#define AST_FALL				"jumpland"
#define AST_LAND				"jumplandidle"
#define AST_JUMP				"jump"
#define AST_JUMP2				"jump"
#define AST_MOVE				"walk"
#define AST_RUN					"run"
#define AST_PREP_SHOOT			"attackprepare"
#define AST_SHOOT				"attack"
#define AST_SHOOT_BACK			"attackgoback"
#define AST_SHOOT_BACK_BODY		"attackgobackbody"
#define AST_TURN				"turnfeetonly"
#define AST_STUNNED				"stun"
#define AST_STUNNED_BOX			"stun"
#define AST_RECHARGE			"recharge"
#define AST_GRAB_DOWN			"grab_box_down"
#define AST_GRAB_UP				"grab_box_up"
#define AST_GRAB_PILA1			"grab_pila_1"
#define AST_GRAB_PILA2			"grab_pila_2"
#define AST_PUT_PILA			"put_pila"
#define AST_GRAB_IDLE			"grab_box_idle"
#define AST_GRAB_IDLE_SMALL		"grab_box_idle_small"
#define AST_PILA_IDLE			"grab_pila_idle"
#define AST_GRAB_WALK			"grab_box_walk"
#define AST_GRAB_WALK_SMALL		"grab_box_walk_small"
#define AST_PILA_WALK			"grab_pila_walk"
#define AST_WORK				"work"
#define AST_THROW				"throw"
#define AST_PUSH_IDLE			"push_idle"
#define AST_PUSH_PREP			"push_prep"
#define AST_PUSH_WALK			"push_walk"
#define AST_PULL_WALK			"pull_walk"
// ------------------- //

template< class TObj >
class CObjectManager;

class TCompSkelController {
protected:
	CHandle owner;
	std::string currentState = "";
	std::string prevState = "";
	bool priority = false;
	float lfoot_correction = 0.f;
	float rfoot_correction = 0.f;
	float lfoot_height_offset;
	float rfoot_height_offset;
	bool foot_ground_left = true;
	bool foot_ground_right = true;
	float FOOT_GROUND_EPSILON;
	float FOOT_AIR_EPSILON;

	void setAnim(std::string anim, bool loop, std::string nextLoop = "");
	void setAction(std::string anim, std::string nextLoop = "");
	void setLoop(std::string anim);
	//animaciones parciales
	void setAnim(std::vector<std::string> anim, bool loop, std::vector<std::string> nextLoop);
	void setAction(std::vector<std::string> anim, std::vector<std::string> nextLoop);
	void setLoop(std::vector<std::string> anim);

	//Virtuals
	virtual void myUpdate();
	virtual void myUpdateIK();
	virtual bool getUpdateInfo() = 0;
public:
	void update();
	void onCreate(const TMsgEntityCreated&);
	void enableIK(std::string bone_name, IK::bone_solver solver, float delay);
	void disableIK(std::string bone_name, float delay = 0.f, IK::bone_solver function = nullptr);
	void setState(std::string state, bool prio = false);
	void renderInMenuParent();

	//IK querys
	void solveFoot(const IK::InfoSolver&, IK::ResultSolver&, float& height_correction, bool& on_ground, float height_offset);
	void solveFootLeft(const IK::InfoSolver&, IK::ResultSolver&);
	void solveFootRight(const IK::InfoSolver&, IK::ResultSolver&);
	float getLFootCorrection() {
		return lfoot_correction;
	}
	float getRFootCorrection() {
		return rfoot_correction;
	}
	void setLFootCorrection(float val) {
		lfoot_correction = val;
	}
	void setRFootCorrection(float val) {
		rfoot_correction = val;
	}
	void updateSteps(float dist_ground, bool& on_ground);
};

#define SET_ANIM_STATE(skc, state) {if (skc) skc->setState(state);}
#define SET_ANIM_STATE_P(skc, state) {if (skc) skc->setState(state, true);}

#endif
