#ifndef INC_SKELETON_CONTROLLER_H_
#define INC_SKELETON_CONTROLLER_H_

#include "components/comp_base.h"

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
// ------------------- //

template< class TObj >
class CObjectManager;

class TCompSkelController {
protected:
	CHandle owner;
	std::string currentState = "";
	std::string prevState = "";
	bool priority = false;

	void setAnim(std::string anim, bool loop, std::string nextLoop = "");
	void setAction(std::string anim, std::string nextLoop = "");
	void setLoop(std::string anim);
	//animaciones parciales
	void setAnim(std::vector<std::string> anim, bool loop, std::vector<std::string> nextLoop);
	void setAction(std::vector<std::string> anim, std::vector<std::string> nextLoop);
	void setLoop(std::vector<std::string> anim);

	void update();
	//Virtuals
	virtual void myUpdate();

public:
	void setState(std::string state, bool prio = false);

};

#define SET_ANIM_STATE(skc, state) {if (skc) skc->setState(state);}
#define SET_ANIM_STATE_P(skc, state) {if (skc) skc->setState(state, true);}

#endif
