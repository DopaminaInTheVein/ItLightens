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
		addChild("mole", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "lookForWpt", ACTION, NULL, (btaction)&bt_mole::actionLookForWpt);
		addChild("patrol", "followPathToWpt", ACTION, NULL, (btaction)&bt_mole::actionFollowPathToWpt);
		addChild("patrol", "reachedPoint", ACTION, NULL, (btaction)&bt_mole::actionEndPathToWpt);
	}

	towptbox = -1;
	towptleave = -1;
	// current wpt
	curwpt = 0;
	CEntity * myParentE = myParent;
	TCompTransform * myParentT = myParentE->get<TCompTransform>();
	rechargePoint = myParentT->getPosition();
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
	if (!isInRoom(myParent))return;
	transform = myEntity->get<TCompTransform>();
	// If we become possessed, reset the tree and stop all actions
	if (possessing)
		setCurrent(NULL);
	if (possessed) return;
	if (stunned)
		SET_ANIM_MOLE_BT(AST_STUNNED);
	updateStuck();
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

// actions
int bt_mole::actionLookForWpt() {
	if (!SBB::readBool("navmesh")) {
		return STAY;
	}
	SET_ANIM_MOLE_BT(AST_IDLE);
	if (fixedWpts.size() <= 0) return OK;
	VEC3 front = transform->getFront();
	VEC3 target = fixedWpts[curwpt];
	if (pointsToRechargePoint == currToRechargePoint) {
		target = rechargePoint;
	}
	VEC3 initial = transform->getPosition();
	getPath(initial, target);
	return OK;
}

int bt_mole::actionFollowPathToWpt() {
	if (!SBB::readBool("navmesh")) {
		setCurrent(NULL);
		return OK;
	}
	if (fixedWpts.size() <= 0) return OK;

	VEC3 target = fixedWpts[curwpt];
	VEC3 my_pos = getTransform()->getPosition();
	float distance_to_point = simpleDistXZ(my_pos, target);

	if (turnTo(target)) {
		// if we didn't reach the point
		if (distance_to_point > DIST_REACH_PNT) {
			getPath(my_pos, target);
			SET_ANIM_MOLE_BT(AST_MOVE);
			goTo(target);
			return STAY;
		}
		return OK;
	}
	else {
		SET_ANIM_MOLE_BT(AST_TURN);
		return STAY;
	}
	//if (pointsToRechargePoint == currToRechargePoint) {
	//	target = rechargePoint;
	//}
	//while (totalPathWpt > 0 && currPathWpt < totalPathWpt && fabsf(squaredDistXZ(pathWpts[currPathWpt], transform->getPosition())) < 0.5f) {
	//	++currPathWpt;
	//}
	//if (currPathWpt < totalPathWpt) {
	//	target = pathWpts[currPathWpt];
	//}
	//VEC3 npcPos = transform->getPosition();
	//VEC3 npcFront = transform->getFront();
	//if (needsSteering(npcPos + npcFront, transform, SPEED_ROT, myParent)) {
	//	SET_ANIM_MOLE_BT(AST_MOVE);
	//	moveFront(SPEED_WALK);
	//	return STAY;
	//}
	//else if (!transform->isHalfConeVision(target, deg2rad(5.0f))) {
	//	SET_ANIM_MOLE_BT(AST_IDLE);
	//	aimToTarget(target);
	//	return STAY;
	//}
	//else {
	//	float distToWPT = squaredDistXZ(target, transform->getPosition());
	//	if (fabsf(distToWPT) > 0.5f && currPathWpt < totalPathWpt || fabsf(distToWPT) > 6.0f) {
	//		SET_ANIM_MOLE_BT(AST_MOVE);
	//		moveFront(SPEED_WALK);
	//		return STAY;
	//	}
	//	else {
	//		return OK;
	//	}
	//}
}

