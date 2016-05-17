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
	//polarity = atts.getString("pol", "neutral");
	speedOpening = atts.getFloat("speedOpening", 1.f);
	speedClosing = atts.getFloat("speedClosing", speedOpening);
	if (locked) targetOpened = atts.getPoint("target");
	else targetClosed = atts.getPoint("target");

	moving = prevMoving = false;
	prevLocked = locked;
	magneticBehaviour = prevMagneticBehaviour = MB_NONE;

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
}

void magnet_door::update(float elapsed)
{
	if (getUpdateInfo()) {
		//TODO:
		//updateMagneticBehaviour();
		//updateCinematicState();
		//updateMove();
		//notifyNewState();
	}
}

bool magnet_door::getUpdateInfo() {
	//My Info
	CHandle myEntity = CHandle(this).getOwner();
	if (!myEntity.isValid()) return false;
	CEntity* eMe = myEntity;
	transform = eMe->get<TCompTransform>();
	if (!transform) return false;
	physics = eMe->get<TCompPhysics>();
	if (!physics) return false;

	//Player Info
	CHandle player = tags_manager.getFirstHavingTag("player");
	if (!player.isValid()) return false;
	CEntity* ePlayer = myEntity;
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