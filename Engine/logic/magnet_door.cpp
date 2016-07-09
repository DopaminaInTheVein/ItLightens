#include "mcv_platform.h"
#include "magnet_door.h"
#include "components/comp_msgs.h"

/*
enum eMagnetismBehaviour {
MB_OPENING,
MB_CLOSING,
MB_NONE
};

eMagnetismBehaviour mb = MB_NONE; //Magnetism Behaviour predomines. If MB_NONE --> openedLogic
bool locked;
bool moving;	//false false = opened, false true = opening
//true false = closed, true true = closing

VEC3 targetOpened, targetClosed;
pols polarity;
float speedOpening;
float speedClosing;
*/
bool magnet_door::load(MKeyValue& atts)
{
	locked = atts.getBool("locked", true);
	string polStr = atts.getString("pol", "neutral");
	if (polStr == "neutral") {
		polarity = NEUTRAL;
	}
	else if (polStr == "plus") {
		polarity = PLUS;
	}
	else if (polStr == "minus") {
		polarity = MINUS;
	}
	else {
		assert(false || fatal("Magnet door has an unknown polarity!\n"));
	}
	speedOpening = atts.getFloat("speedOpening", 5.f);
	speedClosing = atts.getFloat("speedClosing", speedOpening);
	if (locked) {
		targetOpened = atts.getPoint("target");
		cinematicState = prevCinematicState = CS_CLOSED;
	}
	else {
		targetClosed = atts.getPoint("target");
		cinematicState = prevCinematicState = CS_OPENED;
	}

	magneticBehaviour = MB_NONE;
	distPolarity = 5.f;
	epsilonTarget = 0.02f;
	return true;
}

void magnet_door::onCreate(const TMsgEntityCreated&)
{
	CHandle parent = CHandle(this).getOwner();
	CEntity* eMe = parent;
	assert(eMe);
	TCompTransform * transform = eMe->get<TCompTransform>();
	assert(transform);
	if (locked) targetClosed = transform->getPosition();
	else targetOpened = transform->getPosition();

	physics = eMe->get<TCompPhysics>();
	physics->setKinematic(true);
}

void magnet_door::update(float elapsed)
{
	if (!isInRoom(CHandle(this).getOwner()))return;

	updateMagneticBehaviour();
	updateCinematicState();
	updateMove();
	notifyNewState();
}

//Set magneticBehaviour to none, opening or closing
void magnet_door::updateMagneticBehaviour()
{
	// Default: Nothing to do because magnetism
	magneticBehaviour = MB_NONE;
	if (polarity != NEUTRAL && playerPolarity != NEUTRAL) {
		// Door is not neutral
		if (abs(playerTransform->getPosition().y - transform->getPosition().y) < 4.f) {
			//Door and player are on the same floor (Y distance)
			float distPlayerDoor = simpleDistXZ(targetClosed, playerTransform->getPosition());
			if (distPlayerDoor < distPolarity) {
				//Player is close to this door
				if (polarity != playerPolarity) {
					//Different polarity -> Attraction -> Close
					magneticBehaviour = MB_CLOSING;
				}
				else {
					//Same polarity -> Repulsion -> Open
					magneticBehaviour = MB_OPENING;
				}
			}
		}
	}
}

//Set opening or closing
void magnet_door::updateCinematicState()
{
	switch (magneticBehaviour) {
	case MB_NONE:
		cinematicState = locked ? CS_CLOSING : CS_OPENING;
		break;
	case MB_OPENING:
		cinematicState = CS_OPENING;
		break;
	case MB_CLOSING:
		cinematicState = CS_CLOSING;
		break;
	}
}

//Move door and set OPENED or CLOSED if movement ends
void magnet_door::updateMove()
{
	VEC3 target;
	float speed;
	eCinematicState targetState;

	// Set target
	if (cinematicState == CS_OPENING) {
		target = targetOpened;
		speed = speedOpening;
		targetState = CS_OPENED;
	}
	else {
		target = targetClosed;
		speed = speedClosing;
		targetState = CS_CLOSED;
	}
	PxRigidDynamic *rd = physics->getActor()->isRigidDynamic();
	if (rd) {
		PxTransform tmx = rd->getGlobalPose();
		VEC3 pos = PhysxConversion::PxVec3ToVec3(tmx.p);
		// Door has reached targed
		if (simpleDist(target, pos) < epsilonTarget) {
			cinematicState = targetState;
		}
		//Door has to move
		else {
			VEC3 delta = target - pos;
			float moveAmount = speed * getDeltaTime();
			delta.Normalize();
			VEC3 nextPos = pos + delta * moveAmount;
			PxVec3 pxTarget = PhysxConversion::Vec3ToPxVec3(nextPos);
			rd->setKinematicTarget(PxTransform(pxTarget, tmx.q));
		}
	}
}

void magnet_door::notifyNewState()
{
	if (prevCinematicState != cinematicState) {
		switch (cinematicState) {
		case CS_OPENING:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorOpening, "", myEntity);
			break;
		case CS_OPENED:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorOpened, "", myEntity);
			break;
		case CS_CLOSED:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorClosed, "", myEntity);
			break;
		case CS_CLOSING:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorClosing, "", myEntity);
			break;
		}
	}
	prevCinematicState = cinematicState;
}

bool magnet_door::getUpdateInfo() {
	//My Info
	myEntity = compBaseEntity;

	CEntity* eMe = myEntity;
	transform = eMe->get<TCompTransform>();
	if (!transform) return false;
	physics = eMe->get<TCompPhysics>();
	if (!physics) return false;

	//Player Info
	CHandle player = tags_manager.getFirstHavingTag("raijin");
	if (!player.isValid()) return false;
	CEntity* ePlayer = player;
	playerTransform = ePlayer->get<TCompTransform>();

	TMsgGetPolarity msgPol;
	msgPol.polarity = NEUTRAL;
	ePlayer->sendMsgWithReply(msgPol);
	playerPolarity = msgPol.polarity;

	return true;
}

void magnet_door::onSetPolarity(const TMsgSetPolarity& msg)
{
	polarity = msg.polarity;
}
void magnet_door::onSetLocked(const TMsgSetLocked& msg)
{
	locked = msg.locked;
}