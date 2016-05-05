#ifndef INC_SKC_PLAYER_H_
#define INC_SKC_PLAYER_H_

#include "skel_controller.h"

//Forward Declaration
class TCompCharacterController;
class player_controller;



class SkelControllerPlayer : public SkelController {
	TCompCharacterController * cc;
	player_controller * pc;

	void SetCharacterController();
	void SetPlayerController();
	virtual void myUpdate();
};

#endif
