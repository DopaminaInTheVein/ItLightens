#ifndef _BT_SCIENTIST_INC
#define _BT_SCIENTIST_INC

#include "bt_poss.h"
#include "sbb.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
#include "components/comp_msgs.h"
#include "components/comp_workstation.h"
#include "skeleton_controllers/skc_scientist.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "utils/utils.h"
#include "utils/XMLParser.h"
#include <vector>
#include <chrono>
#include <windows.h>

class bt_scientist : public bt_poss, public TCompBase {
	//Enum actions for bot
	enum {
		IDLE = 0,
		WANDER
	};

	//map for debug on ImGui
	static std::map<int, std::string> out;

	//main attributes
	//--------------------------------------
	//float move_speed;
	//float rot_speed;
	//--------------------------------------

	//distance limitations
	//--------------------------------------
	float square_range_action;
	float d_beacon_simple;
	float max_wb_distance;
	float max_beacon_distance;
	//--------------------------------------

	//Map points, TEMP, TODO: look for positions from object_manager
	//--------------------------------------
	VEC3 obj_position;					//Where to move
	std::string wb_to_go_name = "";
	//--------------------------------------

	//Timer counts
	//--------------------------------------
	float waiting_time;
	float t_waitInPos;
	//--------------------------------------

	//Workstation Toggles
	bool busy_state_toggle = true;
	std::string ws_anim;
	VEC3 ws_to_go;
	float ws_yaw;
	float ws_time_waited = 0.f;
	float ws_wait_time;
	float ws_wait_time_offset;

	//general pointers
	//--------------------------------------
	CObjectManager<bt_scientist> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CEntity *myEntity = nullptr;

	SkelControllerScientist * animController;

	int actual_action = IDLE;
	float timerStunt, _timerStunt;
	bool first_update = false;
	//--------------------------------------

	float zmin = 0.0f;
	float zmax = 0.0f;

	CEntity* getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}

	// the nodes
	static map<string, btnode *>tree;
	// the C++ functions that implement node actions, hence, the behaviours
	static map<string, btaction> actions;
	// the C++ functions that implement conditions
	static map<string, btcondition> conditions;
	// the events that will be executed by the decoratos
	static map<string, btevent> events;
	// tree root
	static btnode* root;

	// NPC Virtuals
	TCompTransform * getTransform() override;
	void changeCommonState(std::string) override;
	CHandle getParent() override;
	TCompCharacterController * getCC() override;
public:
	void Init();
	bool getUpdateInfo() override;
	void update(float elapsed);
	void readIniFileAttr();
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	//conditions
	bool workbenchAvailable();
	//toggle conditions
	bool checkBusy();
	//actions
	// movement
	int actionAimToPos();
	int actionMoveToPos();
	// patrol
	int actionNextWpt();
	int actionSeekWpt();
	int actionWaitWpt();
	//toggle actions
	int actionSelectWorkstation();
	int actionGoToWorkstation();
	int actionWaitInWorkstation();
	//Toggle enabling/disabling functions
	void toggleBusyState() {
		setCurrent(NULL);
		busy_state_toggle = !busy_state_toggle;
	}

	//messages function:
	void onEmptyWB(const TMsgWBEmpty& msg);
	void onTakenWB(const TMsgWBTakenByPlayer & msg);
	void onStaticBomb(const TMsgStaticBomb & msg) override;		//need to override to clean old states and reserved objects

	//functions that allow access to the static maps
	map<string, btnode *>* getTree() override {
		return &tree;
	}

	map<string, btaction>* getActions() override {
		return &actions;
	}

	map<string, btcondition>* getConditions() override {
		return &conditions;
	}

	map<string, btevent>* getEvents() override {
		return &events;
	}

	btnode** getRoot() override {
		return &root;
	}

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

	//Movement
	void SetHandleMeInit();
	void SetMyEntity();
	bool aimToTarget(VEC3 target);
	void moveFront(float speed);

	//UI Debug for scientific AI
	void renderInMenu();

	//Virtuals
	PLAYER_TYPE whoAmI() { return SCIENTIST; }
};

#endif