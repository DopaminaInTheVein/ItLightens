#include "mcv_platform.h"
#include "ai_poss.h"
#include "components\entity.h"

ai_poss::ai_poss() {

	AddState("idle", (statehandler)&ai_poss::idle);
	AddState(ST_POSSESSING, (statehandler)&ai_poss::PossessingState);
	AddState(ST_POSSESSED, (statehandler)&ai_poss::PossessedState);
	AddState(ST_UNPOSSESSING, (statehandler)&ai_poss::UnpossessingState);
	AddState(ST_STUNT, (statehandler)&ai_poss::StuntState);
	AddState(ST_STUNT_END, (statehandler)&ai_poss::_StuntEndState);
	possessed = false;
	stunned = false;
}

// MENSAJES
void ai_poss::onSetPossessed(const TMsgAISetPossessed& msg) {
	dbg("ai_poss, recibe Set Possessed = %d\n", msg.possessed);
	if (msg.possessed) {
		_actionBeforePossession();
		ChangeState(ST_POSSESSING);
	}
	else {
		ChangeState(ST_UNPOSSESSING);
	}
}

//ACCIONES implementables
void ai_poss::_actionBeforePossession() {
	//Default: Nothing to do
	dbg("ai_poss::actionBeforePossession\n");
}

const void ai_poss::PossessingState() {
	//Default: Nothing to do
	CEntity* me;
	switch (_actionBeingPossessed()) {
	case DONE:
		ChangeState(ST_POSSESSED);
		possessed = true;

		me = getMyEntity();
		TMsgControllerSetEnable msg;
		msg.enabled = true;
		me->sendMsg(msg);
		break;

	case IN_PROGRESS:
		break;

	case CANCEL:
		fatal("AI_POSS: Cancel result of actionBeingPossessed is not expected!");
		break;
	}
}

ACTION_RESULT ai_poss::_actionBeingPossessed() {
	return DONE;
}

const void ai_poss::PossessedState() {
	// Nothing to do?
}

ACTION_RESULT ai_poss::_actionBeingUnpossessed() {
	dbg("Being Dispossessed");
	//Ejecutar animacion etc.
	return DONE;
}

const void ai_poss::UnpossessingState() {
	switch (_actionBeingUnpossessed()) {
	case DONE:
		ChangeState(ST_STUNT);
		possessed = false;
		stunned = true;
		____TIMER_RESET_(timeStunt);
		//TODO: Enviar mensaje a la misma entidad para apagar PossController (declarado)
		break;

	case IN_PROGRESS:
		break;

	case CANCEL:
		fatal("AI_POSS: Cancel result of actionBeingDisPossessed is not expected!");
		break;
	}
}

const void ai_poss::StuntState() {
	actionStunt();
	____TIMER_CHECK_DO_(timeStunt);
	ChangeState(ST_STUNT_END);
	stunned = false;
	____TIMER_CHECK_DONE_(timeStunt);
}

// Things to do when is possessed
void ai_poss::actionStunt() {
	// Nothing at the moment
}

void ai_poss::_StuntEndState() {
	ChangeState("idle");
}