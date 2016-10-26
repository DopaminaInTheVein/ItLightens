#include "mcv_platform.h"
#include "bt_mole.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_physics.h"
#include "comp_box.h"
#include "app_modules\logic_manager\logic_manager.h"

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_MOLE_BT(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_MOLE_BT_P(state) SET_ANIM_STATE_P(animController, state)

map<string, btnode *> bt_mole::tree = {};
map<string, btaction> bt_mole::actions = {};
map<string, btcondition> bt_mole::conditions = {};
map<string, btevent> bt_mole::events = {};
btnode* bt_mole::root = nullptr;

void bt_mole::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_mole")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields = readIniAtrData(file_ini, "bt_mole");

			readNpcIni(fields);

			//assignValueToVar(speed, fields);
			//assignValueToVar(rotation_speed, fields);
			//rotation_speed = deg2rad(rotation_speed);
			assignValueToVar(distMaxToBox, fields);
			assignValueToVar(rechTime, fields);
			assignValueToVar(TIME_WAIT_MOLE, fields);
		}
	}
}

//NPC virtuals
TCompTransform * bt_mole::getTransform()
{
	return GETH_MY(TCompTransform);
}
void bt_mole::changeCommonState(std::string state)
{
	SET_ANIM_MOLE_BT(state);
}
CHandle bt_mole::getParent()
{
	return MY_OWNER;
}
TCompCharacterController * bt_mole::getCC()
{
	return GETH_MY(TCompCharacterController);
}

void bt_mole::Init()
{
	getUpdateInfoBase(CHandle(this).getOwner());
	// read main attributes from file
	readIniFileAttr();

	myHandle = CHandle(this);
	myParent = myHandle.getOwner();

	if (tree.empty()) {
		addBtPossStates();
		addChild("possessable", "mole", PRIORITY, (btcondition)&bt_mole::npcAvailable, NULL);
		/*
		addChild("mole", "movebox", SEQUENCE, (btcondition)&bt_mole::checkBoxes, NULL);
		addChild("movebox", "followbox", ACTION, NULL, (btaction)&bt_mole::actionFollowBoxWpt);
		addChild("movebox", "grabbox", ACTION, NULL, (btaction)&bt_mole::actionGrabBox);
		addChild("movebox", "carryboxfollowwpt", ACTION, NULL, (btaction)&bt_mole::actionFollowNextBoxLeavepointWpt);
		addChild("movebox", "ungrabbox", ACTION, NULL, (btaction)&bt_mole::actionUngrabBox);*/
		// patrol states
		addChild("mole", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "nextWpt", ACTION, NULL, (btaction)&bt_mole::actionNextWpt);
		addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_mole::actionSeekWpt);
		addChild("patrol", "waitwpt", ACTION, NULL, (btaction)&bt_mole::actionWaitWpt);
	}

	towptbox = -1;
	towptleave = -1;
	// current wpt
	curwpt = 0;
	CEntity * myParentE = myParent;
	TCompTransform * myParentT = myParentE->get<TCompTransform>();
	rechargePoint = myParentT->getPosition();

	timeWaiting = 0;

	____TIMER_REDEFINE_(timerStunt, 15);
	if (animController) {
		animController->setState(AST_IDLE);
	}
	SET_ANIM_MOLE_BT(AST_IDLE);

	initParent();
}

bool bt_mole::getUpdateInfo()
{
	animController = GETH_MY(SkelControllerMole);
	return true;
}

void bt_mole::update(float elapsed) {
	// Update transforms
	SetMyEntity();
	if (!myEntity) return;
	if (first_update && !isInRoom(myParent))return;
	first_update = true;

	transform = myEntity->get<TCompTransform>();
	// If we become possessed, reset the tree and stop all actions
	if (possessing)
		setCurrent(NULL);
	if (possessed) return;
	if (stunned)
		SET_ANIM_MOLE_BT(AST_STUNNED);
	updateStuck();
	updateTalk("Mole", CHandle(this).getOwner());
	Recalc();
}