int bt_mole::actionEndPathToWpt() {
	stuck = false;
	stuck_time = 0.f;
	static float recharging = 0.0f;
	recharging += getDeltaTime();
	SET_ANIM_MOLE_BT(AST_IDLE);
	if (pointsToRechargePoint == currToRechargePoint && rechTime > recharging) {
		return STAY;
	}
	else if (1.0f > recharging) {
		return STAY;
	}
	recharging = 0.0f;
	setCurrent(NULL);
	int lastcurwpt = curwpt;
	if (fixedWpts.size() > 0) {
		while (lastcurwpt == curwpt) {
			curwpt = rand() % fixedWpts.size();
		}
	}
	currToRechargePoint++;
	if (pointsToRechargePoint < currToRechargePoint) {
		currToRechargePoint = 1;
	}
	return OK;
}
/*
bool bt_mole::checkBoxes() {
  bool found = false;
  float minDistanceToBox = distMaxToBox;

  if (SBB::readHandlesVector("wptsBoxes").size() > 0) {
	string key_final = "";
	float higher = -999.9f;
	VEC3 initial = transform->getPosition(), destiny;

	for (int i = 0; i < SBB::readHandlesVector("wptsBoxes").size(); i++) {
	  CEntity * entTransform = this->getEntityPointer(i);
	  if (!entTransform) continue;
	  TCompTransform * transformBox = entTransform->get<TCompTransform>();
	  TCompName * nameBox = entTransform->get<TCompName>();
	  VEC3 wpt = transformBox->getPosition();
	  float disttowpt = simpleDistXZ(wpt, transform->getPosition());
	  string key = nameBox->name;
	  if (!SBB::readBool(key) && !isBoxAtLeavePoint(wpt) && (disttowpt < minDistanceToBox + 2 && wpt.y > higher)) {
		towptbox = i;
		higher = wpt.y;
		minDistanceToBox = disttowpt;
		key_final = key;
		found = true;
		destiny = wpt;
	  }
	}

	if (found) {
	  SBB::postBool(key_final, true);
	  SBB::postMole(key_final, this);

	  getPath(initial, destiny);
	  //ChangePose(pose_run_route);
	  SET_ANIM_MOLE_BT(AST_RUN);
	  return true;
	}
  }
  return false;
}

int bt_mole::actionFollowBoxWpt()
{
  if (towptbox > -1) {
	CEntity * entTransform = this->getEntityPointer(towptbox);
	TCompTransform * transformBox = entTransform->get<TCompTransform>();
	VEC3 boxpos = transformBox->getPosition();
	while (totalPathWpt > 0 && currPathWpt < totalPathWpt && fabsf(squaredDistXZ(pathWpts[currPathWpt], transform->getPosition())) < 0.5f) {
	  ++currPathWpt;
	}
	if (currPathWpt < totalPathWpt) {
	  boxpos = pathWpts[currPathWpt];
	}
	VEC3 npcPos = transform->getPosition();
	VEC3 npcFront = transform->getFront();
	if (needsSteering(npcPos + npcFront, transform, rotation_speed, myParent, entTransform)) {
	  moveFront(speed);
	  return STAY;
	}
	else {
	  float distToWPT = squaredDistXZ(boxpos, transform->getPosition());
	  if (!transform->isHalfConeVision(boxpos, deg2rad(5.0f))) {
		aimToTarget(boxpos);
		return STAY;
	  }
	  else if (fabsf(distToWPT) > 0.5f && currPathWpt < totalPathWpt || fabsf(distToWPT) > 6.0f) {
		moveFront(speed);
		return STAY;
	  }
	  else {
		//ChangePose(pose_box_route);
		logic_manager->throwEvent(logic_manager->OnPickupBox, "");
		return OK;
	  }
	}
  }
  return KO;
}

int bt_mole::actionGrabBox() {
  if (towptbox > -1) {
	SetMyEntity();
	if (!myEntity) return -1;
	myBox = SBB::readHandlesVector("wptsBoxes")[towptbox];
	CEntity* box = myBox;
	TCompTransform* box_t = box->get<TCompTransform>();
	TCompPhysics* box_p = box->get<TCompPhysics>();
	VEC3 posbox = transform->getPosition();
	posbox.y += 4;
	box_p->setKinematic(true);
	box_p->setPosition(posbox, box_t->getRotation());
	carryingBox = true;
	if (myBox.isValid()) {
	  CEntity *e_mole = myEntity;
	  TCompTransform *t_mole = e_mole->get<TCompTransform>();
	  CEntity *e_box = myBox;
	  TCompBox *box = e_box->get<TCompBox>();
	  VEC3 destiny = box->GetLeavePoint();
	  VEC3 initial = t_mole->getPosition();
	  getPath(initial, destiny);
	  return OK;
	}
  }
  return KO;
}

int bt_mole::actionFollowNextBoxLeavepointWpt() {
  CEntity *e_box = myBox;
  TCompBox *cbox = e_box->get<TCompBox>();
  VEC3 leavepos = cbox->GetLeavePoint();
  CEntity * box = this->getEntityPointer(towptbox);
  TCompTransform * transformBox = box->get<TCompTransform>();
  while (totalPathWpt > 0 && currPathWpt < totalPathWpt && fabsf(squaredDistXZ(pathWpts[currPathWpt], transform->getPosition())) < 0.5f) {
	++currPathWpt;
  }
  if (currPathWpt < totalPathWpt) {
	leavepos = pathWpts[currPathWpt];
  }

  VEC3 npcPos = transform->getPosition();
  VEC3 npcFront = transform->getFront();
  if (needsSteering(npcPos + npcFront, transform, rotation_speed, myParent)) {
	moveFront(speed);
	return STAY;
  }
  else {
	float distToWPT = squaredDistXZ(leavepos, transform->getPosition());
	if (!transform->isHalfConeVision(leavepos, deg2rad(5.0f))) {
	  aimToTarget(leavepos);
	  return STAY;
	}
	else if (fabsf(distToWPT) > 0.5f && currPathWpt < totalPathWpt || fabsf(distToWPT) > 1.0f) {
	  moveFront(speed);
	  TCompPhysics *enBoxP = box->get<TCompPhysics>();
	  VEC3 posbox = transform->getPosition();
	  posbox.y += 4;
	  enBoxP->setPosition(posbox, transformBox->getRotation());
	  return STAY;
	}
	else {
	  logic_manager->throwEvent(logic_manager->OnLeaveBox, "");
	  return OK;
	}
  }
}

int bt_mole::actionUngrabBox() {
  CEntity * enBox = SBB::readHandlesVector("wptsBoxes")[towptbox];
  CEntity *e_box = myBox;
  TCompBox *cbox = e_box->get<TCompBox>();
  VEC3 leavepos = cbox->GetLeavePoint();
  TCompTransform * enBoxT = enBox->get<TCompTransform>();
  TCompName * nameBox = enBox->get<TCompName>();
  VEC3 posLeave = cbox->GetLeavePoint();
  posLeave.y += 2;
  TCompPhysics *enBoxP = enBox->get<TCompPhysics>();
  enBoxP->setKinematic(false);
  enBoxP->setPosition(posLeave, enBoxT->getRotation());

  SBB::postBool(nameBox->name, false);
  carryingBox = false;
  SET_ANIM_MOLE_BT(AST_IDLE); //Leave box!

  TMsgLeaveBox msg;
  myBox.sendMsg(msg);

  return OK;
}
*/
void bt_mole::_actionBeforePossession() {
	/*if (towptbox > -1 && carryingBox) {
		vector<CHandle> newPointerVec = SBB::readHandlesVector("wptsBoxes");
		CEntity * en = newPointerVec[towptbox];
		TCompName * nameBox = en->get<TCompName>();
		string key = nameBox->name;

		bt_mole * mole = SBB::readMole(key);
		CEntity * mole_e = mole->getMyEntity();
		TCompName * mole_e_n = mole_e->get<TCompName>();

		CEntity * currmole_e = getMyEntity();
		TCompName * currmole_e_n = currmole_e->get<TCompName>();

		if (mole_e_n->name == currmole_e_n->name) {
			TCompTransform* p_t = mole_e->get<TCompTransform>();
			TCompTransform* b_t = en->get<TCompTransform>();
			VEC3 posboxIni = b_t->getPosition();
			VEC3 posbox;
			posbox.x = posboxIni.x + p_t->getFront().x * 3;
			posbox.y = posboxIni.y - 2;
			posbox.z = posboxIni.z + p_t->getFront().z * 3;
			float angle = 0.0f;
			//TODO PHYSX OBJECT

			while (!b_t->executeMovement(posbox)) {
			  angle += 0.1f;
			  posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
			  posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
			}
			SBB::postBool(key, false);
			carryingBox = false;
		}
	}*/
}

