#include "mcv_platform.h"
#include "poss_controller.h"
#include "ai_poss.h"
#include "components/components.h"

PossController::PossController() {
	AddState(ST_DISABLED, (statehandler)&PossController::DisabledState);
	AddState(ST_INIT_CONTROL, (statehandler)&PossController::InitControlState);
	ChangeState(ST_DISABLED);
}

void PossController::Recalc() {
	if (npcIsPossessed) {
		energyRemain -= speedSpend * getDeltaTime();
		if (energyRemain <= 0) {
			//TODO
			dbg("G A M E    O V E R \n");
		}

		if (GetAsyncKeyState(VK_ESCAPE)) {
			CEntity* myParent = getMyEntity();
			TMsgAISetPossessed msg;
			msg.possessed = false;
			myParent->sendMsg(msg);

			onSetEnable(false);
		}
	}
	else {
		energyRemain += speedRecover * getDeltaTime();
		if (energyRemain >= maxEnergy) energyRemain = maxEnergy;
	}

	____TIMER_CHECK_DO_(timerShowEnergy);
	dbg("PossController: EnergyRemain = %f\n", energyRemain);
	____TIMER_CHECK_DONE_(timerShowEnergy);

	aicontroller::Recalc();
}

void PossController::onSetEnable(const TMsgPossControllerSetEnable& msg) {
	onSetEnable(msg.enabled);
}

void PossController::onSetEnable(bool enabled) {
	dbg("PossController::setEnable(%d)", enabled);
	npcIsPossessed = enabled;
	this->enabled = enabled;
	if (enabled) ChangeState(ST_INIT_CONTROL);
	else ChangeState(ST_DISABLED);
}