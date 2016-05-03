#ifndef INC_SKC_PLAYER_H_
#define INC_SKC_PLAYER_H_

#include "skel_controller.h"

//Forward Declaration
class TCompCharacterController;



class SkelControllerPlayer : public SkelController {
	TCompCharacterController * cc;

	void SetCharacterController();
	virtual void myUpdate();
};

#endif
