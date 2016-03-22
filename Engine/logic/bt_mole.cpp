#include "mcv_platform.h"
#include "bt_mole.h"

void bt_mole::Init()
{
	myHandle = CHandle(this);
	myParent = myHandle.getOwner();

	addChild("possessable", "mole", PRIORITY, (btcondition)&bt_mole::npcAvailable, NULL);
	addChild("mole", "movebox", SEQUENCE, (btcondition)&bt_mole::checkBoxes, NULL);
	addChild("movebox", "nextboxpt", ACTION, NULL, (btaction)&bt_mole::actionNextBoxWpt);
	addChild("movebox", "seekboxpt", ACTION, NULL, (btaction)&bt_mole::actionSeekBoxWpt);
	addChild("movebox", "grabbox", ACTION, NULL, (btaction)&bt_mole::actionGrabBox);
	addChild("movebox", "carryboxfindwpt", ACTION, NULL, (btaction)&bt_mole::actionCarryFindBoxWpt);
	addChild("movebox", "carryboxnextwpt", ACTION, NULL, (btaction)&bt_mole::actionCarryNextBoxWpt);
	addChild("movebox", "carryboxseekwpt", ACTION, NULL, (btaction)&bt_mole::actionCarrySeekBoxWpt);
	addChild("movebox", "ungrabbox", ACTION, NULL, (btaction)&bt_mole::actionUngrabBox);
	addChild("mole", "patrol", SEQUENCE, NULL, NULL);
	addChild("patrol", "nextWpt", ACTION, NULL, (btaction)&bt_mole::actionNextWpt);
	addChild("patrol", "seekwpt", ACTION, NULL, (btaction)&bt_mole::actionSeekWpt);

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
int bt_mole::actionNextWpt() {
	VEC3 front = transform->getFront();
	VEC3 target = fixedWpts[curwpt];
	bool aimed = aimToTarget(target);

	if (aimed) {
		ChangePose(pose_run_route);
		//ChangeState("seekwpt");
		return OK;
	}
	return STAY;
}

int bt_mole::actionSeekWpt() {

	float distance = squaredDistXZ(fixedWpts[curwpt], transform->getPosition());

	if (abs(distance) > 0.1f) {
		moveFront(speed);
		return STAY;
	}
	else {
		transform->setPosition(fixedWpts[curwpt]);
		float distance_to_next_wpt = squaredDist(transform->getPosition(), fixedWpts[(curwpt + 1) % fixedWpts.size()]);
		curwpt = (curwpt + 1) % fixedWpts.size();

		return OK;
	}
}

bool bt_mole::checkBoxes() {

	bool found = false;
	float minDistanceToBox = distMaxToBox;

	if (SBB::readHandlesVector("wptsBoxes").size() > 0) {

		string key_final = "";
		float higher = -999.9f;

		for (int i = 0; i < SBB::readHandlesVector("wptsBoxes").size(); i++) {
			CEntity * entTransform = this->getEntityPointer(i);
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
			}
		}

		if (found) {
			SBB::postBool(key_final, true);
			SBB::postMole(key_final, this);
			ChangePose(pose_idle_route);
			return true;
		}
	}
	return false;
}

int bt_mole::actionNextBoxWpt()
{
	if (towptbox > -1) {
		CEntity * entTransform = this->getEntityPointer(towptbox);
		TCompTransform * transformBox = entTransform->get<TCompTransform>();
		if (!transform->isHalfConeVision(transformBox->getPosition(), deg2rad(0.01f))) {

			bool aimed = aimToTarget(transformBox->getPosition());

			if (aimed) {
				ChangePose(pose_run_route);
				return OK;
			}
			else
				return STAY;
		}
		else {
			ChangePose(pose_run_route);
			return OK;
		}
	}
	return KO;
}

int bt_mole::actionSeekBoxWpt()
{
	if (towptbox > -1) {
		TCompTransform * transformBox = this->getEntityPointer(towptbox)->get<TCompTransform>();
		float distToWPT = simpleDistXZ(transformBox->getPosition(), transform->getPosition());

		if (abs(distToWPT) > 2.0f) {
			moveFront(speed);
			return STAY;
		}
		else {
			ChangePose(pose_box_route);
			return OK;
		}
	}
	return KO;
}

