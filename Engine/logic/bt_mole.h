#ifndef _BT_MOLE_INC
#define _BT_MOLE_INC

#include "mcv_platform.h"
#include "bt_poss.h"
#include "sbb.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/comp_name.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include "resources/resources_manager.h"
#include "components/comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/static_mesh.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "utils/utils.h"
#include "utils/XMLParser.h"
#include <vector>
#include <chrono>
#include <windows.h>

//Anim
class SkelControllerMole;

class bt_mole : public bt_poss, public TCompBase {
	// Main attributes
	//float speed;
	//float rotation_speed;
	float distMaxToBox;
	float rechTime;

	int towptbox;
	int towptleave;
	float timerStunt, _timerStunt;

	float timeWaiting;
	float TIME_WAIT_MOLE;

	bool carryingBox = false;

	vector<VEC3> fixedWpts;
	int curwpt;

	int currToRechargePoint = 0;
	int pointsToRechargePoint;
	VEC3 rechargePoint;

	CObjectManager<bt_mole> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CHandle myBox = CHandle();
	CEntity *myEntity = nullptr;

	TCompTransform* transform;
	bool first_update = false;

	void SetMyEntity();

	CEntity * getEntityPointer(int i) {
		CHandle han = SBB::readHandlesVector("wptsBoxes")[i];
		CEntity * ent = han;
		return ent;
	}

	CEntity* getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}
	//bool isBoxAtLeavePoint(VEC3 posBox);

	//Cambio Anim
	SkelControllerMole * animController;

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

	//NPC virtuals
	TCompTransform * getTransform() override;
	void changeCommonState(std::string) override;
	CHandle getParent() override;
	TCompCharacterController * getCC() override;
public:
	void Init();
	bool getUpdateInfo() override;
	void update(float elapsed);
	void readIniFileAttr();
	void renderInMenu();
	//bool checkBoxes();
	//actions
	/*
	int actionFollowBoxWpt();
	int actionGrabBox();
	int actionFollowNextBoxLeavepointWpt();
	int actionUngrabBox();
	*/
	int actionSeekWpt();
	int actionNextWpt();
	int actionWaitWpt();

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

	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);

	bool aimToTarget(VEC3 target);
	void moveFront(float movement_speed);

	//Virtuals
	PLAYER_TYPE whoAmI() { return MOLE; }
};

#endif