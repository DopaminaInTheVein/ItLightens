#ifndef INC_AI_CAM_H_
#define INC_AI_CAM_H_

#include "aicontroller.h"
#include "sbb.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"

//forward declarations
//--------------------------------------
class CEntity;

template< class TObj >
class CObjectManager;
//--------------------------------------

class ai_cam : public aicontroller, public TCompBase {
	static std::map<int, std::string> out;

	//main attributes
	//--------------------------------------
	float range = 2.0f;		// range sonar
	float width = 1.0f; ;		// width sonar
	float rot_speed_sonar = 3.0f;
	int id_camera = 0;
	bool rotatingR = true;
	float rotatedTo = 0.0f;
	float maxRot = deg2rad(90.0f);
	float distToFloor = 0.0f;
	std::string full_name = "";
	std::string camera_light = "";
	CObjectManager<ai_cam> * om = nullptr;

	CHandle myHandle;
	CHandle myParent;
	CEntity	*myEntity = nullptr;
	float max_idle_waiting = 5.0f;
	float idle_wait;
protected:
	// the states, as maps to functions
	static map<string, statehandler>statemap;

public:

	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}

	ai_cam() {}		//needed to create obj at load
	void Init() override;
	void init() { Init(); }
	bool load(MKeyValue& atts);
	bool playerInRange();
	float getRange() { return range; }
	float getWidth() { return width; }
	float getDistToFloor() { return distToFloor; }
	//Overload functions from TCompBase, needed to loop AI Component
	//--------------------------------------
	void update(float elapsed) { if (!isInRoom(myParent))return; Recalc(); }  //Called from object_manager

	void SetHandleMeInit();
	void SetMyEntity();
	//--------------------------------------

	//Functions AI Nodes:
	//--------------------------------------
	void Idle();
	void RotatingLeft();	// camera rotating left,  looking for player
	void RotatingRight();	// camera rotating right, looking for player
	void AimPlayer();     // camera reached player and aims him

	void renderInMenu();
};

#endif