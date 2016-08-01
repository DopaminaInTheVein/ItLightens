#ifndef INC_SKC_MOLE_H_
#define INC_SKC_MOLE_H_

#include "skel_controller.h"
#include "player_controllers/player_controller_mole.h"

#define SK_MOLE_TIME_TO_GRAB 0.5f
#define SK_MOLE_TIME_TO_UNGRAB 0.1f

//Forward Declaration
class TCompCharacterController;
class player_controller_mole;

class SkelControllerMole : public TCompSkelController, public TCompBase {
protected:
	TCompCharacterController * cc;
	player_controller_mole * pc;
	CHandle grabbed;
	CHandle grabbedPila;
	CHandle pushed;
	VEC3 left_h_target, right_h_target, front_h_dir;
	VEC3 left_h_normal, right_h_normal;

	void SetCharacterController();
	void SetPlayerController();
	void myUpdate();
	void updateGrab();
	void updatePila();
	void updateGrabPoints();
	void updatePushPoints();
	bool isMovingBox();

public:
	void grabObject(CHandle);
	void grabPila(CHandle);
	void pushObject(CHandle);
	void ungrabObject(); // Funcion para soltarlo
	void ungrabPila(); // Funcion para soltarlo
	void unpushObject(); // Funcion para soltarlo
	void removeGrab() { grabbed = CHandle(); pushed = CHandle();  } //"Private" func
	void removePila() { grabbedPila = CHandle(); } //"Private" func
	VEC3 getGrabLeft();
	VEC3 getGrabRight();
	VEC3 getGrabNormalLeft();
	VEC3 getGrabNormalRight();
	VEC3 getGrabFrontDir();
	CHandle getGrabbed();
	CHandle getGrabbedPila();

	bool load(MKeyValue& atts) { return true; }
	//void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo() override;
	void update(float elapsed) { TCompSkelController::update(); }

	void renderInMenu() { TCompSkelController::renderInMenu(); }

	//Overload function for handler_manager
	//SkelControllerMole& SkelControllerMole::operator=(SkelControllerMole arg) { return arg; }
};

#endif