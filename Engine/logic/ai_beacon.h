#ifndef INC_BEACON_H_
#define INC_BEACON_H_

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

class beacon_controller : public aicontroller, public TCompBase {
	std::map<int, std::string> out;

	//main attributes
	//--------------------------------------
	const float range = 7.5f;		//range sonar
	const float rot_speed_sonar = 8.0f;		//TODO: mesh change
	const float rot_speed_disable = 2.0f;		//TODO: mesh change
	//--------------------------------------

	//general info
	//--------------------------------------
	//CHandle *target_h;		//TODO: player target

	int id_beacon = 0;

	std::string full_name = "";

	CObjectManager<beacon_controller>	*om = nullptr;
	CHandle								 myHandle;
	CHandle								 myParent;
	CEntity								*myEntity = nullptr;
	//--------------------------------------

	//messages types
	TMsgBeaconEmpty		msg_empty;
	TMsgBeaconToRemove	msg_remove;

	//Timer counts
	//--------------------------------------
	float			t_waiting = 0.0f;
	const float		t_max_sonar = 10.0f;
	const float		t_max_empty = 7.5f;
	const float		t_max_disable = 15.0f;	//not used for now
	//--------------------------------------

public:

	static int id_curr_max_beacons;	//count ids, id++ each beacon

	//states beacon
	enum {
		INACTIVE = 0,
		INACTIVE_TAKEN,
		SONAR,
		BUSY,
		TO_REMOVE,
		TO_REMOVE_TAKEN
	};

	beacon_controller() {}		//needed to create obj at load
	void Init() override;
	void init() { Init(); }

	//Overload functions from TCompBase, needed to loop AI Component
	//--------------------------------------
	void update(float elapsed) { Recalc(); }  //Called from object_manager
	//--------------------------------------

	//Init handlers
	//--------------------------------------
	void SetHandleMeInit();
	void SetMyEntity();
	void SendMessageEmpty();
	void SendMessageRemove();
	//--------------------------------------

	//Functions AI Nodes:
	//--------------------------------------
	void Idle();
	void WaitInactive();
	void Inactive();		//beacon empty
	void ActiveSonar();
	void ActiveNothing();	//beacon busy, but do nothing
	void WaitToRemove();
	//--------------------------------------

	void renderInMenu();
	beacon_controller& beacon_controller::operator=(beacon_controller arg) { return arg; }
};

#endif