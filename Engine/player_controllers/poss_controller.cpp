#include "mcv_platform.h"
#include "poss_controller.h"
#include "components/entity_tags.h"
#include "components/comp_transform.h"
#include "components/comp_name.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "components/comp_charactercontroller.h"

extern CHandle player;

PossController::PossController() {
	speedRecover = 5.0f;
}

void PossController::addPossStates() {
	AddState(ST_DISABLED, (statehandler)&PossController::DisabledState);
	AddState(ST_INIT_CONTROL, (statehandler)&PossController::InitControlState);
	ChangeState(ST_DISABLED);
}

void PossController::UpdatePossession() {
	if (npcIsPossessed) {
		if (controlEnabled && (controller->PossessionButtonBecomesPressed()) && possessionCooldown <= 0.0f) {
			CEntity* myParent = getMyEntity();
			TCompName * myParentName = myParent->get<TCompName>();
			string name = myParentName->name;
			TMsgAISetPossessed msg;
			msg.possessed = false;
			myParent->sendMsg(msg);
			UpdateUnpossess();
			onSetEnable(false);
			logic_manager->throwEvent(logic_manager->OnUnpossess, name);
		}
		else {
			possessionCooldown -= getDeltaTime();
		}
	}

	____TIMER_CHECK_DO_(timerShowEnergy);
	____TIMER_CHECK_DONE_(timerShowEnergy);
}

void PossController::onForceUnPosses(const TMsgUnpossesDamage& msg) {
	PROFILE_FUNCTION("poss controller: onUnposses");
	getUpdateInfo(); //You can receive a message without the update info loaded!
	UpdateUnpossess();
	/*TMsgDamageSave msg_unpss;
	myEntity->sendMsg(msg_unpss);
	msg_unpss.modif = -0.1f;*/
	onSetEnable(false);
}

void PossController::onSetEnable(const TMsgControllerSetEnable& msg) {
	onSetEnable(msg.enabled);
	/*if (msg.enabled) {
		TMsgDamageSave msg_pss;
		msg_pss.modif = 0.1f;
		getMyEntity()->sendMsg(msg_pss);
	}*/
}

void PossController::UpdateUnpossess() {
}

void PossController::onSetEnable(bool enabled) {
	//You can receive a message without the update info loaded!
	compBaseEntity = getMyEntity();
	getUpdateInfo();

	dbg("PossController::setEnable(%d)", enabled);
	npcIsPossessed = enabled;
	setControllable(enabled);
	player_curr_speed = 0;

	// Componentes y entidades para asignar Controlador y cámara
	CHandle camera3 = tags_manager.getFirstHavingTag(getID("camera_main"));
	CHandle hMe = CHandle(getMyEntity());
	CEntity *eMe = hMe;
	CEntity* e_camera = camera3;

	if (enabled) {
		// Avisar que se activa el control
		ChangeState(ST_INIT_CONTROL);

		//Set 3rd Person Controller
		TMsgSetTarget msg3rdController;
		msg3rdController.target = hMe;

		if (hMe.hasTag("AI_mole")) {
			msg3rdController.who = MOLE;
		}
		else if (hMe.hasTag("AI_cientifico")) {
			msg3rdController.who = SCIENTIST;
		}

		//set flag of controlled for physx queries and simulation collisions
		PxFilterData fd = cc->GetFilterData();
		fd.word0 = ItLightensFilter::ePLAYER_CONTROLLED;
		cc->SetFilterData(fd);

		e_camera->sendMsg(msg3rdController);
		//Set Camera
		camera = camera3;
	}
	else {
		CHandle hTarget = tags_manager.getFirstHavingTag(getID("raijin"));
		//CHandle hTarget = player;

		CEntity * camera3 = tags_manager.getFirstHavingTag(getID("camera_main"));
		TMsgSetTarget msgTarg;
		msgTarg.target = hTarget;
		msgTarg.who = PLAYER;
		camera3->sendMsg(msgTarg);

		//set flag of possessable for physx queries and simulation collisions
		PxFilterData fd = cc->GetFilterData();
		fd.word0 = ItLightensFilter::ePOSSESSABLE;
		cc->SetFilterData(fd);

		//Avisar que se ha deshabilitado
		ChangeState(ST_DISABLED);

		//Volver control al player
		TMsgPossessionLeave msg;
		msg.npcFront = transform->getFront();
		msg.npcPos = transform->getPosition();
		hTarget.sendMsg(msg);

		//Recover Tag Player
		TMsgSetTag msgTag;
		msgTag.add = true;
		msgTag.tag = "player";
		hTarget.sendMsg(msgTag);
	}
}

bool PossController::load_poss(MKeyValue& atts)
{
	npcIsPossessed = atts.getBool("possessed", false);
	return true;
}
bool PossController::save_poss(std::ofstream& os, MKeyValue& atts)
{
	if (npcIsPossessed) atts.put("possessed", npcIsPossessed);
	return true;
}
void PossController::init_poss()
{
	if (npcIsPossessed) {
		onSetEnable(true);
	}
}