int bt_mole::actionGrabBox() {

	if (towptbox > -1) {
		CEntity* box = SBB::readHandlesVector("wptsBoxes")[towptbox];
		TCompTransform* box_t = box->get<TCompTransform>();
		VEC3 posbox = transform->getPosition();
		posbox.y += 2;
		box_t->setPosition(posbox);
		carryingBox = true;
		ChangePose(pose_idle_route);
		return OK;
	}
	ChangePose(pose_idle_route);
	return KO;
}

int bt_mole::actionCarryFindBoxWpt() {

	if (SBB::readHandlesVector("wptsBoxLeavePoint").size() > 0) {

		float distMax = D3D10_FLOAT32_MAX;
		for (int i = 0; i < SBB::readHandlesVector("wptsBoxLeavePoint").size(); i++) {
			CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[i];
			TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
			VEC3 wpt = wptbleavetransform->getPosition();
			float disttowpt = simpleDistXZ(wpt, transform->getPosition());
			if (disttowpt < distMax) {
				towptleave = i;
				distMax = disttowpt;
			}
		}
		ChangePose(pose_idle_route);
		return OK;
	}
	return KO;
}

int bt_mole::actionCarryNextBoxWpt() {

	CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[towptleave];
	TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
	if (!transform->isHalfConeVision(wptbleavetransform->getPosition(), deg2rad(0.01f))) {
		//ROTATE CAUSE WE DON'T SEE OBJECTIVE
		ChangePose(pose_box_route);
		bool aimed = aimToTarget(wptbleavetransform->getPosition());

		if (aimed) {
			ChangePose(pose_run_route);
			return OK;
		}
		else
			return STAY;
	}
	else {
		ChangePose(pose_run_route);
		return OK;
	}
	return KO;
}

int bt_mole::actionCarrySeekBoxWpt() {

	TCompTransform * transformBox = this->getEntityPointer(towptbox)->get<TCompTransform>();
	CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[towptleave];
	TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
	float distToWPT = simpleDistXZ(wptbleavetransform->getPosition(), transform->getPosition());
	if (distToWPT > 2.0f) {
		// Move Mole
		moveFront(speed);
		// Move box
		VEC3 front = transform->getFront();
		VEC3 posBox = transformBox->getPosition();
		posBox.x += front.x*speed*getDeltaTime();
		posBox.z += front.z*speed*getDeltaTime();
		transformBox->setPosition(posBox);
		return STAY;
	}
	else {
		ChangePose(pose_box_route);
		return OK;
	}
}

int bt_mole::actionUngrabBox() {
	if (towptbox > -1) {
		CEntity * enBox = SBB::readHandlesVector("wptsBoxes")[towptbox];
		CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[towptleave];
		TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
		TCompTransform * enBoxT = enBox->get<TCompTransform>();
		TCompName * nameBox = enBox->get<TCompName>();
		enBoxT->setPosition(wptbleavetransform->getPosition());
		VEC3 posbox = enBoxT->getPosition();
		VEC3 posboxIni = enBoxT->getPosition();

		float angle = 0.0f;
		while (!enBoxT->executeMovement(posbox)) {
			angle += 0.1f;
			posbox.x = posboxIni.x + transform->getFront().x * cos(angle) * 3;
			posbox.z = posboxIni.z + transform->getFront().z * sin(angle) * 3;
		}
		SBB::postBool(nameBox->name, false);
		carryingBox = false;
		ChangePose(pose_idle_route);
		return OK;
	}
	ChangePose(pose_idle_route);
	return KO;
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
			while (!b_t->executeMovement(posbox)) {
				angle += 0.1f;
				posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
				posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
			}
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
				while (!b_t->executeMovement(posbox)) {
					angle += 0.1f;
					posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
					posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
				}
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
			TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
			VEC3 wpt = wptbleavetransform->getPosition();
			float disttowpt = simpleDistXZ(wpt, posBox);
			if (disttowpt < 0.1f) {
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
		transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
		return false;
	}
	else {
		return true;
	}
}

void bt_mole::moveFront(float movement_speed) {
	VEC3 front = transform->getFront();
	VEC3 position = transform->getPosition();

	transform->setPosition(VEC3(position.x + front.x*movement_speed*getDeltaTime(), position.y, position.z + front.z*movement_speed*getDeltaTime()));
}

//Cambio de malla
void bt_mole::ChangePose(string new_pose_route) {

	mesh->unregisterFromRender();
	MKeyValue atts_mesh;
	atts_mesh["name"] = new_pose_route;
	mesh->load(atts_mesh);
	mesh->registerToRender();
}
