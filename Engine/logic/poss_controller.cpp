#include "mcv_platform.h"
#include "poss_controller.h"
#include "ai_poss.h"
#include "components/entity_tags.h"
#include "components/comp_transform.h"

PossController::PossController() {
	AddState(ST_DISABLED, (statehandler)&PossController::DisabledState);
	AddState(ST_INIT_CONTROL, (statehandler)&PossController::InitControlState);
	ChangeState(ST_DISABLED);
	maxEnergy = 100.0f;
	energyRemain = 100.0f;
	speedRecover = 5.0f;
	speedSpend = 5.0f;
}

void PossController::UpdatePossession() {
	if (npcIsPossessed) {
		energyRemain -= speedSpend * getDeltaTime();
		if (energyRemain <= 0) {
			//TODO
			dbg("G A M E    O V E R \n");
		}

		if (Input.IsKeyPressed(DIK_LSHIFT)) {
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

void PossController::onSetEnable(const TMsgControllerSetEnable& msg) {
	onSetEnable(msg.enabled);
}

void PossController::onSetEnable(bool enabled) {
	dbg("PossController::setEnable(%d)", enabled);
	npcIsPossessed = enabled;
	this->controlEnabled = enabled;

	// Componentes y entidades para asignar Controlador y cámara
	CHandle hPlayer = tags_manager.getFirstHavingTag(getID("player"));
	CHandle hMe = CHandle(getMyEntity());
	CEntity* ePlayer = hPlayer;

	if (enabled) {
		// Avisar que se activa el control
		ChangeState(ST_INIT_CONTROL);

		//Set 3rd Person Controller
		TMsgSetTarget msg3rdController;
		msg3rdController.target = hMe;
		ePlayer->sendMsg(msg3rdController);

		//Set Camera
		camera = CHandle(ePlayer);
	}
	else {
		CHandle hTarget = tags_manager.getFirstHavingTag(getID("target"));
		CEntity* eTarget = hTarget;
		CEntity* eMe = hMe;
		TCompTransform* tMe = eMe->get<TCompTransform>();

		//Avisar que se ha deshabilitado
		ChangeState(ST_DISABLED);

		//Volver control al player
		TMsgPossessionLeave msg;
		msg.npcFront = tMe->getFront();
		msg.npcPos = tMe->getPosition();
		eTarget->sendMsg(msg);
	}
}