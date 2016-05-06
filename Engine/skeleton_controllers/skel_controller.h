#ifndef INC_SKELETON_CONTROLLER_H_
#define INC_SKELETON_CONTROLLER_H_

// --- Anims State --- //
#define AST_IDLE	"idle"
#define AST_FALL	"jumpland"
#define AST_LAND	"jumplandidle"
#define AST_JUMP	"jump"
#define AST_JUMP2	"jump2"
#define AST_MOVE	"walk"
#define AST_RUN		"run"
#define AST_PREP_SHOOT "attackprepare"
#define AST_SHOOT	"attack"
#define AST_TURNL	"turn90left"
#define AST_TURNR	"turn90right"
// ------------------- //

class SkelController {
protected:
	CHandle owner;
	std::string currentState = "";
	std::string prevState = "";
	float logicYaw = 0.f;

	void setAnim(std::string anim, bool loop, std::string nextLoop = "", float blendTime = 0.f);
	void setAction(std::string anim, std::string nextLoop = "");
	void setLoop(std::string anim);
	float getYaw();
	void setYaw();
	float getAnimFrameDuration(std::string anim_name);
	
	//Virtuals
	virtual void myUpdate();
public:
	float getLogicYaw();
	void init(CHandle new_owner);
	void setState(std::string state);
	void update();

};

#endif
