#ifndef INC_AI_WORKBENCH_H
#define	INC_AI_WORKBENCH_H


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

class workbench_controller : public aicontroller, public TCompBase {
	std::map<int, std::string> out;

	//main attributes
	//--------------------------------------
	const float range = 7.5f;		//range sonar
	const float rot_speed_sonar = 8.0f;	
	const float rot_speed_disable = 2.0f;		
	//--------------------------------------

	//general info
	//--------------------------------------

	int id_wb = 0;

	std::string full_name = "";

	CObjectManager<workbench_controller>		*om = nullptr;
	CHandle								 myHandle;
	CHandle								 myParent;
	CEntity								*myEntity = nullptr;
	//--------------------------------------

	//messages types
	TMsgWBEmpty				msg_empty;
	TMsgWBTakenByPlayer		msg_taken;


public:

	static int id_curr_max_wb;	//count ids, id++ each beacon

									//states wb
	enum {
		INACTIVE = 0,
		INACTIVE_TAKEN,
		BUSY
	};

	workbench_controller() {}		//needed to create obj at load
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
	void SendMessageTaken();
	//--------------------------------------

	//Functions AI Nodes:
	//--------------------------------------
	void Idle();
	void Inactive();		//beacon empty
	void Busy();
	//--------------------------------------

	void renderInMenu();
	workbench_controller& workbench_controller::operator=(workbench_controller arg) { return arg; }
};

#endif