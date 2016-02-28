#include "mcv_platform.h"
#include "poss_controller.h"
#include "ai_poss.h"
#include "components/entity_tags.h"
#include "components/components.h"

PossController::PossController() {
	AddState(ST_DISABLED, (statehandler)&PossController::DisabledState);
	AddState(ST_INIT_CONTROL, (statehandler)&PossController::InitControlState);
	ChangeState(ST_DISABLED);
}

void PossController::UpdatePossession() {
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
}

void PossController::onSetEnable(const TMsgPossControllerSetEnable& msg) {
	onSetEnable(msg.enabled);
}

void PossController::onSetEnable(bool enabled) {
	dbg("PossController::setEnable(%d)", enabled);
	npcIsPossessed = enabled;
	this->controlEnabled = enabled;
	if (enabled) {
		// Avisar que se activa el control
		ChangeState(ST_INIT_CONTROL);

		// Componentes y entidades para asignar Controlador y cámara
		CHandle hPlayer = tags_manager.getFirstHavingTag(getID("player"));
		CHandle me = CHandle(getMyEntity());
		CEntity* ePlayer = hPlayer;
		//TCompCamera* pCamera = ePlayer->get<TCompCamera>();

		//Set 3rd Person Controller
		TMsgSetTarget msg3rdController;
		msg3rdController.target = me;
		ePlayer->sendMsg(msg3rdController);

		//Set Camera
		camera = CHandle(ePlayer);
	}
	else {
		ChangeState(ST_DISABLED);
	}
}