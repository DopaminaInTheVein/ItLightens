#ifndef INC_SKC_SCIENTIST_H_
#define INC_SKC_SCIENTIST_H_

#include "skel_controller.h"

//Forward Declaration
class TCompCharacterController;
class player_controller_cientifico;



class SkelControllerScientist : public TCompSkelController, public TCompBase{
protected:
	TCompCharacterController * cc;
	player_controller_cientifico * pc;

	void SetCharacterController();
	void SetPlayerController();
	void myUpdate();

public:
	bool load(MKeyValue& atts) { return true; }
	//void onCreate(const TMsgEntityCreated&);

	bool getUpdateInfo() override;
	void update(float elapsed) { TCompSkelController::update(); }

	void renderInMenu() {}

	//Overload function for handler_manager
	//SkelControllerScientist& SkelControllerScientist::operator=(SkelControllerScientist arg) { return arg; }
};

#endif