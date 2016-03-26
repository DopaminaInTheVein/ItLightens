#include "mcv_platform.h"
#include "bt_poss.h"
#include "components\entity.h"
#include "components\comp_transform.h"
#include "components\entity_tags.h"

bt_poss::bt_poss() {

	createRoot("possessable", PRIORITY, NULL, NULL);
	addChild("possessable", "possession", SEQUENCE, (btcondition)&bt_poss::beingPossessed, NULL);
	addChild("possession", "possessing", ACTION, NULL, (btaction)&bt_poss::actionPossessing);
	addChild("possession", "posessed", ACTION, NULL, (btaction)&bt_poss::actionPossessed);
	addChild("possessable", "unpossession", SEQUENCE, (btcondition)&bt_poss::beingUnpossessed, NULL);
	addChild("unpossession", "unpossessing", ACTION, NULL, (btaction)&bt_poss::actionUnpossessing);
	addChild("unpossession", "stunt", ACTION, NULL, (btaction)&bt_poss::actionStunt);
	addChild("unpossession", "stunt_end", ACTION, NULL, (btaction)&bt_poss::actionStuntEnd);

}

// MENSAJES
void bt_poss::onSetPossessed(const TMsgAISetPossessed& msg) {
	if (msg.possessed) {
		_actionBeforePossession();
		possessing = true;
	}
	else {
		stunning = true;
		possessed = false;
	}
}

void bt_poss::onSetStunned(const TMsgAISetStunned& msg) {
	dbg("ai_poss, recibe Set Stunned = %d\n", msg.stunned);
	stunned = msg.stunned;
	if (msg.stunned) {
		stunning = true;
	}
	else {
		stunned = false;
	}
}

//ACCIONES implementables
void bt_poss::_actionBeforePossession() {
	//Default: Nothing to do
	dbg("ai_poss::actionBeforePossession\n");
}

void bt_poss::_actionBeingUnpossessed() {
	dbg("Being Dispossessed");
	//Ejecutar animacion etc.
}

void bt_poss::_actionWhenStunt() {
	dbg("Being Stunt");
	//Ejecutar animacion etc.
}

//conditions
bool bt_poss::npcAvailable() {
	return isAvailable();
}
bool bt_poss::beingPossessed() {
	return possessing == true;
}
bool bt_poss::beingUnpossessed() {
	return stunning == true;
}
//actions
int bt_poss::actionPossessing() {
	
	CEntity* me;
	possessed = true;
	possessing = false;

	me = getMyEntity();
	TMsgControllerSetEnable msg;
	msg.enabled = true;
	me->sendMsg(msg);
	return OK;

}
int bt_poss::actionPossessed() {
	// Nothing to do
	return OK;
}
int bt_poss::actionUnpossessing() {
	possessed = false;
	stunning = false;
	stunned = true;
	____TIMER_RESET_(timeStunt);
	return OK;
}
int bt_poss::actionStunt() {
	_actionWhenStunt();
	if (timeStunt < 0) {
		stunned = false;
		return OK;
	}
	else {
		if (timeStunt > -1)
			timeStunt -= getDeltaTime();
		return STAY;
	}
}
int bt_poss::actionStuntEnd() {
	stunned = false;
	return OK;
}

void bt_poss::onStaticBomb(const TMsgStaticBomb & msg)
{
	CEntity *me = getMyEntity();
	TCompTransform *me_transform = me->get<TCompTransform>();
	VEC3 curr_pos = me_transform->getPosition();

	float d = squaredDist(msg.pos, curr_pos);

	if (d < msg.r) {
		stunned = true;
	}
}