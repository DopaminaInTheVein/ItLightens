#ifndef INC_AI_SCIENTIFIC_H_
#define	INC_AI_SCIENTIFIC_H_

#include "ai_poss.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "sbb.h"
#include "components\comp_msgs.h"
#include <map>

//forward declarations
//--------------------------------------
class CEntity;

template< class TObj >
class CObjectManager;
//--------------------------------------

class ai_scientific : public ai_poss, public TCompBase {
	//Enum actions for bot
	enum {
		IDLE = 0,
		CREATE_BEACON,
		ADD_BEACON,
		REMOVE_BEACON,
		WANDER
	};

	//map for debug on ImGui
	std::map<int, std::string> out;

	//main attributes
	//--------------------------------------
	float move_speed = 2.0f;
	float rot_speed = 2.0f;
	//--------------------------------------

	//distance limitations
	//--------------------------------------
	float square_range_action = 1.0f;
	float d_epsilon = 0.1f;
	//--------------------------------------

	//Map points, TEMP, TODO: look for positions from object_manager
	//--------------------------------------
	VEC3 obj_position;					//Where to move
	VEC3 beacon_to_go;
	std::string beacon_to_go_name = "";
	std::string wb_to_go_name = "";
	//--------------------------------------

	//Timer counts
	//--------------------------------------
	float waiting_time = 0.0f;
	float t_addBeacon = 2.5f;
	float t_createBeacon = 5.0f;
	float t_removeBeacon = 1.5f;
	float t_waitInPos = 0.0f;
	//--------------------------------------

	//general pointers
	//--------------------------------------
	CObjectManager<ai_scientific> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CEntity *myEntity = nullptr;

	int actual_action = IDLE;
	//--------------------------------------

public:

	ai_scientific() {}		//needed to create obj at load
	void Init() override;
	void init() { Init(); }

	//Overload functions from TCompBase, needed to loop AI Component
	//--------------------------------------
	bool load(MKeyValue& atts);
	void update(float elapsed) { Recalc(); }  //Called from object_manager
	//--------------------------------------

	//Init handlers
	//--------------------------------------
	void SetHandleMeInit();
	void SetMyEntity();
	const void StuntState() override;
	//--------------------------------------

	void CleanStates();

	//messages function:
	void onRemoveBeacon(const TMsgBeaconToRemove& msg);
	void onEmptyBeacon(const TMsgBeaconEmpty& msg);
	void onEmptyWB(const TMsgWBEmpty& msg);
	void onTakenBeacon(const TMsgBeaconTakenByPlayer & msg);
	void onTakenWB(const TMsgWBTakenByPlayer & msg);
	void onStaticBomb(const TMsgStaticBomb & msg) override;		//need to override to clean old states and reserved objects

	//Functions AI Nodes:
	//--------------------------------------
	void Idle();
	void LookForObj();
	void SeekWorkbench();
	void AimToPos();
	void MoveToPos();
	void CreateBeaconFromWB();
	void AddBeacon();
	void RemoveBeacon();
	void WaitInPos();
	//--------------------------------------

	//Patrulla
	//--------------------------------------
	enum KptTipo { Seek, Look };
	struct KeyPoint {
		KptTipo type;
		VEC3 pos;
		float time;
		KeyPoint() : type(KptTipo::Seek), pos(VEC3(0, 0, 0)), time(0) {};
		KeyPoint(VEC3 p) : type(KptTipo::Seek), pos(p), time(0) {};
		KeyPoint(KptTipo t, VEC3 p) : type(t), pos(p), time(0) {};
		KeyPoint(KptTipo t, VEC3 p, float temps) : type(t), pos(p), time(temps) {};
	};
	static map<string, KptTipo > kptTypes;
	std::vector<KeyPoint> keyPoints;
	int curkpt;
	//--------------------------------------

	//UI Debug for scientific AI
	void renderInMenu();

	//Possession
	CEntity* getMyEntity() override;
};

#endif