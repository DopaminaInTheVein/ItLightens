#include "mcv_platform.h"
#include "ai_mole.h"

void ai_mole::Init()
{
	myHandle = CHandle(this);
	myParent = myHandle.getOwner();

	// insert all states in the map
	// insertar punters a funcions de una classe hereva del aiccontroller.
//	AddState("idle", (statehandler)&ai_mole::IdleState);
	AddState("seekwpt", (statehandler)&ai_mole::SeekWptState);
	AddState("orientTowpt", (statehandler)&ai_mole::OrientToWptState);
	AddState("nextwpt", (statehandler)&ai_mole::NextWptState);

	AddState("grab", (statehandler)&ai_mole::GrabState);
	AddState("seekwptcarry", (statehandler)&ai_mole::SeekWptCarryState);
	AddState("orientTowptCarry", (statehandler)&ai_mole::OrientToCarryWptState);
	AddState("nextwptCarry", (statehandler)&ai_mole::NextWptCarryState);
	AddState("ungrab", (statehandler)&ai_mole::UnGrabState);

	towptbox = -1;
	towptleave = -1;
	// reset the state
	ChangeState("idle");
}

void ai_mole::IdleState() {
	ChangeState("seekwpt");
}

void ai_mole::SeekWptState() {
	if (SBB::readHandlesVector("wptsBoxes").size() > 0) {
		float distMax = 15.0f;
		string key_final = "";
		bool found = false;
		float higher = -999.9f;
		for (int i = 0; i < SBB::readHandlesVector("wptsBoxes").size(); i++) {
			CEntity * entTransform = this->getEntityPointer(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
			string key = nameBox->name;
			if (!SBB::readBool(key) && !isBoxAtLeavePoint(wpt) && (disttowpt < distMax + 2 && wpt.y > higher)) {
				towptbox = i;
				higher = wpt.y;
				distMax = disttowpt;
				key_final = key;
				found = true;
			}
		}
		if (found) {
			SBB::postBool(key_final, true);
			SBB::postMole(key_final, this);
			ChangeState("orientTowpt");
		}
	}
}

void ai_mole::OrientToWptState()
{
	if (towptbox > -1) {
		CEntity * entTransform = this->getEntityPointer(towptbox);
		TCompTransform * transformBox = entTransform->get<TCompTransform>();
		TCompTransform * transform = getEntityTransform();
		if (!transform->isHalfConeVision(transformBox->getPosition(), deg2rad(0.01f))) {
			//ROTATE CAUSE WE DON'T SEE OBJECTIVE
			float angle = 0.0f;

			float littleAngle = transform->getDeltaYawToAimTo(transformBox->getPosition());
			float littleDeltaAngle = getDeltaTime() / 2;
			if (littleAngle < 0) {
				littleDeltaAngle *= -1;
				angle = max(littleAngle, littleDeltaAngle);
			}
			else {
				angle = min(littleAngle, littleDeltaAngle);
			}
			float yaw = 0.0f, pitch = 0.0f;
			transform->getAngles(&yaw, &pitch);
			transform->setAngles(yaw + angle, 0.0f);
		}
		else {
			ChangeState("nextwpt");
		}
	}
}
void ai_mole::NextWptState()
{
	if (towptbox > -1) {
		TCompTransform * transformBox = this->getEntityPointer(towptbox)->get<TCompTransform>();
		TCompTransform * transform = getEntityTransform();
		float distToWPT = simpleDistXZ(transformBox->getPosition(), transform->getPosition());

		if (distToWPT > 2.0f) {
			//MOVE
			VEC3 front = transform->getFront();
			VEC3 pos = transform->getPosition();
			pos.x += front.x*getDeltaTime() * 2;
			pos.z += front.z*getDeltaTime() * 2;
			transform->setPosition(pos);
		}
		else {
			ChangeState("grab");
		}
	}
}

void ai_mole::GrabState() {
	if (towptbox > -1) {
		TCompTransform * transform = getEntityTransform();
		CEntity* box = SBB::readHandlesVector("wptsBoxes")[towptbox];
		TCompTransform* box_t = box->get<TCompTransform>();
		VEC3 posbox = transform->getPosition();
		posbox.y += 2;
		box_t->setPosition(posbox);
		carryingBox = true;
		ChangeState("seekwptcarry");
	}
}
void ai_mole::SeekWptCarryState() {
	TCompTransform * transform = getEntityTransform();
	if (SBB::readHandlesVector("wptsBoxLeavePoint").size() > 0) {
		float distMax = 999999999.9999f;
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
		ChangeState("orientTowptCarry");
	}
}

void ai_mole::OrientToCarryWptState() {
	CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[towptleave];
	TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
	TCompTransform * transform = getEntityTransform();
	if (!transform->isHalfConeVision(wptbleavetransform->getPosition(), deg2rad(0.01f))) {
		//ROTATE CAUSE WE DON'T SEE OBJECTIVE
		float angle = 0.0f;
		float littleAngle = transform->getDeltaYawToAimTo(wptbleavetransform->getPosition());
		float littleDeltaAngle = getDeltaTime() / 2;
		if (littleAngle < 0) {
			littleDeltaAngle *= -1;
			angle = max(littleAngle, littleDeltaAngle);
		}
		else {
			angle = min(littleAngle, littleDeltaAngle);
		}
		float yaw = 0.0f, pitch = 0.0f;
		transform->getAngles(&yaw, &pitch);
		transform->setAngles(yaw + angle, 0.0f);
	}
	else {
		ChangeState("nextwptCarry");
	}
}

void ai_mole::NextWptCarryState() {
	TCompTransform * transformBox = this->getEntityPointer(towptbox)->get<TCompTransform>();
	CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[towptleave];
	TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
	TCompTransform * transform = getEntityTransform();
	float distToWPT = simpleDistXZ(wptbleavetransform->getPosition(), transform->getPosition());
	if (distToWPT > 2.0f) {
		//MOVE
		VEC3 front = transform->getFront();
		VEC3 pos = transform->getPosition();
		pos.x += front.x*getDeltaTime();
		pos.z += front.z*getDeltaTime();
		transform->setPosition(pos);
		VEC3 posBox = transformBox->getPosition();
		posBox.x += front.x*getDeltaTime();
		posBox.z += front.z*getDeltaTime();
		transformBox->setPosition(posBox);
	}
	else {
		ChangeState("ungrab");
	}
}

void ai_mole::UnGrabState() {
	if (towptbox > -1) {
		CEntity * enBox = SBB::readHandlesVector("wptsBoxes")[towptbox];
		CEntity * wptbleave = SBB::readHandlesVector("wptsBoxLeavePoint")[towptleave];
		TCompTransform * wptbleavetransform = wptbleave->get<TCompTransform>();
		TCompTransform * enBoxT = enBox->get<TCompTransform>();
		TCompName * nameBox = enBox->get<TCompName>();
		enBoxT->setPosition(wptbleavetransform->getPosition());
		VEC3 posbox = enBoxT->getPosition();
		VEC3 posboxIni = enBoxT->getPosition();
		TCompTransform * transform = getEntityTransform();

		float angle = 0.0f;
		while (!enBoxT->executeMovement(posbox)) {
			angle += 0.1;
			posbox.x = posboxIni.x + transform->getFront().x * cos(angle) * 3;
			posbox.z = posboxIni.z + transform->getFront().z * sin(angle) * 3;
		}
		SBB::postBool(nameBox->name, false);
		carryingBox = false;
	}
	ChangeState("idle");
}

void ai_mole::_actionBeforePossession() {
	if (towptbox > -1 && carryingBox) {
		vector<CHandle> newPointerVec = SBB::readHandlesVector("wptsBoxes");
		CEntity * en = newPointerVec[towptbox];
		TCompName * nameBox = en->get<TCompName>();
		string key = nameBox->name;

		ai_mole * mole = SBB::readMole(key);
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
				angle += 0.1;
				posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
				posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
			}
			SBB::postBool(key, false);
			carryingBox = false;
		}
	}
}

void ai_mole::actionStunt() {
	if (towptbox > -1 && carryingBox) {
		vector<CHandle> newPointerVec = SBB::readHandlesVector("wptsBoxes");
		CEntity * en = newPointerVec[towptbox];
		TCompName * nameBox = en->get<TCompName>();
		string key = nameBox->name;
		if (SBB::readBool(key)) {
			ai_mole * mole = SBB::readMole(key);
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
					angle += 0.1;
					posbox.x = posboxIni.x + p_t->getFront().x * cos(angle) * 3;
					posbox.z = posboxIni.z + p_t->getFront().z * sin(angle) * 3;
				}
				SBB::postBool(key, false);
				carryingBox = false;
			}
		}
	}
}

bool ai_mole::isBoxAtLeavePoint(VEC3 posBox) {
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