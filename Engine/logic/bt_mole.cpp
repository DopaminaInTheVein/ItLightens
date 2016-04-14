#include "mcv_platform.h"
#include "bt_mole.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_physics.h"
#include "components\comp_box.h"
#include "app_modules\logic_manager\logic_manager.h"

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

			assignValueToVar(speed, fields);
			assignValueToVar(rotation_speed, fields);
			rotation_speed = deg2rad(rotation_speed);
			assignValueToVar(distMaxToBox, fields);
		}
	}
}

void bt_mole::Init()
{
	// read main attributes from file
	readIniFileAttr();

	myHandle = CHandle(this);
	myParent = myHandle.getOwner();

	if (tree.empty()) {
		addBtPossStates();
		addChild("possessable", "mole", PRIORITY, (btcondition)&bt_mole::npcAvailable, NULL);
		addChild("mole", "movebox", SEQUENCE, (btcondition)&bt_mole::checkBoxes, NULL);
		addChild("movebox", "followbox", ACTION, NULL, (btaction)&bt_mole::actionFollowBoxWpt);
		addChild("movebox", "grabbox", ACTION, NULL, (btaction)&bt_mole::actionGrabBox);
		addChild("movebox", "carryboxfollowwpt", ACTION, NULL, (btaction)&bt_mole::actionFollowNextBoxLeavepointWpt);
		addChild("movebox", "ungrabbox", ACTION, NULL, (btaction)&bt_mole::actionUngrabBox);
		addChild("mole", "patrol", SEQUENCE, NULL, NULL);
		addChild("patrol", "lookForWpt", ACTION, NULL, (btaction)&bt_mole::actionLookForWpt);
		addChild("patrol", "followPathToWpt", ACTION, NULL, (btaction)&bt_mole::actionFollowPathToWpt);
	}

	towptbox = -1;
	towptleave = -1;
	// current wpt
	curwpt = 0;
	CEntity* myEntity = myParent;

	mesh = myEntity->get<TCompRenderStaticMesh>();

	pose_idle_route = "static_meshes/mole/mole.static_mesh";
	pose_jump_route = "static_meshes/mole/mole_jump.static_mesh";
	pose_run_route = "static_meshes/mole/mole_run.static_mesh";
	pose_box_route = "static_meshes/mole/mole_box.static_mesh";
	pose_wall_route = "static_meshes/mole/mole_wall.static_mesh";
}

void bt_mole::update(float elapsed) {
	// Update transforms
	SetMyEntity();
	if (!myEntity) return;
	transform = myEntity->get<TCompTransform>();
	// If we become possessed, reset the tree and stop all actions
	if (possessing)
		setCurrent(NULL);
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
	return true;
}

// actions
int bt_mole::actionLookForWpt() {
	if (!SBB::readBool("sala1")) {
		return STAY;
	}
	VEC3 front = transform->getFront();
	VEC3 target = fixedWpts[curwpt];
	VEC3 initial = transform->getPosition();
	getPath(initial, target, "sala1");
	return OK;
}

int bt_mole::actionFollowPathToWpt() {
	if (!SBB::readBool("sala1")) {
		setCurrent(NULL);
		return OK;
	}
	VEC3 target = fixedWpts[curwpt];
	while (totalPathWpt > 0 && currPathWpt < totalPathWpt && fabsf(squaredDistXZ(pathWpts[currPathWpt], transform->getPosition())) < 0.5f) {
		++currPathWpt;
	}
	if (currPathWpt < totalPathWpt) {
		target = pathWpts[currPathWpt];
	}
	VEC3 npcPos = transform->getPosition();
	VEC3 npcFront = transform->getFront();
	if (needsSteering(npcPos + npcFront, transform, rotation_speed, myParent, "sala1")) {
		moveFront(speed);
		return STAY;
	}
	else if (!transform->isHalfConeVision(target, deg2rad(5.0f))) {
		aimToTarget(target);
		return STAY;
	}
	else {
		float distToWPT = squaredDistXZ(target, transform->getPosition());
		if (fabsf(distToWPT) > 0.5f && currPathWpt < totalPathWpt || fabsf(distToWPT) > 6.0f) {
			moveFront(speed);
			return STAY;
		}
		else {
			curwpt = (curwpt + 1) % fixedWpts.size();
			return OK;
		}
	}
}

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

			getPath(initial, destiny, "sala1");
			ChangePose(pose_run_route);
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
		if (needsSteering(npcPos + npcFront, transform, rotation_speed, myParent, "sala1", entTransform)) {
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
				ChangePose(pose_box_route);
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
			getPath(initial, destiny, "sala1");
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
	if (needsSteering(npcPos + npcFront, transform, rotation_speed, myParent, "sala1")) {
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
	ChangePose(pose_idle_route);

	TMsgLeaveBox msg;
	myBox.sendMsg(msg);

	return OK;
}

void bt_mole::_actionBeforePossession() {
	if (towptbox > -1 && carryingBox) {
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
			/*
			while (!b_t->executeMovement(posbox)) {
				angle += 0.1f;
				posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
				posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
			}*/
			SBB::postBool(key, false);
			carryingBox = false;
		}
	}
}

void bt_mole::_actionWhenStunt() {
	if (towptbox > -1 && carryingBox) {
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
				/*
				while (!b_t->executeMovement(posbox)) {
					angle += 0.1f;
					posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
					posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
				}
				*/
				SBB::postBool(key, false);
				carryingBox = false;
			}
		}
	}
}

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

bool bt_mole::aimToTarget(VEC3 target) {
	float delta_yaw = transform->getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		transform->getAngles(&yaw, &pitch);
		if (delta_yaw > 0.15) {
			transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
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

//Cambio de malla
void bt_mole::ChangePose(string new_pose_route) {
	if (last_pose != new_pose_route) {
		mesh->unregisterFromRender();
		MKeyValue atts_mesh;
		atts_mesh["name"] = new_pose_route;
		mesh->load(atts_mesh);
		mesh->registerToRender();
		last_pose = new_pose_route;
	}
}