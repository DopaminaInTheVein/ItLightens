#ifndef INC_SKC_PLAYER_H_
#define INC_SKC_PLAYER_H_

#include "skel_controller.h"

//Forward Declaration
class TCompCharacterController;
class player_controller;



class SkelControllerPlayer : public TCompSkelController, public TCompBase{
protected:
	TCompCharacterController * cc;
	player_controller * pc;

	void SetCharacterController();
	void SetPlayerController();
	void myUpdate();
	void myUpdateIK();

public:
	bool load(MKeyValue& atts) { return true; }
	//void onCreate(const TMsgEntityCreated&);

	bool getUpdateInfo() override;
	void update(float elapsed) { TCompSkelController::update(); }

	void renderInMenu() {}

	//Overload function for handler_manager
	//SkelControllerPlayer& SkelControllerPlayer::operator=(SkelControllerPlayer arg) { return arg; }
};

#endif