void bt_mole::_actionWhenStunt() {
	/*if (towptbox > -1 && carryingBox) {
		vector<CHandle> newPointerVec = SBB::readHandlesVector("wptsBoxes");
		CEntity * en = newPointerVec[towptbox];
		TCompName * nameBox = en->get<TCompName>();
		string key = nameBox->name;
		if (SBB::readBool(key)) {
			bt_mole * mole = SBB::readMole(key);
			CEntity * mole_e = mole->getMyEntity();
			TCompName * mole_e_n = mole_e->get<TCompName>();

			CEntity * currmole_e = getMyEntity();
			TCompName * currmole_e_n = currmole_e->get<TCompName>();

			if (mole_e_n->name == currmole_e_n->name) {
				TCompTransform* p_t = mole_e->get<TCompTransform>();
				TCompTransform* b_t = en->get<TCompTransform>();
				VEC3 posboxIni = b_t->getPosition();
				VEC3 posbox;
				posbox.x = posboxIni.x + p_t->getFront().x * 3;
				posbox.y = posboxIni.y - 2;
				posbox.z = posboxIni.z + p_t->getFront().z * 3;
				float angle = 0.0f;
				//TODO PHYSX OBJECT

				while (!b_t->executeMovement(posbox)) {
				  angle += 0.1f;
				  posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
				  posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
				}

				SBB::postBool(key, false);
				carryingBox = false;
			}
		}
	}*/
}
/*
bool bt_mole::isBoxAtLeavePoint(VEC3 posBox) {
	if (SBB::readHandlesVector("wptsBoxLeavePoint").size() > 0) {
		for (int i = 0; i < SBB::readHandlesVector("wptsBoxLeavePoint").size(); i++) {
			CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[i];
			if (!wptbleave) continue;
			TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
			VEC3 wpt = wptbleavetransform->getPosition();
			float disttowpt = squaredDist(wpt, posBox);
			if (disttowpt < 4.0f) {
				return true;
			}
		}
	}
	return false;
}
*/
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
	VEC3 position = transform->getPosition();
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