#include "mcv_platform.h"
#include "poss_controller.h"
#include "logic/ai_poss.h"
#include "components/entity_tags.h"
#include "components/comp_transform.h"
#include "components/comp_name.h"
#include "app_modules\io\io.h"

extern CHandle player;

PossController::PossController() {
	AddState(ST_DISABLED, (statehandler)&PossController::DisabledState);
	AddState(ST_INIT_CONTROL, (statehandler)&PossController::InitControlState);
	ChangeState(ST_DISABLED);
	speedRecover = 5.0f;
}

void PossController::UpdatePossession() {
	if (npcIsPossessed) {
		if ((io->keys[VK_SHIFT].becomesPressed() || io->joystick.button_Y.becomesPressed()) && possessionCooldown <= 0.0f) {
			CEntity* myParent = getMyEntity();
			TCompName * myParentName = myParent->get<TCompName>();
			string name = myParentName->name;
			TMsgAISetPossessed msg;
			msg.possessed = false;
			myParent->sendMsg(msg);
			UpdateUnpossess();
			onSetEnable(false);
		}
		else {
			possessionCooldown -= getDeltaTime();
		}
	}
	else {
		energyDecreasal(-getDeltaTime()*speedRecover);
	}

	____TIMER_CHECK_DO_(timerShowEnergy);
	____TIMER_CHECK_DONE_(timerShowEnergy);
}

void PossController::onDamage(const TMsgDamage& msg) {
	PROFILE_FUNCTION("poss controller: onDamage");
	CEntity* myParent = getMyEntity();
	switch (msg.dmgType) {
	case LASER:		
		TMsgAISetPossessed msg;
		msg.possessed = false;
		myParent->sendMsg(msg);
		UpdateUnpossess();
		onSetEnable(false);
		break;
	case WATER:
		break;
	default:
		//nothing
		break;
	}
}

void PossController::onSetEnable(const TMsgControllerSetEnable& msg) {
	onSetEnable(msg.enabled);
}

void PossController::UpdateUnpossess() {
}

void PossController::onSetEnable(bool enabled) {
	dbg("PossController::setEnable(%d)", enabled);
	npcIsPossessed = enabled;
	this->controlEnabled = enabled;
	player_curr_speed = 0;

	// Componentes y entidades para asignar Controlador y cámara
	CHandle camera3 = tags_manager.getFirstHavingTag(getID("camera_main"));
	CHandle hMe = CHandle(getMyEntity());
	CEntity* e_camera = camera3;

	if (enabled) {
		// Avisar que se activa el control
		ChangeState(ST_INIT_CONTROL);

		//Set 3rd Person Controller
		TMsgSetTarget msg3rdController;
		msg3rdController.target = hMe;
		e_camera->sendMsg(msg3rdController);

		//Set Camera
		camera = camera3;
	}
	else {
		CHandle hTarget = tags_manager.getFirstHavingTag(getID("player"));
		//CHandle hTarget = player;

		CEntity * camera3 = tags_manager.getFirstHavingTag(getID("camera_main"));

		TMsgSetTarget msgTarg;
		msgTarg.target = hTarget;
		camera3->sendMsg(msgTarg);

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