// Sets the entity
void bt_mole::SetMyEntity() {
	myEntity = myParent;
}

// Loading the wpts
#define WPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d_%s", index, nameSufix);

bool bt_mole::load(MKeyValue& atts) {
	int n = atts.getInt("wpts_size", 0);
	fixedWpts.resize(n);
	for (int i = 0; i < n; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		fixedWpts[i] = atts.getPoint(atrPos);
	}
	pointsToRechargePoint = atts.getInt("max_patrol", 5);
	load_bt(atts);
	return true;
}

bool bt_mole::save(std::ofstream& os, MKeyValue& atts)
{
	int n = fixedWpts.size();
	atts.put("wpts_size", n);
	for (int i = 0; i < n; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		atts.put(atrPos, fixedWpts[i]);
	}

	save_bt(os, atts);
	return true;
}

// conditions

// actions
int bt_mole::actionNextWpt() {
	PROFILE_FUNCTION("mole: actionnextwpt");
	if (!myParent.isValid()) return false;
	if (fixedWpts.size() == 0) return false;
	SET_ANIM_MOLE_BT(AST_TURN);
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = fixedWpts[curwpt];
	//If we are already there, we continue
	if (simpleDistXZ(myPos, dest) < DIST_REACH_PNT)
		return OK;
	//Look to waypoint
	if (turnTo(dest)) {
		return OK;
	}
	else {
		return STAY;
	}
}

int bt_mole::actionSeekWpt() {
	PROFILE_FUNCTION("mole: actionseekwpt");
	if (!myParent.isValid()) return 0;
	VEC3 myPos = getTransform()->getPosition();
	VEC3 dest = fixedWpts[curwpt];
	//reach waypoint?
	if (simpleDistXZ(myPos, dest) < DIST_REACH_PNT) {
		curwpt = (curwpt + 1) % fixedWpts.size();
		path_found = false;
		return OK;
	}
	else {
		if (!path_found) {
			path_found = getPath(myPos, dest);
		}
		SET_ANIM_MOLE_BT(AST_MOVE);
		goTo(dest);
		return STAY;
	}

	path_found = false;
	return OK;
}

int bt_mole::actionWaitWpt() {
	PROFILE_FUNCTION("mole: actionwaitwpt");
	if (!myParent.isValid()) return false;
	SET_ANIM_MOLE_BT(AST_IDLE);
	stuck = false;
	stuck_time = 0.f;
	if (timeWaiting > TIME_WAIT_MOLE) {
		timeWaiting = 0;
		return OK;
	}
	else {
		timeWaiting += getDeltaTime();
		return STAY;
	}
}

bool bt_mole::aimToTarget(VEC3 target) {
	float delta_yaw = transform->getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		transform->getAngles(&yaw, &pitch);
		if (delta_yaw > 0.15) {
			transform->setAngles(yaw + delta_yaw*SPEED_ROT*getDeltaTime(), pitch);
		}
		else {
			transform->setAngles(yaw + delta_yaw, pitch);
		}
		return false;
	}
	else {
		return true;
	}
}

void bt_mole::moveFront(float movement_speed) {
	VEC3 front = transform->getFront();
	//VEC3 position = transform->getPosition();
	TCompCharacterController *cc = myEntity->get<TCompCharacterController>();
	float dt = getDeltaTime();
	cc->AddMovement(VEC3(front.x*movement_speed*dt, 0.0f, front.z*movement_speed*dt));
}

void bt_mole::renderInMenu()
{
	if (bt::current) ImGui::Text("NODE: %s", bt::current->getName().c_str());
	else ImGui::Text("NODE: %s", "???\n");
	//if (fixedWpts.size() >= 0) {
	//	ImGui::Text("Next patrol: %d, Pos: (%f,%f,%f)"
	//		, curwpt
	//		, VEC3_VALUES(fixedWpts[curwpt])
	//	);
	//}
}