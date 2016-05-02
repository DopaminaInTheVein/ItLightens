#ifndef INC_SKELETON_CONTROLLER_H_
#define INC_SKELETON_CONTROLLER_H_

// --- Anims State --- //
#define AST_IDLE	"idle"
#define AST_FALL	"fall"
#define AST_JUMP	"jump"
#define AST_JUMP2	"jump2"
#define AST_MOVE	"walk"
#define AST_RUN		"run"
#define AST_SHOOT	"shoot"
// ------------------- //

class SkelController {
protected:
	CHandle owner;
	std::string currentState = "";
	std::string prevState = "";

	void setAnim(std::string anim, bool loop);
	void setAction(std::string anim);
	void setLoop(std::string anim);
	
	//Virtuals
	virtual void myUpdate();
public:
	void init(CHandle new_owner);
	void setState(std::string state);
	void update();

};

#endif
