#ifndef INC_MAGNET_DOOR_H_
#define INC_MAGNET_DOOR_H_

#include "components/components.h"
#include "geometry/geometry.h"
#include "logic/polarity.h"


class magnet_door : public TCompBase {
	enum eMagnetismBehaviour {
		MB_OPENING,
		MB_CLOSING,
		MB_NONE
	};

	eMagnetismBehaviour mb = MB_NONE; //Magnetism Behaviour predomines. If MB_NONE --> openedLogic
	bool locked;
	bool moving;	//false false = opened, false true = opening
					//true false = closed, true true = closing 

	VEC3 targetOpened, targetClosed;
	pols polarity;
	float speedOpening;
	float speedClosing;
public:
	void update(float elapsed);
	void onSetPolarity(const TMsgSetPolarity&);
	void onSetLocked(const TMsgSetLocked&);
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
};
/*
//forward declarations
//--------------------------------------
class CEntity;

template< class TObj >
class CObjectManager;
//--------------------------------------

class beacon_controller : public aicontroller, public TCompBase {
	static std::map<int, std::string> out;


	//main attributes
	//--------------------------------------
	float range;		//range sonar
	float rot_speed_sonar;		//TODO: mesh change
	float rot_speed_disable;		//TODO: mesh change
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
	TMsgBeaconEmpty				msg_empty;
	TMsgBeaconToRemove			msg_remove;
	TMsgBeaconTakenByPlayer		msg_taken;

	//Timer counts
	//--------------------------------------
	float		t_waiting;
	float		t_max_sonar;
	float		t_max_empty;
	float		t_max_disable;	
	//--------------------------------------

protected:
	// the states, as maps to functions
	static map<string, statehandler>statemap;

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

	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}

	beacon_controller() {}		//needed to create obj at load
	void Init() override;
	void init() { Init(); }
	void readIniFileAttr();

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
	void SendMessageTaken();
	//--------------------------------------

	//Functions AI Nodes:
	//--------------------------------------
	void Idle();
	void WaitInactive();
	void Inactive();		//beacon empty
	void ActiveSonar();
	void ActiveNothing();	//beacon busy, but do nothing
	void WaitToRemoveSonar();
	void WaitToRemoveNothing();
	void WaitToRemove();
	//--------------------------------------

	//Messages:
	//--------------------------------------
	void onPlayerAction(TMsgBeaconBusy& msg);
	//--------------------------------------

	void renderInMenu();
};
*/

#endif
