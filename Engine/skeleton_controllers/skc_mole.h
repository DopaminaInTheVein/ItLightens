#ifndef INC_SKC_MOLE_H_
#define INC_SKC_MOLE_H_

#include "skel_controller.h"
#include "player_controllers/player_controller_mole.h"

//Forward Declaration
class TCompCharacterController;
class player_controller_mole;

class SkelControllerMole : public TCompSkelController, public TCompBase {
protected:
	TCompCharacterController * cc;
	player_controller_mole * pc;
	CHandle grabbed;
	VEC3 left_h_target, right_h_target;

	void SetCharacterController();
	void SetPlayerController();
	void myUpdate();

public:
	void grabObject(CHandle);
	VEC3 getGrabLeft();
	VEC3 getGrabRight();
	CHandle getGrabbed();

	bool load(MKeyValue& atts) { return true; }
	//void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo() override;
	void update(float elapsed) { TCompSkelController::update(); }

	void renderInMenu() {}

	//Overload function for handler_manager
	//SkelControllerMole& SkelControllerMole::operator=(SkelControllerMole arg) { return arg; }
};

#endif
