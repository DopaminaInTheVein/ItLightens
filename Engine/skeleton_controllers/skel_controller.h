#ifndef INC_SKELETON_CONTROLLER_H_
#define INC_SKELETON_CONTROLLER_H_

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
