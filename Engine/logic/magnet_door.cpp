#include "mcv_platform.h"
#include "magnet_door.h"

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
	moving = false;
	if (locked) targetOpened = atts.getPoint("target");
	else targetClosed = atts.getPoint("target");
	speedOpening = atts.getFloat("speedOpening", 1.f);
	speedClosing = atts.getFloat("speedClosing", speedOpening);
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

}

void magnet_door::onSetPolarity(const TMsgSetPolarity& msg)
{
	polarity = msg.polarity;
}
void magnet_door::onSetLocked(const TMsgSetLocked& msg)
{
	locked = msg.locked;
}