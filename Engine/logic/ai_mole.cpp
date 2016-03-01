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

	waitSecondsToBoxRespawn = waitSeconds;
	timer = 0.0f;

	towptbox = 0;
	towptleave = 0;
	// reset the state
	ChangeState("idle");
}

void ai_mole::IdleState() {
	timer += getDeltaTime();
	if (timer >= waitSecondsToBoxRespawn) {
		timer = 0.0f;
		ChangeState("seekwpt");
	}
}

void ai_mole::SeekWptState() {
	if (SBB::readHandlesVector("wptsBoxes").size() > 0) {
		float distMax = 999999999.9999f;
		string key_final = "";
		bool found = false;
		for (int i = 0; i < SBB::readHandlesVector("wptsBoxes").size(); i++) {
			CEntity * entTransform = this->getEntityPointer(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
			string key = nameBox->name;
			if (!SBB::readBool(key) && disttowpt < distMax) {
				towptbox = i;
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
void ai_mole::NextWptState()
{
	TCompTransform * transformBox = this->getEntityPointer(towptbox)->get<TCompTransform>();
	TCompTransform * transform = getEntityTransform();
	float distToWPT = simpleDistXZ(transformBox->getPosition(), transform->getPosition());

	if (distToWPT > 1.0f) {
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

void ai_mole::GrabState() {
	ChangeState("seekwptcarry");
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
	if (distToWPT > 1.0f) {
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
	VEC3 new_posBox;
	new_posBox.x = (float)(rand() % 15);
	if (rand() % 2 == 0) {
		new_posBox.x *= -1;
	}
	new_posBox.z = (float)(rand() % 15);
	if (rand() % 2 == 0) {
		new_posBox.z *= -1;
	}
	vector<CHandle> newPointerVec = SBB::readHandlesVector("wptsBoxes");
	CEntity * en = newPointerVec[towptbox];
	TCompTransform * transformBox = en->get<TCompTransform>();
	TCompName * nameBox = en->get<TCompName>();
	transformBox->setPosition(new_posBox);
	SBB::postHandlesVector("wptsBoxes", newPointerVec);

	string key = nameBox->name;

	SBB::postBool(key, false);
	ChangeState("idle");
}

void ai_mole::_actionBeforePossession() {
	vector<CHandle> newPointerVec = SBB::readHandlesVector("wptsBoxes");
	CEntity * en = newPointerVec[towptbox];
	TCompName * nameBox = en->get<TCompName>();
	string key = nameBox->name;
	SBB::postBool(key, false);
}